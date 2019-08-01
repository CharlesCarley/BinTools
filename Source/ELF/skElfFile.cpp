/*
-------------------------------------------------------------------------------

    Copyright (c) 2019 Charles Carley.

    Contributor(s): none yet.

-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#include "ELF/skElfFile.h"
#include <capstone/capstone.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "ELF/skElfSection.h"
#include "ELF/skElfSymbol.h"
#include "ELF/skElfUtils.h"
#include "Utils/skString.h"
#include "skSection.h"


skElfFile::skElfFile() :
    m_strtab(0)
{
    // Make sure this type of instance is visible in the base class.
    m_fileFormat = FF_ELF;

    skMemset(&m_header, 0, sizeof(skElfHeaderInfo64));
}

skElfFile::~skElfFile()
{
}


void skElfFile::loadImpl(void)
{
    if (m_data == 0)
    {
        printf("No data was loaded prior to calling load\n");
        return;
    }

    // Find the file's platform type.
    if (*(m_data + EMN_CLASS) == 1)
        m_fileFormatType = FFT_32BIT;
    else if (*(m_data + EMN_CLASS) == 2)
        m_fileFormatType = FFT_64BIT;
    else
    {
        printf("Unknown class descriptor found in the file header.\n");
        return;
    }

    if (m_fileFormatType == FFT_32BIT)
    {
        // Read as 32 bit then store as a 64bit type
        skElfHeaderInfo32 header;
        skMemcpy(&header, (char*)m_data, sizeof(skElfHeaderInfo32));
        skElfUtils::copyHeader(m_header, header);
    }
    else
    {
        skMemcpy(&m_header, (char*)m_data, sizeof(skElfHeaderInfo64));
    }


    // Handle the machine architecture
    switch (m_header.m_machine)
    {
    case EIA_SPARC:
        m_arch = IS_SPARC;
        break;
    case EIA_X86:
        m_arch = IS_X86;
        break;
    case EIA_MPS:
        m_arch = IS_MPS;
        break;
    case EIA_POWERPC:
        m_arch = IS_POWERPC;
        break;
    case EIA_S390:
        m_arch = IS_S390;
        break;
    case EIA_SUPERH:
        m_arch = IS_SUPERH;
        break;
    case EIA_IA64:
        m_arch = IS_IA64;
        break;
    case EIA_X8664:
        m_arch = IS_X8664;
        break;
    case EIA_AARCH64:
        m_arch = IS_AARCH64;
        break;
    case EIA_RISCV:
        m_arch = IS_RISCV;
        break;
    case EIA_NONE:
    default:
        m_arch = IS_NONE;
        break;
    }

    if (m_arch == IS_NONE)
    {
        printf("Unknown machine architecture found in the file header");
        return;
    }

    if (m_fileFormatType == FFT_32BIT)
    {
        loadSections<skElfSectionHeader32>();
        loadSymbolTable<skElfSymbol32>(".dynstr", ".dynsym");
        loadSymbolTable<skElfSymbol32>(".strtab", ".symtab");
    }
    else
    {
        loadSections<skElfSectionHeader64>();
        loadSymbolTable<skElfSymbol64>(".dynstr", ".dynsym");
        loadSymbolTable<skElfSymbol64>(".strtab", ".symtab");
    }
}


template <typename skElfSectionHeader>
void skElfFile::loadSections(void)
{
    elf64 offs, offe, i;

    if (!m_data || m_len == 0 || m_len == (size_t)-1)
        return;

    // The section headers are packed in m_data[start:end]
    offs = getSectionHeaderStart();
    offe = getSectionHeaderEnd();

    if (offe > m_len)
    {
        printf("Error - The section header's offset exceeds the amount of allocated memory.\n");
        return;
    }

    // Find the offset
    skElfSectionHeader* secPtr = (skElfSectionHeader*)(m_data + (offe - sizeof(skElfSectionHeader)));
    
    m_strtab = secPtr->m_offset;
    secPtr   = (skElfSectionHeader*)(m_data + offs);

    // Store each section one by one.
    for (i = 0; i < m_header.m_sectionTableEntryCount; ++i, secPtr++)
    {
        const skElfSectionHeader& sp = (*secPtr);

        SKsize sn = (SKsize)m_strtab + sp.m_name;
        if (sn < m_len)
        {
            char* name = (char*)(m_data + sn);

            // Skip the null entry
            if (name != 0 && (*name) == '\0')
                continue;

            if (m_sectionLookup.find(name) == SK_NPOS)
            {
                if (sp.m_offset + sp.m_size < m_len)
                {
                    skElfSectionHeader64 header;
                    if (m_fileFormatType == FFT_32BIT)
                        skElfUtils::copyHeader(header, *((skElfSectionHeader32*)secPtr));
                    else
                        skElfUtils::copyHeader(header, *((skElfSectionHeader64*)secPtr));

                    skSection* section = new skElfSection(
                        this,
                        name,
                        m_data + sp.m_offset,
                        (SKsize)sp.m_size,
                        (size_t)sp.m_offset,
                        header);

                    m_sectionLookup.insert(name, section);
                }
                else
                {
                    SKuint64 len = sp.m_offset + sp.m_size;
                    printf("Error - Section size exceeds the amount of allocated memory (%s, %llu).\n", name, len - m_len);
                }
            }
            else
            {
                // this is an error, it shouldn't have duplicate sections
                printf("Error - duplicate section name!\n");
            }
        }
    }
}


template <typename skElfSymbolHeader>
void skElfFile::loadSymbolTable(const char* strLookup, const char* symLookup)
{
    if (!strLookup || !symLookup)
        return;

    // Some symbols are nonexistent in a stripped binary (.strtab, .symtab)
 
    skElfSection* str = reinterpret_cast<skElfSection*>(getSection(strLookup));
    skElfSection* sym = reinterpret_cast<skElfSection*>(getSection(symLookup));

    if (sym && str)
    {
        SKuint64           i = 0;
        skElfSymbolHeader* symPtr;
        SKint8*            strPtr;

        const skElfSectionHeader64& hdr = sym->getHeader();

        symPtr = (skElfSymbolHeader*)sym->getPointer();
        strPtr = (SKint8*)str->getPointer();

        if (hdr.m_entSize == 0)
        {
            printf("Error - No entries in the string table.\n");
            return;
        }


        // Total number of elements
        SKuint64 entryLen = hdr.m_size / hdr.m_entSize;

        i = 0;
        while (i < entryLen)
        {
            const skElfSymbolHeader& syml = (*symPtr);

            // Make sure that the index is at least in range.
            if (syml.m_name >= str->getSize())
            {
                printf("Error - The size of the symbol name exceeds the size of the string table.\n");
                break;
            }

            char* cp = (char*)(strPtr + syml.m_name);
            if (*cp == '\0')
                ++cp;

            const skString& strl = cp;

            // Ensure that the parsed name at least has some info.
            if (!strl.empty())
            {
                SKsize idx = m_symTable.find(strl);
                if (idx == SK_NPOS)
                {
                    skElfSymbol64 sdp;
                    if (m_fileFormatType == FFT_32BIT)
                        skElfUtils::copyHeader(sdp, (*(skElfSymbol32*)symPtr));
                    else
                        skElfUtils::copyHeader(sdp, (*(skElfSymbol64*)symPtr));

                    skSymbol* elfSym = new skElfSymbol(this, strl, sdp);
                    m_symTable.insert(strl, elfSym);
                }
            }
            ++symPtr;
            i++;
        }
    }
}

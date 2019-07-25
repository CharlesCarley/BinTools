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
#include "skPrintUtils.h"
#include "skSection.h"


skElfFile::skElfFile()
{
    // make sure this type of instance
    // is visible in base class
    m_fileFormat = FF_ELF;

    m_symtab = 0;
    memset(&m_header, 0, sizeof(skElfHeaderInfo64));
}

skElfFile::~skElfFile()
{
}


void skElfFile::loadImpl(void)
{
    if (m_data == 0)
    {
        skPrintf("No data was loaded prior to calling load\n");
        return;
    }

    // Copy the header info and find the file's platform type.
    elf8 m_id[16];
    skMemcpy(&m_id, ((char*)m_data), 16);



    if (m_id[EMN_CLASS] == 1)
        m_fileFormatType = FFT_32BIT;
    else if (m_id[EMN_CLASS] == 2)
        m_fileFormatType = FFT_64BIT;
    else
    {
        skPrintf("Unknown class descriptor found in the file header.\n");
        return;
    }

    if (m_fileFormatType == FFT_32BIT)
    {
        // read as 32 bit then store as a 64bit type

        skElfHeaderInfo32 header;
        skMemcpy(&header, (char*)m_data, sizeof(skElfHeaderInfo32));
        skElfUtils::copyHeader(m_header, header);
    }
    else
    {
        skMemcpy(&m_header, (char*)m_data, sizeof(skElfHeaderInfo64));
    }


    // handle the machine architecture
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
        skPrintf("Unknown machine architecture found in the file header");
        return;
    }

    if (m_fileFormatType == FFT_32BIT)
    {
        loadSections<skElfSectionHeader32>();
        loadSymbolTable<skElfSymbol32>();
    }
    else
    {
        loadSections<skElfSectionHeader64>();
        loadSymbolTable<skElfSymbol64>();
    }

    loadDynSymbolTable();
}


template <typename skElfSectionHeader>
void skElfFile::loadSections(void)
{
    elf64 offs, offe;

    // The section headers are packed in m_data[start:end]
    offs = getSectionHeaderStart();
    offe = getSectionHeaderEnd();

    if (offe > m_len)
    {
        skPrintf("Error - The section header's offset exceeds the amount of allocated memory.\n");
        return;
    }


    if (!m_data || m_len == 0 || m_len == (size_t)-1)
        return;

    elf64 i;

    // Find the offset
    skElfSectionHeader* sectionBasePtr = (skElfSectionHeader*)(m_data + (offe - sizeof(skElfSectionHeader)));


    m_symtab = sectionBasePtr->m_offset;

    sectionBasePtr = (skElfSectionHeader*)(m_data + offs);

    // Store each section one by one.
    for (i = offs; i < offe && sectionBasePtr; i += sizeof(skElfSectionHeader), ++sectionBasePtr)
    {
        const skElfSectionHeader& sp = (*sectionBasePtr);

        SKsize sn = (SKsize)m_symtab + sp.m_name;
        if (sn < m_len)
        {
            elfName name = (elfName)(m_data + sn);

            // Skip the null entry
            if (name != 0 && (*name) == '\0')
                continue;

            if (m_sectionLookup.find(name) == SK_NPOS)
            {
                if (sp.m_offset + sp.m_size < m_len)
                {
                    skElfSectionHeader64 spd;
                    if (m_fileFormatType == FFT_32BIT)
                        skElfUtils::copyHeader(spd, *((skElfSectionHeader32*)sectionBasePtr));
                    else
                        skElfUtils::copyHeader(spd, *((skElfSectionHeader64*)sectionBasePtr));

                    skSection* section = new skElfSection(
                        this,
                        name,
                        m_data + sp.m_offset,
                        (SKsize)sp.m_size,
                        (size_t)sp.m_offset,
                        spd);

                    if (sp.m_flags & ESHT_EXEC_INST)
                        section->_setExectuable(true);

                    m_sectionLookup.insert(name, section);
                }
                else
                {
                    skPrintf("Error - Section size exceeds the amount of allocated memory.\n");
                }
            }
            else
            {
                // this is an error, it shouldn't have duplicate sections
                skPrintf("Error - duplicate section name!");
            }
        }
    }
}


template <typename skElfSymbolHeader>
void skElfFile::loadSymbolTable(void)
{
    skElfSection* strtab = reinterpret_cast<skElfSection*>(getSection(".strtab"));
    skElfSection* symtab = reinterpret_cast<skElfSection*>(getSection(".symtab"));

    if (symtab && strtab)
    {
        const skElfSectionHeader64& hdr = symtab->getHeader();


        skElfSymbolHeader* symPtr = (skElfSymbolHeader*)symtab->getPointer();
        SKint8*            strPtr = (SKint8*)strtab->getPointer();

        SKuint64 i = 0;

        // Skip past the initial NULL byte
        if (*(strPtr) == '\0')
            ++strPtr;


        // Build the string table array
        StringArray arr;
        arr.reserve((SKsize)hdr.m_entSize);

        for (i = 0; i < strtab->getSize();)
        {
            char*  cp = (char*)strPtr + i;
            SKsize sl = skStringUtils::length(cp);

            if (sl > 0)  // !!could misalign indices
                arr.push_back(skString(cp, sl));

            i += sl;
            ++i;  // skip past the null terminator
        }


        m_symTable.reserve((SKsize)hdr.m_entSize);
        i = 0;
        while (i < hdr.m_entSize)
        {
            const skElfSymbolHeader& sym = (*symPtr);

            // Make sure that the index is at least in range.
            if (sym.m_strTableIdx <= arr.size())
            {
                const skString& str = arr[sym.m_strTableIdx];

                // Ensure that the parsed name at least has some info.
                if (!str.empty())
                {
                    SKsize idx = m_symTable.find(str);
                    if (idx == SK_NPOS)
                    {
                        skElfSymbol64 sdp;

                        if (m_fileFormatType == FFT_32BIT)
                            skElfUtils::copyHeader(sdp, (*(skElfSymbol32*)symPtr));
                        else
                            skElfUtils::copyHeader(sdp, (*(skElfSymbol64*)symPtr));

                        skSymbol* elfSym = new skElfSymbol(this, str, sdp);
                        m_symTable.insert(str, elfSym);
                    }
                }
            }

            ++symPtr;
            i++;
        }
    }
}


void skElfFile::loadDynSymbolTable(void)
{
}

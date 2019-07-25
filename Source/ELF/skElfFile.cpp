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
    elf64 offs, offe, i;
    if (m_data == 0)
    {
        skPrintf("No data was loaded prior to calling load\n");
        return;
    }

    skMemcpy(&m_header, ((char*)m_data), sizeof(skElfHeaderInfo64));
    if (m_header.m_id[EMN_CLASS] == 1)
        m_fileFormatType = FFT_32BIT;
    else if (m_header.m_id[EMN_CLASS] == 2)
        m_fileFormatType = FFT_64BIT;
    else
    {
        skPrintf("Unknown class descriptor found in the file header.\n");
        return;
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

    // The section headers are packed in m_data[start:end]
    offs = getSectionHeaderStart();
    offe = getSectionHeaderEnd();

    if (offe > m_len)
    {
        skPrintf("Error - The section header's offset exceeds the amount of allocated memory.\n");
        return;
    }

    // Find the offset
    skElfSectionHeader64* sectionBasePtr = (skElfSectionHeader64*)(m_data + (offe - sizeof(skElfSectionHeader64)));
    

    m_symtab = sectionBasePtr->m_offset;

    sectionBasePtr = (skElfSectionHeader64*)(m_data + offs);


    // Preallocate to prevent unnecessary allocations.
    // Allocations expand with m_size*2 when m_size == m_capacity
    // TODO: remove this, just use: m_sectionLookup.iterator().getNext().first

    m_sectionHeaderStringTable.reserve((SKsize)((offe - offs) / sizeof(skElfSectionHeader64)));


    // Store each section one by one.
    for (i = offs; i < offe && sectionBasePtr; i += sizeof(skElfSectionHeader64), ++sectionBasePtr)
    {
        const skElfSectionHeader64& sp = (*sectionBasePtr);
        SKsize sn = (SKsize)getNameOffset(sp);

        if (sn < m_len)
        {
            elfName name = (elfName)(m_data + sn);

            // Skip the null entry
            if (name != 0 && (*name) == '\0')
                continue;

            if (m_sectionLookup.find(name) == SK_NPOS)
            {
                m_sectionHeaderStringTable.push_back(name);

                if (sp.m_offset + sp.m_size < m_len)
                {
                    skSection* section = new skElfSection(
                        this, name, m_data + sp.m_offset, (SKsize)sp.m_size, (size_t)sp.m_offset, sp);

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


    loadSymbolTable();
    loadDynSymbolTable();
}


void skElfFile::loadSymbolTable(void)
{
    skElfSection* strtab = reinterpret_cast<skElfSection*>(getSection(".strtab"));
    skElfSection* symtab = reinterpret_cast<skElfSection*>(getSection(".symtab"));

    if (symtab && strtab)
    {
        const skElfSectionHeader64& hdr = symtab->getHeader();

        skElfSymbol64* symPtr = (skElfSymbol64*)symtab->getPointer();
        SKint8*        strPtr = (SKint8*)strtab->getPointer();

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
            arr.push_back(skString(cp, sl));

            i += sl;
            ++i;  // skip past the null terminator
        }


        m_symTable.reserve(hdr.m_entSize);


        i = 0;
        while (i < hdr.m_entSize)
        {
            const skElfSymbol64& sym = (*symPtr++);

            // Make sure that the index is at least in range
            if (sym.m_strTableIdx <= arr.size())
            {
                const skString& str = arr[sym.m_strTableIdx];

                // ensure that it at least has info
                if (!str.empty()) 
                {
                    SKsize idx = m_symTable.find(str);
                    if (idx == SK_NPOS)
                    {
                        skSymbol* elfSym = new skElfSymbol(this, str, sym);
                        m_symTable.insert(str, elfSym);
                    }
                }
            }

            i++;
        }
    }
}


void skElfFile::loadDynSymbolTable(void)
{

}

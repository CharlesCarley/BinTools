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
#include "ELF/skElfUtils.h"
#include "Utils/skString.h"
#include "skPrintUtils.h"
#include "skSection.h"
#include "ELF/skElfSection.h"
#include "ELF/skElfSymbol.h"


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


template <typename skElfSectionHeader>
void skElfFile_BuildSectionGeneric(skBinaryFile::SectionMap& dest, skElfFile* parent, SKuint8* data, size_t len, elf64& symtab, elf64 offs, elf64 offe, int fmt)
{
    if (!data || len == 0 || len == (size_t)-1)
        return;

    elf64 i;

    // Find the offset
    skElfSectionHeader* sectionBasePtr = (skElfSectionHeader*)(data + (offe - sizeof(skElfSectionHeader)));


    symtab = sectionBasePtr->m_offset;

    sectionBasePtr = (skElfSectionHeader*)(data + offs);


    // Preallocate to prevent unnecessary allocations.
    // Allocations expand with m_size*2 when m_size == m_capacity
    // TODO: remove this, just use: m_sectionLookup.iterator().getNext().first

    //m_sectionHeaderStringTable.reserve((SKsize)((offe - offs) / sizeof(skElfSectionHeader)));


    // Store each section one by one.
    for (i = offs; i < offe && sectionBasePtr; i += sizeof(skElfSectionHeader), ++sectionBasePtr)
    {
        const skElfSectionHeader&   sp = (*sectionBasePtr);


        SKsize sn = (SKsize)symtab + sp.m_name;
        if (sn < len)
        {
            elfName name = (elfName)(data + sn);

            // Skip the null entry
            if (name != 0 && (*name) == '\0')
                continue;

            if (dest.find(name) == SK_NPOS)
            {
                //m_sectionHeaderStringTable.push_back(name);

                if (sp.m_offset + sp.m_size < len)
                {

                    skElfSectionHeader64 spd;
                    if (fmt == FFT_32BIT)
                        skElfUtils::copyHeader(spd, *((skElfSectionHeader32*)sectionBasePtr));
                    else
                        skElfUtils::copyHeader(spd, *((skElfSectionHeader64*)sectionBasePtr));
                    

                    skSection* section = new skElfSection(
                        parent, name, data + sp.m_offset, (SKsize)sp.m_size, (size_t)sp.m_offset, 
                        spd);

                    if (sp.m_flags & ESHT_EXEC_INST)
                        section->_setExectuable(true);

                    dest.insert(name, section);
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


void skElfFile::loadImpl(void)
{
    elf64 offs, offe;
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



    if (m_fileFormatType == FFT_32BIT)
    {
        skElfFile_BuildSectionGeneric<skElfSectionHeader32>(
            m_sectionLookup,
            this,
            (SKuint8*)m_data,
            m_len,
            m_symtab,
            offs,
            offe,
            m_fileFormatType);
    }
    else
    {
        skElfFile_BuildSectionGeneric<skElfSectionHeader64>(
            m_sectionLookup,
            this,
            (SKuint8*)m_data,
            m_len,
            m_symtab,
            offs,
            offe,
            m_fileFormatType);
    }

    loadSymbolTable();
    loadDynSymbolTable();
}


void skElfFile::loadSymbolTable(void)
{
    if (m_fileFormatType == FFT_32BIT)
        return;


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


        m_symTable.reserve((SKsize)hdr.m_entSize);


        i = 0;
        while (i < hdr.m_entSize)
        {
            const skElfSymbol64& sym = (*symPtr++);

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

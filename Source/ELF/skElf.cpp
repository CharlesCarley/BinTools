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
#include "ELF/skElf.h"
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include "ELF/skElfUtils.h"
#include "ELF/skElfSection.h"
#include "Utils/skString.h"
#include "skSection.h"


skElfFile::skElfFile()
{
    // make sure this type of instance
    // is visible in base class
    m_fileFormat = FF_ELF;

    m_symtab = 0;
    memset(&m_inf, 0, sizeof(skElfHeaderInfo64));
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

    skMemcpy(&m_inf, ((char*)m_data), sizeof(skElfHeaderInfo64));
    if (m_inf.m_id[EMN_CLASS] == 1)
        m_fileFormatType = FFT_32BIT;
    else if (m_inf.m_id[EMN_CLASS] == 2)
        m_fileFormatType = FFT_64BIT;
    else
    {
        skPrintf("Unknown class descriptor found in the file header.\n");
        return;
    }

    // handle the machine architecture
    switch (m_inf.m_machine)
    {
    case EIA_SPARC:
        m_instructionSetType = IS_SPARC;
        break;
    case EIA_X86:
        m_instructionSetType = IS_X86;
        break;
    case EIA_MPS:
        m_instructionSetType = IS_MPS;
        break;
    case EIA_POWERPC:
        m_instructionSetType = IS_POWERPC;
        break;
    case EIA_S390:
        m_instructionSetType = IS_S390;
        break;
    case EIA_SUPERH:
        m_instructionSetType = IS_SUPERH;
        break;
    case EIA_IA64:
        m_instructionSetType = IS_IA64;
        break;
    case EIA_X8664:
        m_instructionSetType = IS_X8664;
        break;
    case EIA_AARCH64:
        m_instructionSetType = IS_AARCH64;
        break;
    case EIA_RISCV:
        m_instructionSetType = IS_RISCV;
        break;
    case EIA_NONE:
    default:
        m_instructionSetType = IS_NONE;
        break;
    }

    if (m_instructionSetType == IS_NONE)
    {
        skPrintf("Unknown machine architecture found in the file header");
        return;
    }

    // The section headers are packed in m_data[start:end]
    offs = getSectionHeaderStart();
    offe = getSectionHeaderEnd();

    if (offe > m_len)
        return;


    // Offset to the start section
    skElfSectionHeader64* sp = reinterpret_cast<skElfSectionHeader64*>(m_data + offs);

    // Preallocate to prevent unnecessary allocations since
    // allocations expand with m_capacity*2 when m_size >  m_capacity
    m_sections.reserve((SKsize)((offe - offs) / sizeof(skElfSectionHeader64)));
    m_sectionHeaderStringTable.reserve(m_sections.capacity());


    for (i = offs; i < offe && sp; i += sizeof(skElfSectionHeader64), ++sp)
    {
        skElfSectionHeader64 spn;
        skMemcpy(&spn, sp, sizeof(skElfSectionHeader64));
        m_sections.push_back(spn);
    }

    if (!m_sections.empty())
        m_symtab = m_sections.back().m_offset;
    else
    {
        // This is an error, it should have sections
        skPrintf("Exception: No sections extracted from the file!");
        return;
    }

    // map sections
    SKsize si = 0, sl = m_sections.size(), sn;

    Sections::PointerType p = m_sections.ptr();
    while (si < sl)
    {
        skElfSectionHeader64& sh = p[si++];

        sn = (SKsize)getNameOffset(sh);
        if (sn < m_len)
        {
            elfName name = (elfName)(m_data + sn);
            if (name != 0 && (*name) == '\0')
                continue;

            if (m_sectionTable.find(name) == SK_NPOS)
            {
                m_sectionTable.insert(name, sh);
                m_sectionHeaderStringTable.push_back(name);

                if (sh.m_offset + sh.m_size < m_len)
                {
                    skSection* section = new skElfSection(
                        this, name, m_data + sh.m_offset, (SKsize)sh.m_size, (size_t)sh.m_offset, sh);

                    m_sectionLookup.insert(name, section);
                }
                else
                {
                    skPrintf("Exception: Section size exceeds the amount of memory allocated.\n");
                }
            }
            else
            {
                // this is an error, it shouldn't have duplicate symbols
                skPrintf("Exception: duplicate symbol name!");
            }
        }
    }
}

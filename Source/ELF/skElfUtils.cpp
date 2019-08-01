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
#include "ELF/skElfUtils.h"
#include "Utils/skMemoryUtils.h"



void skElfUtils::copyHeader(skElfHeaderInfo64& header, const skElfHeaderInfo32& header32Bit)
{
    // Always store the header as 64bit, and copy the members manually

    header.m_ver                    = header32Bit.m_ver;
    header.m_type                   = header32Bit.m_type;
    header.m_sectionTableIndex      = header32Bit.m_sectionTableIndex;
    header.m_sectionTableEntrySize  = header32Bit.m_sectionTableEntrySize;
    header.m_sectionTableEntryCount = header32Bit.m_sectionTableEntryCount;
    header.m_sectionOffset          = header32Bit.m_sectionOffset;
    header.m_programOffset          = header32Bit.m_programOffset;
    header.m_machine                = header32Bit.m_machine;
    header.m_headerSizeInBytes      = header32Bit.m_headerSizeInBytes;
    header.m_headerEntrySize        = header32Bit.m_headerEntrySize;
    header.m_headerEntryCount       = header32Bit.m_headerEntryCount;
    header.m_flags                  = header32Bit.m_flags;
    header.m_entry                  = header32Bit.m_entry;

    skMemcpy(header.m_id, header32Bit.m_id, 16);
}

void skElfUtils::copyHeader(skElfSectionHeader64& header, const skElfSectionHeader32& header32Bit)
{
    header.m_addr      = header32Bit.m_addr;
    header.m_addrAlign = header32Bit.m_addrAlign;
    header.m_entSize   = header32Bit.m_entSize;
    header.m_flags     = header32Bit.m_flags;
    header.m_info      = header32Bit.m_info;
    header.m_link      = header32Bit.m_link;
    header.m_name      = header32Bit.m_name;
    header.m_offset    = header32Bit.m_offset;
    header.m_size      = header32Bit.m_size;
    header.m_type      = header32Bit.m_type;
}

void skElfUtils::copyHeader(skElfSectionHeader64& header, const skElfSectionHeader64& header64Bit)
{
    skMemcpy(&header, &header64Bit, sizeof(skElfSectionHeader64));
}

void skElfUtils::copyHeader(skElfSymbol64& header, const skElfSymbol64& header64Bit)
{
    skMemcpy(&header, &header64Bit, sizeof(skElfSymbol64));
}

void skElfUtils::copyHeader(skElfSymbol64& header, const skElfSymbol32& header32Bit)
{
    header.m_info       = header32Bit.m_info;
    header.m_name       = header32Bit.m_name;
    header.m_other      = header32Bit.m_other;
    header.m_size       = header32Bit.m_size;
    header.m_sectionIdx = header32Bit.m_sectionIdx;
    header.m_value      = header32Bit.m_value;
}

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
#include "PE/skPortableSection.h"
#include "skPrintUtils.h"
#include "Utils/skDebugger.h"



skPortableSection::skPortableSection(skBinaryFile*      owner,
                                     const skString&    name,
                                     void*              data,
                                     size_t             size,
                                     size_t             offset,
                                     COFFSectionHeader& hdr) :
    skSection(owner, name, data, size, offset)
{
    skMemcpy(&m_header, &hdr, sizeof(COFFSectionHeader));

    //  Fix the header offset so that it points to the correct location
    m_header.m_pointerToRawData = offset;
}

skPortableSection::~skPortableSection()
{
}


void skPortableSection::printHeader(void)
{
    skPrintf("  Name:                  %s\n", m_header.m_name);
    skPrintf("  Virtual Size:          %u\n", m_header.m_virtualSize);
    skPrintf("  Virtual Address:       0x%08x\n", m_header.m_virtualAddress);
    skPrintf("  Size of Raw Data:      %u\n", m_header.m_sizeOfRawData);
    skPrintf("  Pointer To Raw Data:   0x%08x\n", m_header.m_pointerToRawData);
    skPrintf("  Relocation table:      %u\n", m_header.m_pointerToRelocations);
    skPrintf("  Relocation count:      %u\n", m_header.m_numberOfRelocations);
    skPrintf("  Line numbers:          %u\n", m_header.m_pointerToLineNumbers);
    skPrintf("  Line number count:     %u\n", m_header.m_numberOfLineNumbers);
    skPrintf("  Characteristics:       0x%08x\n", m_header.m_characteristics);
    skPrintf("  Size of Header:        %u\n", sizeof(COFFSectionHeader));
}


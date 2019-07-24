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
#include "PE/skPortableUtils.h"
#include "Utils/skDebugger.h"
#include <time.h>


void skPortableUtils::printCommonHeader(const COFFHeader& header)
{

    char* result = ctime((time_t*)&header.m_timeDateStamp);

    skPrintf("  Machine:              %u\n", header.m_machine);
    skPrintf("  Section Count:        %u\n", header.m_sectionCount);

    if (result != 0)
        skPrintf("  Timestamp:            %s", result);
    else
        skPrintf("  Timestamp:            %u\n", header.m_timeDateStamp);

    skPrintf("  Symbol Table Offset:  %u\n", header.m_symbolTableOffset);
    skPrintf("  Number Of Symbols:    %u\n", header.m_symbolCount);
    skPrintf("  Optional Header Size: %u\n", header.m_optionalHeaderSize);
    skPrintf("  Characteristics:      0x%x\n", header.m_characteristics);
}



void skPortableUtils::printSectionHeader(const COFFSectionHeader& header)
{
    skPrintf("  Name:                  %s\n", header.m_name);
    skPrintf("  Virtual Size:          %u\n", header.m_virtualSize);
    skPrintf("  Virtual Address:       0x%x\n", header.m_virtualAddress);
    skPrintf("  Size of Raw Data:      %u\n", header.m_sizeOfRawData);
    skPrintf("  Pointer To Raw Data:   0x%x\n", header.m_pointerToRawData);
    skPrintf("  Relocation table:      %u\n", header.m_pointerToRelocations);
    skPrintf("  Relocation count:      %u\n", header.m_numberOfRelocations);
    skPrintf("  Line numbers:          %u\n", header.m_pointerToLineNumbers);
    skPrintf("  Line number count:     %u\n", header.m_numberOfLineNumbers);
    skPrintf("  Characteristics:       0x%x\n", header.m_characteristics);
    skPrintf("  Size of Header:        %u\n", (SKuint32)sizeof(COFFSectionHeader));
}

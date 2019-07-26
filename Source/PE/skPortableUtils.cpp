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

#define _CRT_SECURE_NO_WARNINGS
#include "PE/skPortableUtils.h"
#include <time.h>
#include "Utils/skDebugger.h"



void skPortableUtils::printHeader(const COFFHeader& header)
{
    skPrintf("  Machine:                    %u\n", header.m_machine);
    skPrintf("  Section Count:              %u\n", header.m_sectionCount);

    char buf[32];
    strftime(buf, 32, "%D %r", localtime((time_t*)&header.m_timeDateStamp));
    if (buf[0] != 0)
        skPrintf("  Timestamp:                  %s\n", buf);
    else
        skPrintf("  Timestamp:                  %u\n", header.m_timeDateStamp);
        
    skPrintf("  Symbol Table Offset:        %u\n", header.m_symbolTableOffset);
    skPrintf("  Number Of Symbols:          %u\n", header.m_symbolCount);
    skPrintf("  Optional Header Size:       %u\n", header.m_optionalHeaderSize);
    skPrintf("  Characteristics:            0x%x\n", header.m_characteristics);
}

void skPortableUtils::printHeader(const COFFSectionHeader& header)
{
    skPrintf("  Name:                       %s\n", header.m_name);
    skPrintf("  Virtual Size:               %u\n", header.m_virtualSize);
    skPrintf("  Virtual Address:            0x%x\n", header.m_virtualAddress);
    skPrintf("  Size of Raw Data:           %u\n", header.m_sizeOfRawData);
    skPrintf("  Pointer To Raw Data:        0x%x\n", header.m_pointerToRawData);
    skPrintf("  Relocation Table:           %u\n", header.m_pointerToRelocations);
    skPrintf("  Relocation Count:           %u\n", header.m_numberOfRelocations);
    skPrintf("  Line Number Location:       0x%x\n", header.m_pointerToLineNumbers);
    skPrintf("  Line Number Count:          %u\n", header.m_numberOfLineNumbers);
    skPrintf("  Characteristics:            0x%x\n", header.m_characteristics);
    skPrintf("  Size of Header:             %u\n", (SKuint32)sizeof(COFFSectionHeader));
}


template <typename COFFOptionalHeaderVaryingBase, typename SKuintV>
void skPortableUtils_printHeader(const COFFOptionalHeader<COFFOptionalHeaderVaryingBase, SKuintV>& header)
{
    skPrintf("  ImageBase:                  0x%llx\n", (SKuint64)header.m_imageBase);
    skPrintf("  Section Alignment:          %u\n", header.m_sectionAlignment);
    skPrintf("  File Alignment:             %u\n", header.m_fileAlignment);
    skPrintf("  Operating System Version:   %u.%u\n", header.m_majOpSysVersion, header.m_minOpSysVersion);
    skPrintf("  Image Version:              %u.%u\n", header.m_majImSysVersion, header.m_minImSysVersion);
    skPrintf("  Sub System Version:         %u.%u\n", header.m_majSubSysVersion, header.m_minSubSysVersion);
    skPrintf("  Image Size:                 %u\n", header.m_sizeOfImage);
    skPrintf("  Size of Headers:            %u\n", header.m_sizeOfHeaders);
    skPrintf("  Checksum:                   %u\n", header.m_checkSum);
    skPrintf("  Subsystem:                  %u\n", header.m_subsystem);
    skPrintf("  DLL Characteristics:        0x%llx\n", (SKuint64)header.m_dllCharacteristics);
    skPrintf("  Stack Reserve Size:         %llu\n", (SKuint64)header.m_sizeOfStackReserve);
    skPrintf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfStackCommit);
    skPrintf("  Stack Heap Reserve:         %llu\n", (SKuint64)header.m_sizeOfHeapReserve);
    skPrintf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfHeapCommit);
    skPrintf("  Loader Flags:               0x%x\n", header.m_loaderFlags);
    skPrintf("  RVA and Size Count:         %u\n", header.m_numberOfRvaAndSizes);

    skPrintf("  Export Table:               0x%llx\n", (SKuint64)header.m_exportTable);
    skPrintf("  Import Table:               0x%llx\n", (SKuint64)header.m_importTable);
    skPrintf("  Resource Table:             0x%llx\n", (SKuint64)header.m_resourceTable);
    skPrintf("  Exception Table:            0x%llx\n", (SKuint64)header.m_exceptionTable);
    skPrintf("  Certificate Table:          0x%llx\n", (SKuint64)header.m_certificateTable);
    skPrintf("  Base Relocation Table:      0x%llx\n", (SKuint64)header.m_baseRelocationTable);
    skPrintf("  Debug Table:                0x%llx\n", (SKuint64)header.m_debug);
    skPrintf("  Architecture (Reserved):    %llu\n", (SKuint64)header.m_architecture);
    skPrintf("  Global Pointer Register:    %llu\n", (SKuint64)header.m_globPtrReg);
    skPrintf("  Thread Local Storage:       0x%llx\n", (SKuint64)header.m_threadLocalStorage);
    skPrintf("  Loader Config Table:        0x%llx\n", (SKuint64)header.m_loadConfigTable);
    skPrintf("  Bound Import Table:         0x%llx\n", (SKuint64)header.m_boundImport);
    skPrintf("  Import Address Table:       0x%llx\n", (SKuint64)header.m_importAddressTable);
    skPrintf("  Delay Import Descriptor:    0x%llx\n", (SKuint64)header.m_delayImportDescriptor);
    skPrintf("  CRT Runtime Header:         0x%llx\n", (SKuint64)header.m_crtRuntimeHeader);
    skPrintf("  (Reserved):                 %llu\n", (SKuint64)header.m_reserved);
}


void skPortableUtils::printHeader(const COFFOptionalHeader32& header)
{
    skPortableUtils_printHeader<COFFOptionalHeaderCommonPE32, SKuint32>(header);
}


void skPortableUtils::printHeader(const COFFOptionalHeader64& header)
{
    skPortableUtils_printHeader<COFFOptionalHeaderCommonPE64, SKuint64>(header);
}

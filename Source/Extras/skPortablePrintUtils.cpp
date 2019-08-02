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
#include "Utils/skDebugger.h"
#include "Utils/skTimer.h"
#include "skPrintUtils.h"



void skPortableUtils::printHeader(const COFFHeader& header)
{
    SKuint32 bw;
    char   buf[32];

    
    skPrintf("  Machine:                    %u\n", header.m_machine);
    skPrintf("  Section Count:              %u\n", header.m_sectionCount);

    bw = skGetTimeString(buf, 32, "%D %r", header.m_timeDateStamp);
    if (bw != SK_NPOS)
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

void skPrintDataDir(const char* msg, const COFFDataDirectory& dd)
{
    if (dd.m_size)
        skPrintf("%s0x%x,%u\n", msg, dd.m_virtualAddress, dd.m_size);
    else
        skPrintf("%s0\n", msg);
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
    skPrintf("  Image Size:                 0x%x,%u\n", header.m_sizeOfImage, header.m_sizeOfImage);
    skPrintf("  Size of Headers:            %u\n", header.m_sizeOfHeaders);
    skPrintf("  Checksum:                   %u\n", header.m_checkSum);
    skPrintf("  Subsystem:                  %u\n", header.m_subsystem);
    skPrintf("  DLL Characteristics:        0x%llx\n", (SKuint64)header.m_dllCharacteristics);
    skPrintf("  Stack Reserve Size:         %llu\n", (SKuint64)header.m_sizeOfStackReserve);
    skPrintf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfStackCommit);
    skPrintf("  Stack Heap Reserve:         %llu\n", (SKuint64)header.m_sizeOfHeapReserve);
    skPrintf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfHeapCommit);
    skPrintf("  Loader Flags:               0x%x\n", header.m_loaderFlags);
    skPrintf("  RVA and Size Count:         %u\n\n", header.m_numberOfRvaAndSizes);


    skPrintUtils::writeColor(CS_DARKYELLOW);
    skPrintf("Data Directories\n\n");
    skPrintUtils::writeColor(CS_LIGHT_GREY);


    const COFFDataDirectories& dir = header.m_directories;



    skPrintDataDir("  Export Table:               ", dir.m_exportTable);
    skPrintDataDir("  Import Table:               ", dir.m_importTable);
    skPrintDataDir("  Resource Table:             ", dir.m_resourceTable);
    skPrintDataDir("  Exception Table:            ", dir.m_exceptionTable);
    skPrintDataDir("  Certificate Table:          ", dir.m_certificateTable);
    skPrintDataDir("  Base Relocation Table:      ", dir.m_baseRelocationTable);
    skPrintDataDir("  Debug Table:                ", dir.m_debug);
    skPrintDataDir("  Architecture (Reserved):    ", dir.m_architecture);
    skPrintDataDir("  Global Pointer Register:    ", dir.m_globPtrReg);
    skPrintDataDir("  Thread Local Storage:       ", dir.m_threadLocalStorage);
    skPrintDataDir("  Loader Config Table:        ", dir.m_loadConfigTable);
    skPrintDataDir("  Bound Import Table:         ", dir.m_boundImport);
    skPrintDataDir("  Import Address Table:       ", dir.m_importAddressTable);
    skPrintDataDir("  Delay Import Descriptor:    ", dir.m_delayImportDescriptor);
    skPrintDataDir("  CRT Runtime Header:         ", dir.m_crtRuntimeHeader);
}


void skPortableUtils::printHeader(const COFFOptionalHeader32& header)
{
    skPortableUtils_printHeader<COFFOptionalHeaderCommonPE32, SKuint32>(header);
}


void skPortableUtils::printHeader(const COFFOptionalHeader64& header)
{
    skPortableUtils_printHeader<COFFOptionalHeaderCommonPE64, SKuint64>(header);
}

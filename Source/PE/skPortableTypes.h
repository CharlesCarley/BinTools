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

    See: https://docs.microsoft.com/en-us/windows/win32/debug/pe-format

*/
#ifndef _skPortableTypes_h_
#define _skPortableTypes_h_

#include "Utils/Config/skConfig.h"

enum COFFMagic
{
    COFF_MAG_PE32 = 0x010B,
    COFF_MAG_PE64 = 0x020B,  // PE32+
};

enum COFFMachineType
{
    CMT_NONE  = 0,
    CMT_AMD64 = 0x01D3,
    CMT_ARM   = 0x01C0,
    CMT_ARM64 = 0xAA64,
    CMT_I386  = 0x014C,
    CMT_IA64  = 0x0200,
};



enum COFFSectionCharacteristics
{
    CSC_HAS_CODE    = 0x00000020,
    CSC_CAN_EXECUTE = 0x20000000
};


struct COFFHeader
{
    SKuint16 m_machine;
    SKuint16 m_sectionCount;
    SKuint32 m_timeDateStamp;
    SKuint32 m_symbolTableOffset;
    SKuint32 m_symbolCount;
    SKuint16 m_optionalHeaderSize;  // required for images
    SKuint16 m_characteristics;
};


struct COFFOptionalHeaderCommon
{
    SKuint16 m_magic;  // PE32 | PE32+ format
    SKuint8  m_majorVersion;
    SKuint8  m_minorVersion;
    SKuint32 m_sizeofCode;
    SKuint32 m_sizeofInitData;
    SKuint32 m_sizeofBSSdData;
    SKuint32 m_entryPoint;
    SKuint32 m_baseOfCode;
};


struct COFFOptionalHeaderCommonPE32 : COFFOptionalHeaderCommon
{
    SKuint32 m_baseOfData;
};


struct COFFOptionalHeaderCommonPE64 : COFFOptionalHeaderCommon
{
    // Intentionally blank. The member baseOfData is absent in PE32+ 
};



struct COFFDataDirectory 
{
    SKuint32 m_virtualAddress;
    SKuint32 m_size;
};


enum COFFDirectoryEnum
{
    CDE_UNKNOWN = -1,          // initial flag
    CDE_EXPORT,                // .edata section
    CDE_IMPORT,                // .idata
    CDE_RESOURCE,              // .rsrc
    CDE_EXCEPTION,             // .pdata
    CDE_CERTIFICATE,           // Attribute certificate table
    CDE_BASE_RELOCATION,       // .reloc
    CDE_DEBUG,                 // .debug
    CDE_ARCHITECTURE,          // (reserved)
    CDE_GLOBAL_PTR,            // global ptr resister
    CDE_THREAD_LOCAL_STORAGE,  // .tls
    CDE_LOAD_CONFIG,           // load configuration
    CDE_BOUND_IMPORT,          // bound import table
    CDE_IMPORT_ADDRESS_TABLE,  // import address table
    CDE_DELAY_IMPORT_DESC,     // delay load import
    CDE_CRT_RUNTIME_HEADER,    //.cormeta
    CDE_RESERVED,
    CDE_MAX
};


struct COFFDataDirectories 
{
    COFFDataDirectory m_exportTable;
    COFFDataDirectory m_importTable;
    COFFDataDirectory m_resourceTable;
    COFFDataDirectory m_exceptionTable;
    COFFDataDirectory m_certificateTable;
    COFFDataDirectory m_baseRelocationTable;
    COFFDataDirectory m_debug;
    COFFDataDirectory m_architecture;
    COFFDataDirectory m_globPtrReg;
    COFFDataDirectory m_threadLocalStorage;
    COFFDataDirectory m_loadConfigTable;
    COFFDataDirectory m_boundImport;
    COFFDataDirectory m_importAddressTable;
    COFFDataDirectory m_delayImportDescriptor;
    COFFDataDirectory m_crtRuntimeHeader;
    COFFDataDirectory m_reserved;
};


template <typename COFFOptionalHeaderVaryingBase, typename SKuintV>
struct COFFOptionalHeader : COFFOptionalHeaderVaryingBase
{
    SKuintV  m_imageBase;
    SKuint32 m_sectionAlignment;
    SKuint32 m_fileAlignment;
    SKuint16 m_majOpSysVersion;
    SKuint16 m_minOpSysVersion;
    SKuint16 m_majImSysVersion;
    SKuint16 m_minImSysVersion;
    SKuint16 m_majSubSysVersion;
    SKuint16 m_minSubSysVersion;
    SKuint32 m_wi32VerReserved;
    SKuint32 m_sizeOfImage;
    SKuint32 m_sizeOfHeaders;
    SKuint32 m_checkSum;
    SKuint16 m_subsystem;
    SKuint16 m_dllCharacteristics;
    SKuintV  m_sizeOfStackReserve;
    SKuintV  m_sizeOfStackCommit;
    SKuintV  m_sizeOfHeapReserve;
    SKuintV  m_sizeOfHeapCommit;
    SKuint32 m_loaderFlags;
    SKuint32 m_numberOfRvaAndSizes;


    // Header Data Directories
    COFFDataDirectories m_directories;
};


typedef COFFOptionalHeader<COFFOptionalHeaderCommonPE32, SKuint32> COFFOptionalHeader32;
typedef COFFOptionalHeader<COFFOptionalHeaderCommonPE64, SKuint64> COFFOptionalHeader64;



struct COFFSectionHeader
{
    SKuint8  m_name[8];
    SKuint32 m_virtualSize;
    SKuint32 m_virtualAddress;
    SKuint32 m_sizeOfRawData;
    SKuint32 m_pointerToRawData;
    SKuint32 m_pointerToRelocations;
    SKuint32 m_pointerToLineNumbers;
    SKuint16 m_numberOfRelocations;
    SKuint16 m_numberOfLineNumbers;
    SKuint32 m_characteristics;
};



struct COFFResourceDirectory
{
    SKuint32 m_characteristics;
    SKuint32 m_timeStamp;
    SKuint16 m_majorVersion;
    SKuint16 m_minorVersion;
    SKuint16 m_namedEntryCount;
    SKuint16 m_idEntryCount;
};


struct COFFImportDirectoryTable
{
    SKuint32 m_nameHintRVA;
    SKuint32 m_timeStamp;
    SKuint32 m_forwardChain;
    SKuint32 m_nameRVA;
    SKuint32 m_iatAddress;
};





// Define Utils_USE_COMPILER_CHECKS via CMake to check computed sizes during build.
// If any of these fail the compiler will spit a negative subscript error.
SK_ASSERTCOMP(COFFOptionalHeaderCommon_sizeof, sizeof(COFFOptionalHeaderCommon) == 24);
SK_ASSERTCOMP(COFFOptionalHeader32_sizeof, sizeof(COFFOptionalHeader32) == 224);
SK_ASSERTCOMP(COFFOptionalHeader64_sizeof, sizeof(COFFOptionalHeader64) == 240);
SK_ASSERTCOMP(COFFOptionalHeaderCommonPE32_sizeof, sizeof(COFFOptionalHeaderCommonPE32) == 28);
SK_ASSERTCOMP(COFFOptionalHeaderCommonPE64_sizeof, sizeof(COFFOptionalHeaderCommonPE64) == 24);
SK_ASSERTCOMP(COFFSectionHeader_sizeof, sizeof(COFFSectionHeader) == 40);
SK_ASSERTCOMP(SKuint64_sizeof, sizeof(SKuint64) == 8);
SK_ASSERTCOMP(COFFDataDirectory_sizeof, sizeof(COFFDataDirectory) == 8);



class skPortableFile;
class skPortableSection;
class skPortableDirectory;



#endif  //_skPortableTypes_h_


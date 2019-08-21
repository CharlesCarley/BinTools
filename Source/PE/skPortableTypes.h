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
    CMT_AMD64 = 0x8664, // x64
    CMT_ARM   = 0x01C0, // ARM little endian (byte swapping is not implemented)
    CMT_ARM64 = 0xAA64, // ARM64 little endian 
    CMT_I386  = 0x014C, // Intel 386 or later processors.
    CMT_IA64  = 0x0200, // Intel Itanium processor family.
};




enum COFFCharacteristics
{
    // 16 bit Characteristics
    CC_RELOC_STRIPPED     = 0x0001,
    CC_EXE_IMAGE          = 0x0002,
    CC_LINENO_STRIPPED    = 0x0004,
    CC_LOCAL_SYM_STRIPPED = 0x0008, // Depreciated should be 0
    CC_TRIM_WS            = 0x0010, // Depreciated should be 0
    CC_LARGE_ADDR_AWARE   = 0x0020, 
    CC_RESERVED           = 0x0040, 
    CC_BYTES_REVERSED_LO  = 0x0080, // Depreciated should be 0
    CC_FILE_32BIT         = 0x0100,
    CC_DEBUG_STRIPPED     = 0x0200,
    CC_FILE_RUN_FROM_SWAP = 0x0400,
    CC_NET_RUN_FROM_SWAP  = 0x0800,
    CC_SYSTEM_FILE        = 0x1000,
    CC_DLL_FILE           = 0x2000,
    CC_UP_ONLY            = 0x4000,
    CC_BYTES_REVERSED_HI  = 0x8000,  // Depreciated should be 0
};




enum COFFSectionCharacteristics
{
    // (0x00000000 - 0x00000004) (Reserved)
    CSEC_NO_PAD               = 0x00000008,
    CSEC_CONTAINS_CODE        = 0x00000020,
    CSEC_CONTAINS_INIT_DATA   = 0x00000040,
    CSEC_CONTAINS_UNINIT_DATA = 0x00000080,
    CSEC_LNK_OTHER            = 0x00000100,  // (reserved)
    CSEC_LNK_INFO             = 0x00000200,
    CSEC_LNK_REMOVE           = 0x00000800,  // (reserved)
    CSEC_LNK_COMDAT           = 0x00001000,
    CSEC_GPREL                = 0x00008000,
    CSEC_MEM_PURGE            = 0x00020000,  // (reserved)
    CSEC_MEM_LOCKED           = 0x00040000,  // (reserved)
    CSEC_MEM_PRELOAD          = 0x00080000,  // (reserved)
    CSEC_ALIGN_1              = 0x00100000,
    CSEC_ALIGN_2              = 0x00200000,
    CSEC_ALIGN_4              = 0x00300000,
    CSEC_ALIGN_8              = 0x00400000,
    CSEC_ALIGN_16             = 0x00500000,
    CSEC_ALIGN_32             = 0x00600000,
    CSEC_ALIGN_64             = 0x00700000,
    CSEC_ALIGN_128            = 0x00800000,
    CSEC_ALIGN_256            = 0x00900000,
    CSEC_ALIGN_512            = 0x00A00000,
    CSEC_ALIGN_1024           = 0x00B00000,
    CSEC_ALIGN_2048           = 0x00C00000,
    CSEC_ALIGN_4096           = 0x00D00000,
    CSEC_ALIGN_8192           = 0x00E00000,
    CSEC_EXT_RELOC            = 0x01000000,
    CSEC_MEM_DISCARD          = 0x02000000,
    CSEC_MEM_NOT_CACHED       = 0x04000000,
    CSEC_MEM_NOT_PAGED        = 0x08000000,
    CSEC_MEM_SHARED           = 0x10000000,
    CSEC_MEM_CAN_EXEC         = 0x20000000,
    CSEC_MEM_CAN_READ         = 0x40000000,
    CSEC_MEM_CAN_WRITE        = 0x80000000,
};

enum ImageSubsystem
{
    ISS_NONE,
    ISS_NATIVE,
    ISS_WIN_GUI,             // GUI,
    ISS_WIN_CUI,             // character subsystem
    ISS_OS2_CUI   = 5,       // OS/2 character subsystem
    ISS_POSIX_CUI = 7,       // Posix character subsystem
    ISS_NATIVE_WIN,          // Native Win9x driver
    ISS_WIN_CE_GUI,          // Windows CE
    ISS_EFI_APP,             
    ISS_EFI_BOOT_DRIVER,     
    ISS_EFI_RUNTIME_DRIVER,  
    ISS_EFI_ROM,             
    ISS_XBOX,                
    ISS_WIN_BOOT_APP = 16,   
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
    SKuint32 m_sizeofBSSData;
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
    COFFDataDirectory m_directories[CDE_MAX];
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


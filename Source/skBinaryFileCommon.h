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
#ifndef _skBinaryFileCommon_h_
#define _skBinaryFileCommon_h_

#include "Utils/Config/skConfig.h"




enum skRuntimePointerLen
{
    RPL_LEN_IS_8 = sizeof(void*) == 8,
    RPL_LEN_IS_4 = sizeof(void*) == 4
};



/// Generic architecture type. 
/// Derived classes should convert the 
/// the specific value to one of these values.
enum skMachineArchitecture
{
    IS_NONE = 0,
    IS_SPARC,
    IS_X86,
    IS_MPS,
    IS_POWERPC,
    IS_S390,
    IS_ARM,
    IS_SUPERH,
    IS_IA64,
    IS_X8664,
    IS_AARCH64,
    IS_RISCV
};


enum skErrorCodes
{
    EC_OK=0,
    EC_INVALID_POINTER,
    EC_FILE_LOADING_FAILED,
    EC_UNKNOWN_FILE_FORMAT,
    EC_OVERFLOW,
    EC_DUPLICATE,
    EC_UNEXPECTED
};


enum skFileFormat
{
    FF_UNKNOWN,
    FF_ELF,  /// instance skElfFile 
    FF_PE   /// instance skPortableExecutable
};


enum skFileFormatType
{
    FFT_UNKNOWN,
    FFT_32BIT, 
    FFT_64BIT,  
};

class skStream;
class skSection;
class skSymbol;
class skBinaryFile;



#endif  //_skBinaryFileCommon_h_

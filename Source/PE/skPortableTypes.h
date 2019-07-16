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


struct COFFOptionalHeaderStandard
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



#endif  //_skPortableTypes_h_

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
#include "Utils/skString.h"

#define IsBitFlagSet(b, f) ((b & f) != 0)



void skPortableUtils::getMachine(const COFFHeader& inf, char dest[], int len)
{
    dest[0] = 0;
    switch (inf.m_machine)
    {
    case CMT_AMD64:
        sk_strncpy(dest, len, "AMD64", 16);
        break;
    case CMT_ARM:
        sk_strncpy(dest, len, "ARM", 16);
        break;
    case CMT_ARM64:
        sk_strncpy(dest, len, "ARM64", 16);
        break;
    case CMT_IA64:
        sk_strncpy(dest, len, "IA64", 16);
        break;
    case CMT_I386:
        sk_strncpy(dest, len, "I386", 16);
        break;
    default:
        sk_strncpy(dest, len, "Not handled", 16);
        break;
    }
}


void skPortableUtils::getPlatformId(const COFFOptionalHeaderCommon& inf, char dest[], int len)
{
    dest[0] = 0;
    if (inf.m_magic == COFF_MAG_PE32)
        sk_strncpy(dest, len, "PE32", len);
    else if (inf.m_magic == COFF_MAG_PE64)
        sk_strncpy(dest, len, "PE32+", len);
    else
        sk_strncpy(dest, len, "Unknown Header Magic", len);
}



void pu_appendStr(skString& dest, const char* str)
{
    if (dest.size() > 1)
        dest += '|';
    dest += str;
}



void skPortableUtils::getCharacteristicsString16(const SKuint16& characteristics, skString& dest) 
{

    dest.clear();

    if (IsBitFlagSet(characteristics, CC_RELOC_STRIPPED))
        pu_appendStr(dest, "ReloactionsStripped");
    if (IsBitFlagSet(characteristics, CC_EXE_IMAGE))
        pu_appendStr(dest, "ImageTypeExecutable");
    if (IsBitFlagSet(characteristics, CC_LINENO_STRIPPED))
        pu_appendStr(dest, "LineNoStripped");
    if (IsBitFlagSet(characteristics, CC_LOCAL_SYM_STRIPPED))
        pu_appendStr(dest, "LocalSymbolsStripped");
    if (IsBitFlagSet(characteristics, CC_TRIM_WS))
        pu_appendStr(dest, "TrimWorkingSet");
    if (IsBitFlagSet(characteristics, CC_LARGE_ADDR_AWARE))
        pu_appendStr(dest, "LargeAddressAware");
    if (IsBitFlagSet(characteristics, CC_RESERVED))
        pu_appendStr(dest, "Reserved");
    if (IsBitFlagSet(characteristics, CC_BYTES_REVERSED_LO))
        pu_appendStr(dest, "LittleEndian");
    if (IsBitFlagSet(characteristics, CC_FILE_32BIT))
        pu_appendStr(dest, "FileIs32Bit");
    if (IsBitFlagSet(characteristics, CC_DEBUG_STRIPPED))
        pu_appendStr(dest, "DebugSymbolsStripped");
    if (IsBitFlagSet(characteristics, CC_FILE_RUN_FROM_SWAP))
        pu_appendStr(dest, "MediaRunFromSwap");
    if (IsBitFlagSet(characteristics, CC_NET_RUN_FROM_SWAP))
        pu_appendStr(dest, "NetworkRunFromSwap");
    if (IsBitFlagSet(characteristics, CC_SYSTEM_FILE))
        pu_appendStr(dest, "IsSystemFile");
    if (IsBitFlagSet(characteristics, CC_DLL_FILE))
        pu_appendStr(dest, "IsDLL");
    if (IsBitFlagSet(characteristics, CC_BYTES_REVERSED_HI))
        pu_appendStr(dest, "BigEndian");
}

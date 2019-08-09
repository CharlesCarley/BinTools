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

void skPortableUtils::getSectionCharacteristics(const SKuint32& inf, skString& dest)
{
    dest.clear();

    if (IsBitFlagSet(inf, CSEC_NO_PAD))
        pu_appendStr(dest, "NoPading");
    if (IsBitFlagSet(inf, CSEC_CONTAINS_CODE))
        pu_appendStr(dest, "ContainsCode");
    if (IsBitFlagSet(inf, CSEC_CONTAINS_INIT_DATA))
        pu_appendStr(dest, "ContainsInitializedData");
    if (IsBitFlagSet(inf, CSEC_CONTAINS_UNINIT_DATA))
        pu_appendStr(dest, "ContainsUninitializedData");
    if (IsBitFlagSet(inf, CSEC_LNK_OTHER))
        pu_appendStr(dest, "Reserved");
    if (IsBitFlagSet(inf, CSEC_LNK_INFO))
        pu_appendStr(dest, "ContainsExtraInfo");
    if (IsBitFlagSet(inf, CSEC_LNK_REMOVE))
        pu_appendStr(dest, "WillBeRemovedWhenLinked");
    if (IsBitFlagSet(inf, CSEC_LNK_COMDAT))
        pu_appendStr(dest, "ContainsCOMDAT");
    if (IsBitFlagSet(inf, CSEC_GPREL))
        pu_appendStr(dest, "RefrencedThroughGlobPtr");
    if (IsBitFlagSet(inf, CSEC_MEM_PURGE))
        pu_appendStr(dest, "Reserved");
    if (IsBitFlagSet(inf, CSEC_MEM_LOCKED))
        pu_appendStr(dest, "Reserved");
    if (IsBitFlagSet(inf, CSEC_MEM_PRELOAD))
        pu_appendStr(dest, "Reserved");
    if (IsBitFlagSet(inf, CSEC_ALIGN_1))
        pu_appendStr(dest, "Align1");
    if (IsBitFlagSet(inf, CSEC_ALIGN_2))
        pu_appendStr(dest, "Align2");
    if (IsBitFlagSet(inf, CSEC_ALIGN_4))
        pu_appendStr(dest, "Align4");
    if (IsBitFlagSet(inf, CSEC_ALIGN_8))
        pu_appendStr(dest, "Align8");
    if (IsBitFlagSet(inf, CSEC_ALIGN_16))
        pu_appendStr(dest, "Align16");
    if (IsBitFlagSet(inf, CSEC_ALIGN_32))
        pu_appendStr(dest, "Align32");
    if (IsBitFlagSet(inf, CSEC_ALIGN_64))
        pu_appendStr(dest, "Align64");
    if (IsBitFlagSet(inf, CSEC_ALIGN_128))
        pu_appendStr(dest, "Align128");
    if (IsBitFlagSet(inf, CSEC_ALIGN_256))
        pu_appendStr(dest, "Align256");
    if (IsBitFlagSet(inf, CSEC_ALIGN_512))
        pu_appendStr(dest, "Align512");
    if (IsBitFlagSet(inf, CSEC_ALIGN_1024))
        pu_appendStr(dest, "Align1024");
    if (IsBitFlagSet(inf, CSEC_ALIGN_2048))
        pu_appendStr(dest, "Align2048");
    if (IsBitFlagSet(inf, CSEC_ALIGN_4096))
        pu_appendStr(dest, "Align4096");
    if (IsBitFlagSet(inf, CSEC_ALIGN_8192))
        pu_appendStr(dest, "Align8192");
    if (IsBitFlagSet(inf, CSEC_EXT_RELOC))
        pu_appendStr(dest, "ExtendedRelocations");
    if (IsBitFlagSet(inf, CSEC_MEM_DISCARD))
        pu_appendStr(dest, "Discardable");
    if (IsBitFlagSet(inf, CSEC_MEM_NOT_CACHED))
        pu_appendStr(dest, "CannotCache");
    if (IsBitFlagSet(inf, CSEC_MEM_NOT_PAGED))
        pu_appendStr(dest, "NotPageable");
    if (IsBitFlagSet(inf, CSEC_MEM_SHARED))
        pu_appendStr(dest, "CanShare");
    if (IsBitFlagSet(inf, CSEC_MEM_CAN_EXEC))
        pu_appendStr(dest, "CanExec");
    if (IsBitFlagSet(inf, CSEC_MEM_CAN_READ))
        pu_appendStr(dest, "CanRead");
    if (IsBitFlagSet(inf, CSEC_MEM_CAN_WRITE))
        pu_appendStr(dest, "CanWrite");
}


void skPortableUtils::getSubsystem(const SKuint16& ss, char dest[], int len)
{
    switch (ss)
    {
    case ISS_NATIVE:
        sk_strncpy(dest, len, "Native", 6);
        break;
    case ISS_WIN_GUI:     
        sk_strncpy(dest, len, "Native", 6);
        break;
    case ISS_WIN_CUI:     
        sk_strncpy(dest, len, "WinCharacterSubsystem", 21);
        break;
    case ISS_OS2_CUI:     
        sk_strncpy(dest, len, "OS2CharacterSubsystem", 21);
        break;
    case ISS_POSIX_CUI:   
        sk_strncpy(dest, len, "PosixCharacterSubsystem", 23);
        break;
    case ISS_NATIVE_WIN:     
        sk_strncpy(dest, len, "NativeWin", 9);
        break;
    case ISS_WIN_CE_GUI:     
        sk_strncpy(dest, len, "WindowsCE", 9);
        break;
    case ISS_EFI_APP:
        sk_strncpy(dest, len, "EFIApp", 7);
        break;
    case ISS_EFI_BOOT_DRIVER:
        sk_strncpy(dest, len, "EFIBootDriver", 13);
        break;
    case ISS_EFI_RUNTIME_DRIVER:
        sk_strncpy(dest, len, "EFIRuntimeDriver", 19);
        break;
    case ISS_EFI_ROM:
        sk_strncpy(dest, len, "EFIROM", 9);
        break;
    case ISS_XBOX:
        sk_strncpy(dest, len, "XBOX", 4);
        break;
    case ISS_WIN_BOOT_APP: 
        sk_strncpy(dest, len, "WinBootApp", 10);
        break;
    default:
    case ISS_NONE:
        sk_strncpy(dest, len, "None", 4);
        break;
    }

}

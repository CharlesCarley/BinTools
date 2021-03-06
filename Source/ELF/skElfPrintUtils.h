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
#ifndef _skElfPrintUtils_h_
#define _skElfPrintUtils_h_

#include "Utils/skDebugger.h"
#include "Utils/skString.h"
#include "ELF/skElfTypes.h"


class skElfPrintUtils
{
public:
    template <typename T>
    static void getPlatformId(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        dest[0] = 0;
        if (inf.m_id[EMN_CLASS] == 1)
            sk_strncpy(dest, len, "ELF32", 64);
        else if (inf.m_id[EMN_CLASS] == 2)
            sk_strncpy(dest, len, "ELF64", 64);
    }

    template <typename T>
    static void getByteOrder(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        dest[0] = 0;
        if (inf.m_id[EMN_ENDIAN] == 1)
            sk_strncpy(dest, len, "Little Endian", 32);
        else if (inf.m_id[EMN_ENDIAN] == 2)
            sk_strncpy(dest, len, "Big Endian", 32);
    }

    template <typename T>
    static void getVersion(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        dest[0] = 0;
        if (inf.m_id[EMN_VERSION] == 1)
            sk_strncpy(dest, len, "1 (current)", 32);
    }

    template <typename T>
    static void getABI(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        dest[0] = 0;
        switch (inf.m_id[EMN_ABI])
        {
            case 0x00:
                sk_strncpy(dest, len, "System V", 32);
                break;
            case 0x01:
                sk_strncpy(dest, len, "HP - UX", 32);
                break;
            case 0x02:
                sk_strncpy(dest, len, "NetBSD", 32);
                break;
            case 0x03:
                sk_strncpy(dest, len, "Linux", 32);
                break;
            case 0x04:
                sk_strncpy(dest, len, "GNU Hurd", 32);
                break;
            case 0x06:
                sk_strncpy(dest, len, "Solaris", 32);
                break;
            case 0x07:
                sk_strncpy(dest, len, "AIX", 32);
                break;
            case 0x08:
                sk_strncpy(dest, len, "IRIX", 32);
                break;
            case 0x09:
                sk_strncpy(dest, len, "FreeBSD", 32);
                break;
            case 0x0A:
                sk_strncpy(dest, len, "Tru64", 32);
                break;
            case 0x0B:
                sk_strncpy(dest, len, "Novell Modesto", 32);
                break;
            case 0x0C:
                sk_strncpy(dest, len, "OpenBSD", 32);
                break;
            case 0x0D:
                sk_strncpy(dest, len, "OpenVMS", 32);
                break;
            case 0x0E:
                sk_strncpy(dest, len, "NonStop Kernel", 32);
                break;
            case 0x0F:
                sk_strncpy(dest, len, "AROS", 32);
                break;
            case 0x10:
                sk_strncpy(dest, len, "Fenix OS", 32);
                break;
            case 0x11:
                sk_strncpy(dest, len, "CloudABI", 32);
                break;
            default:
                sk_strncpy(dest, len, "Unknown", 32);
                break;
        }
    }

    template <typename T>
    static void getType(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        switch (inf.m_type)
        {
            case ET_NONE:
                sk_strncpy(dest, len, "ET_NONE", 32);
                break;
            case ET_EXEC:
                sk_strncpy(dest, len, "ET_EXEC", 32);
                break;
            case ET_DYN:
                sk_strncpy(dest, len, "ET_DYN", 32);
                break;
            case ET_CORE:
                sk_strncpy(dest, len, "ET_CORE", 32);
                break;
            case ET_LOPRC:
                sk_strncpy(dest, len, "ET_LOPRC", 32);
                break;
            case ET_HIPRC:
                sk_strncpy(dest, len, "ET_HIPRC", 32);
                break;
            default:
                sk_strncpy(dest, len, "Unknown", 32);
                break;
        }
    }

    template <typename T>
    static void getArch(const skElfHeaderInfo<T>& inf, char dest[], int len)
    {
        switch (inf.m_machine)
        {
            case EIA_NONE:
                sk_strncpy(dest, len, "EIA_NONE", 32);
                break;
            case EIA_SPARC:
                sk_strncpy(dest, len, "EIA_SPARC", 32);
                break;
            case EIA_X86:
                sk_strncpy(dest, len, "EIA_X86", 32);
                break;
            case EIA_MPS:
                sk_strncpy(dest, len, "EIA_MPS", 32);
                break;
            case EIA_POWERPC:
                sk_strncpy(dest, len, "EIA_POWERPC", 32);
                break;
            case EIA_S390:
                sk_strncpy(dest, len, "EIA_S390", 32);
                break;
            case EIA_SUPERH:
                sk_strncpy(dest, len, "EIA_SUPERH", 32);
                break;
            case EIA_IA64:
                sk_strncpy(dest, len, "EIA_IA64", 32);
                break;
            case EIA_X8664:
                sk_strncpy(dest, len, "EIA_X8664", 32);
                break;
            case EIA_AARCH64:
                sk_strncpy(dest, len, "EIA_AARCH64", 32);
                break;
            case EIA_RISCV:
                sk_strncpy(dest, len, "EIA_RISCV", 32);
                break;
            default:
                sk_strncpy(dest, len, "Unknown", 32);
                break;
        }
    }

  


    template <typename T>
    static void getSectionType(const skElfSectionHeader<T>& inf, char dest[], int len)
    {
        switch (inf.m_type)
        {
            case EST_NONE:
                sk_strncpy(dest, len, "EST_NONE", 32);
                break;
            case EST_PROGBITS:
                sk_strncpy(dest, len, "EST_PROGBITS", 32);
                break;
            case EST_SYMTAB:
                sk_strncpy(dest, len, "EST_SYMTAB", 32);
                break;
            case EST_STARTTAB:
                sk_strncpy(dest, len, "EST_STARTTAB", 32);
                break;
            case EST_RELA:
                sk_strncpy(dest, len, "EST_RELA", 32);
                break;
            case EST_HASH:
                sk_strncpy(dest, len, "EST_HASH", 32);
                break;
            case EST_DYNAMIC:
                sk_strncpy(dest, len, "EST_DYNAMIC", 32);
                break;
            case EST_NOTE:
                sk_strncpy(dest, len, "EST_NOTE", 32);
                break;
            case EST_NOBITS:
                sk_strncpy(dest, len, "EST_NOBITS", 32);
                break;
            case EST_REL:
                sk_strncpy(dest, len, "EST_REL", 32);
                break;
            case EST_SHLIB:
                sk_strncpy(dest, len, "EST_SHLIB", 32);
                break;
            case EST_DYNSYM:
                sk_strncpy(dest, len, "EST_DYNSYM", 32);
                break;
            case EST_LOPROC:
                sk_strncpy(dest, len, "EST_LOPROC", 32);
                break;
            case EST_HIPROC:
                sk_strncpy(dest, len, "EST_HIPROC", 32);
                break;
            default:
                sk_strncpy(dest, len, "Unknown", 32);
                break;
        }
    }

};


#endif  //_skElfPrintUtils_h_

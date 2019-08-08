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
        sk_strncpy(dest, len, "PE32", 64);
    else if (inf.m_magic == COFF_MAG_PE64)
        sk_strncpy(dest, len, "PE32+", 64);
    else
        sk_strncpy(dest, len, "Unknown Header Magic", 64);
}

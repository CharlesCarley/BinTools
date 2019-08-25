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
#include "bpCommon.h"





void b2WriteColor(skConsoleColorSpace cs)
{
    skDebugger::writeColor(cs);
}



void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKuint8 c;
    SKsize  j;

    if (!cp || offs == SK_NPOS || max == SK_NPOS)
        return;

    for (j = 0; j < 16; ++j)
    {
        if (j % 8 == 0)
            printf(" ");

        if (offs + j < max)
        {
            c = (SKint32)cp[offs + j];

            b2MarkColor(c, mark);
            printf("%02X ", c);
        }
        else
            printf("   ");
    }
}



void b2WriteAscii(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKuint8 c;
    SKsize  j;

    if (!cp || offs == SK_NPOS || max == SK_NPOS)
        return;


    b2WriteColor(CS_WHITE);
    printf(" |");
    for (j = 0; j < 16; ++j)
    {
        if (offs + j < max)
        {
            c = (SKuint8)cp[offs + j];


            b2MarkColor(c, mark);
            if (c >= 0x20 && c < 0x7F)
                printf("%c", c);
            else
                printf(".");
        }
        else
            printf(" ");
    }

    b2WriteColor(CS_WHITE);
    printf("|");
}



void b2MarkColor(int c, int mark)
{
    if (c == mark)
        b2WriteColor(CS_RED);
    else if (c == 0x00 || c == 0xCC)
        b2WriteColor(CS_GREY);
    else
        b2WriteColor(CS_WHITE);
}



void b2WriteAddress(SKuint64 addr)
{
    b2WriteColor(CS_GREY);
    printf("%16llx  ", addr);
}


void b2DumpHex(void* ptr, SKuint32 offset, SKuint32 len, int flags, int mark, bool nl)
{
    if (!ptr || offset == SK_NPOS || len == SK_NPOS)
        return;

    SKsize i;
    char*  cp = (char*)ptr;

    for (i = 0; i < len; i += 16)
    {
        if (flags & PF_ADDRESS)
            b2WriteAddress((size_t)(i + offset));
        if (flags & PF_HEX)
            b2WriteHex(cp, i, len, flags, mark);
        if (flags & PF_ASCII)
            b2WriteAscii(cp, i, len, flags, mark);
        if (nl)
            printf("\n");
    }
}

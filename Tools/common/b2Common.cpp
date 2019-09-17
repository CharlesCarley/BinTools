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
#include "b2Common.h"



void b2WriteColor(skConsoleColorSpace cs)
{
    skDebugger::writeColor(cs);
}



void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, SKuint32 mark)
{
    SKuint8 c1, c2, c3, c4;
    SKsize  j, n =0;

    union {
        SKuint8 b[4];
        SKuint32 i;
    } cmp;
    cmp.i = mark;

    if (!cp || offs == SK_NPOS32 || max == SK_NPOS)
        return;


    for (j = 0; j < 16; ++j)
    {
        if (j % 8 == 0)
            printf(" ");

        if (offs + j < max)
        {
            c1 = (SKuint32)cp[offs + j];

            if (n)
            {
                n--;
                b2MarkColor(1, 1);
            }
            else
            {
                if (mark < 256)
                    b2MarkColor(c1, cmp.i);
                else if (mark < 65536)
                {
                    if (n == 0)
                    {
                        c2 = (SKuint32)cp[offs + ((j + 1) % 16)];

                        if ((c1 == cmp.b[1] && c2 == cmp.b[0]) ||
                            (c1 == cmp.b[0] && c2 == cmp.b[1]))
                        {
                            n = 1;  // bleed through x times
                            b2MarkColor(1, 1);
                        }
                        else
                        {
                            b2MarkColor(c1, cmp.i);
                            n = 0;
                        }
                    }
                }
                else
                {
                    if (n == 0)
                    {
                        c2 = (SKuint32)cp[offs + ((j + 1) % 16)];
                        c3 = (SKuint32)cp[offs + ((j + 2) % 16)];
                        c4 = (SKuint32)cp[offs + ((j + 3) % 16)];

                        if ((c1 == cmp.b[3] && c2 == cmp.b[2] && c3 == cmp.b[1] && c4 == cmp.b[0])  ||
                            (c1 == cmp.b[0] && c2 == cmp.b[1] && c3 == cmp.b[2] && c4 == cmp.b[3]))
                        {
                            n = 3;  // bleed through x times
                            b2MarkColor(1, 1);
                        }
                        else
                        {
                            b2MarkColor(c1, cmp.i);
                            n = 0;
                        }
                    }
                }
            }
            printf("%02X ", c1);
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



void b2MarkColor(SKuint32 c, SKuint32 mark)
{
    if (c == mark)
        b2WriteColor(CS_RED);
    else if (c == 0x00 || c == 0xCC)
        b2WriteColor(CS_GREY);
    else
        b2WriteColor(CS_WHITE);
}



void b2WriteAddress(SKsize addr, int flags)
{
    b2WriteColor(CS_LIGHT_GREY);
    if (flags & PF_FULLADDR)
        printf("%016llx  ", addr);
    else
        printf("%16llx  ", addr);
}


void b2DumpHex(void* ptr, SKsize offset, SKsize len, int flags, SKuint32 mark, bool nl)
{
    if (!ptr || offset == SK_NPOS || len == SK_NPOS)
        return;

    SKsize i;
    char*  cp = (char*)ptr;

    for (i = 0; i < len; i += 16)
    {
        if (flags & PF_ADDRESS)
            b2WriteAddress((SKsize)(i + offset), flags);
        if (flags & PF_HEX)
            b2WriteHex(cp, i, len, flags, mark);
        if (flags & PF_ASCII)
            b2WriteAscii(cp, i, len, flags, mark);
        if (nl)
            printf("\n");
    }
}

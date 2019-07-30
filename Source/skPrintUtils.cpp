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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils/Config/skConfig.h"

#include "Utils/skDebugger.h"
#include "Utils/skTimer.h"
#include "skPrintUtils.h"
#include "skBinaryFileCommon.h"



void skPrintUtils::writeColor(skConsoleColorSpace fg, skConsoleColorSpace bg)
{
    skDebugger::writeColor(fg, bg);
}




void skPrintUtils_Hexdump(void* ptr, SKsize start, SKsize stop, int flags, int mark, bool nl);
void skPrintUtils_doMarkColor(int c, int mark);
void skPrintUtils_writeAscii(char* cp, SKsize offs, SKsize max, int flags, int mark);
void skPrintUtils_writeBinary(char* cp, SKsize offs, SKsize max, int flags, int mark);
void skPrintUtils_writeHex(char* cp, SKsize offs, SKsize max, int flags, int mark);


void skPrintUtils_writeHex(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKsize j;
    for (j = 0; j < 16; ++j)
    {
        if (cp)
        {
            if (j % 8 == 0)
                skPrintf(" ");

            if (offs + j < max)
            {
                unsigned char np = (unsigned char)cp[offs + j];
                if (flags & PF_COLORIZE)
                    skPrintUtils_doMarkColor((int)np, mark);

                skPrintf("%02X ", (np));
            }
            else
                skPrintf("   ");
        }
    }
}


void skPrintUtils_writeBinary(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    const int BinaryTable[9] = {256, 128, 64, 32, 16, 8, 4, 2, 1};

    if (!cp)
        return;

    SKsize j;
    for (j = 0; j < 16; ++j)
    {
        if (offs + j < max)
        {
            int c = (int)(unsigned char)cp[offs + j], k;

            if (flags & PF_COLORIZE)
                skPrintUtils_doMarkColor(c, mark);

            if (c < BinaryTable[0])
            {
                for (k = 0; k < 9; ++k)
                {
                    if (c >= BinaryTable[k])
                    {
                        skPrintf("%c", 0x31);
                        c -= BinaryTable[k];
                    }
                    else
                        skPrintf("%c", 0x30);
                }
                skPrintf("%c", 0x20);
            }
        }
        else
            skPrintf("         ");
    }
}


void skPrintUtils_writeAscii(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKsize j;
    if (!cp)
        return;

    if (flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_WHITE);

    skPrintf(" |");
    for (j = 0; j < 16; ++j)
    {
        if (offs + j < max)
        {
            unsigned char np = (unsigned char)cp[offs + j];

            if (flags & PF_COLORIZE)
                skPrintUtils_doMarkColor((int)np, mark);

            if (np >= 0x20 && np < 0x7F)
                skPrintf("%c", np);
            else
                skPrintf(".");
        }
        else
            skPrintf(" ");
    }

    if (flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_WHITE);
    skPrintf("|");
}


void skPrintUtils_doMarkColor(int c, int mark)
{
    if (c == mark)
        skPrintUtils::writeColor(CS_RED);
    else if (c == 0x00 || c == 0xCC)
        skPrintUtils::writeColor(CS_GREY);
    else
        skPrintUtils::writeColor(CS_WHITE);
}


void skPrintUtils_Hexdump(void* ptr, SKsize offset, SKsize stop, int flags, int mark, bool nl)
{
    char* cp = (char*)ptr;
    for (SKsize i = 0; i < stop; i += 16)
    {
        if (flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_LIGHT_GREY);
        if (flags & PF_ADDRESS)
            skPrintUtils::writeAddress((size_t)(i + offset));
        if (flags & PF_HEX)
            skPrintUtils_writeHex(cp, i, stop, flags, mark);
        if (flags & PF_BINARY)
            skPrintUtils_writeBinary(cp, i, stop, flags, mark);
        if (flags & PF_ASCII)
            skPrintUtils_writeAscii(cp, i, stop, flags, mark);
        if (nl)
            skPrintf("\n");
    }
}


void skPrintUtils::writeAddress(SKuint64 addr)
{
    skPrintUtils::writeColor(CS_GREY);
    skPrintf("%16llx  ", addr);
}
 
void skPrintUtils::dumpHex(void* ptr, size_t offset, size_t len, int flags, int mark, bool nl)
{
    /// TODO: Update this to handle more options.
    /// It should be able to convert printout to 
    /// short, int, and unsigned int as well as 
    /// have the option to specify a max column width.
    skPrintUtils_Hexdump(ptr, offset, len, flags, mark, nl);
}



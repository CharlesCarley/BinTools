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

#if SK_PLATFORM == SK_PLATFORM_WIN32
#include "conio.h"
#include "windows.h"
#else
#include "sys/select.h"
#endif

#include "Utils/skDebugger.h"
#include "Utils/skTimer.h"
#include "skPrintUtils.h"
#include "skBinaryFileCommon.h"





#if SK_PLATFORM == SK_PLATFORM_WIN32

void*         skPrintUtils::m_stdout            = 0;
unsigned char skPrintUtils::COLOR_TABLE[16][16] = {
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F},
    {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F},
    {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F},
    {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F},
    {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F},
    {0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F},
    {0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F},
    {0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F},
    {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},
    {0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F},
    {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF},
    {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF},
    {0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF},
    {0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF},
    {0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF},
    {0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF},
};


unsigned char skPrintUtils::getColor(skConsoleColorSpace fore, skConsoleColorSpace back)
{
    return COLOR_TABLE[back][fore];
}



#else

unsigned char* skPrintUtils::getColor(skConsoleColorSpace fore, skConsoleColorSpace)
{

    static unsigned char color[2];
    switch (fore)
    {
    case CS_DARKGREEN:
        color[0] = 32;
        break;
    case CS_GREEN:
        color[0] = 92;
        break;
    case CS_CYAN:
        color[0] = 96;
        break;
    case CS_DARKCYAN:
        color[0] = 36;
        break;
    case CS_YELLOW:
        color[0] = 93;
        break;
    case CS_DARKYELLOW:
        color[0] = 33;
        break;
    case CS_RED:
        color[0] = 91;
        break;
    case CS_DARKRED:
        color[0] = 31;
        break;
    case CS_DARKBLUE:
        color[0] = 34;
        break;
    case CS_BLUE:
        color[0] = 94;
        break;
    case CS_DARKMAGENTA:
        color[0] = 35;
        break;
    case CS_MAGENTA:
        color[0] = 95;
        break;
    case CS_BLACK:
        color[0] = 30;
        break;
    case CS_LIGHT_GREY:
        color[0] = 37;
        break;
    case CS_GREY:
        color[0] = 90;
        break;
    case CS_WHITE:
        color[0] = 37;
        break;
    }

    // leaving out the background color for now...
    color[1] = color[0];
    return color; 
}

#endif



void skPrintUtils::writeColor(int fg, int bg)
{
    // filter out invalid colors and do nothing if one is supplied 
    if (fg < 0 || fg > CS_COLOR_MAX || bg < 0 || bg > CS_COLOR_MAX)
        return;

#if SK_PLATFORM != SK_PLATFORM_WIN32
    unsigned char* col = getColor((skConsoleColorSpace)fg, (skConsoleColorSpace)bg);
    skPrintf("\e[%im", col[0]);
#else
    if (m_stdout == 0)
        m_stdout = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ::SetConsoleTextAttribute(m_stdout, getColor((skConsoleColorSpace)fg, (skConsoleColorSpace)bg));
#endif
}



void skPrintUtils::dumpColors()
{
    for (int i = 0; i < 16; ++i)
    {
        for (int j = 0; j < 16; ++j)
        {
            writeColor(i, j);
            skPrintf("%i, %i\n", i, j);
        }
    }
}

void skPrintUtils::writeSeperator(bool color)
{
    if (color)
        writeColor(CS_GREY);

    skPrintf("------------------------------------------------------------------\n");

    if (color)
        writeColor(CS_WHITE);
}

void skPrintUtils::clear(void)
{
#if SK_PLATFORM == SK_PLATFORM_WIN32
    system("cls");
#else
    system("clear");
#endif  // WIN32
}

void skPrintUtils::pause(void)
{
#if SK_PLATFORM == SK_PLATFORM_WIN32
    skPrintf("\n");
    writeColor(CS_WHITE);
    system("pause");
    clear();
#else
    writeColor(CS_WHITE);
    skPrintf("Press any key to continue . . .\n");

    fd_set m_readInfo;
    FD_ZERO(&m_readInfo);
    FD_SET(0, &m_readInfo);

    int result = -1;
    while (result == -1)
    {
        result = select(1, &m_readInfo, 0, 0, 0);
        if (FD_ISSET(0, &m_readInfo))
            break;
    }

    clear();
#endif
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
    if (RPL_LEN_IS_8)
        skPrintf("%016llx ", addr);
    else
        skPrintf("%08x ", (SKuint32)addr);
}


void skPrintUtils::dumpHex(void* ptr, size_t offset, size_t len, int flags, int mark, bool nl)
{
    /// TODO: Update this to handle more options.
    /// It should be able to convert printout to 
    /// short, int, and unsigned int as well as 
    /// have the option to specify a max column width.
    skPrintUtils_Hexdump(ptr, offset, len, flags, mark, nl);
}



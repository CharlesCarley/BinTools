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
#ifndef _skPrintUtils_h_
#define _skPrintUtils_h_

#include "Utils/Config/skConfig.h"

enum skConsoleColorSpace
{
    CS_BLACK = 0,
    CS_DARKBLUE,
    CS_DARKGREEN,
    CS_DARKCYAN,
    CS_DARKRED,
    CS_DARKMAGENTA,
    CS_DARKYELLOW,
    CS_LIGHT_GREY,
    CS_GREY,
    CS_BLUE,
    CS_GREEN,
    CS_CYAN,
    CS_RED,
    CS_MAGENTA,
    CS_YELLOW,
    CS_WHITE,
};



enum skPrintFlags
{
    PF_NONE       = 0,
    PF_COLORIZE   = (1 << 0),
    PF_HEX        = (1 << 1),
    PF_ASCII      = (1 << 2),
    PF_BINARY     = (1 << 3),
    PF_ADDRESS    = (1 << 4),
    PF_DISASEMBLE = (1 << 5), // ignores hex in place of disassembly
    PF_DEFAULT    = PF_COLORIZE | PF_ADDRESS | PF_HEX | PF_ASCII,
    PF_BIN        = PF_COLORIZE | PF_BINARY,
    PF_HEXDIS     = PF_COLORIZE | PF_HEX,
};


class skPrintUtils
{
private:
#if SK_PLATFORM == SK_PLATFORM_WIN32
    static void*         m_stdout;
    static unsigned char COLOR_TABLE[16][16];
    static unsigned char getColor(skConsoleColorSpace fore, skConsoleColorSpace back = CS_BLACK);
#else
    static unsigned char  COLOR_TABLE[16][16][3];
    static unsigned char* getColor(skConsoleColorSpace fore, skConsoleColorSpace back = CS_BLACK);
#endif

public:
    static void writeColor(int cs, int back = CS_BLACK);
    static void writeSeperator(bool color = true);

    static void dumpColors();

    static void clear(void);
    static void pause(void);
    static int  charHexToInt(char ch);
    static void dumpHex(void* ptr, size_t len, int flags = PF_DEFAULT, int mark=0, bool nl = true);
};

#endif  //_skPrintUtils_h_

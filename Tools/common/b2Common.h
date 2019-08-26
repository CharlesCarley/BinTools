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
#ifndef _bpCommon_h_
#define _bpCommon_h_


#include "Utils/skDebugger.h"

enum b2PrintFlags
{
    PF_NONE       = 0,
    PF_COLORIZE   = (1 << 0),
    PF_HEX        = (1 << 1),
    PF_ASCII      = (1 << 2),
    PF_ADDRESS    = (1 << 3),
    PF_FULLADDR   = (1 << 4),
    PF_DISASEMBLE = (1 << 5),  // ignores hex in place of disassembly
    PF_DEFAULT    = PF_COLORIZE | PF_ADDRESS | PF_HEX | PF_ASCII,
    PF_HEXDIS     = PF_COLORIZE | PF_HEX,
};


extern void b2WriteColor(skConsoleColorSpace cs);
extern void b2WriteAddress(SKuint64 addr, int flags=0);

extern void b2DumpHex(void* ptr, SKuint32 offset, SKuint32 len, int flags = PF_DEFAULT, int mark = -1, bool nl = true);
extern void b2MarkColor(SKuint32 c, SKuint32 mark);
extern void b2WriteAscii(char* cp, SKsize offs, SKsize max, int flags, int mark);
extern void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark);


#endif  //_bpCommon_h_
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
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils/skFileStream.h"
#include "Utils/skArray.h"
#include "b2Common.h"


struct b2ProgramInfo
{
    skFileStream  m_stream;
    SKint64       m_code;
    SKuint32      m_flags;
};


void b2Usage(void);
int  b2ParseCommandLine(b2ProgramInfo &ctx, int argc, char** argv);
bool b2Alloc(b2ProgramInfo &ctx, const char *fname);
void b2Print(b2ProgramInfo &ctx);
void b2Free(b2ProgramInfo &ctx);


int main(int argc, char** argv)
{
    b2ProgramInfo ctx = {skFileStream(), -1, PF_DEFAULT | PF_FULLADDR};

    if (b2ParseCommandLine(ctx, argc, argv) == -1)
    {
        b2Usage();
        return -1;
    }

    b2Print(ctx);
    b2Free(ctx);

    return 0;
}


void b2Usage(void)
{
    printf("b2Usage\n");
}

int b2ParseCommandLine(b2ProgramInfo &ctx, int argc, char** argv)
{
    if (argc <= 1)
        return -1;

    size_t alen, offs = 0;
    int    i;
    char * ch = 0;
    char   sw;
    bool   err = false;

    for (i = 1; i < argc - 1; i++)
    {
        ch = argv[i];
        if (ch && *ch == '-')
        {
            offs = 1;
            alen = ::strlen(ch);
            sw   = 0x00;
            if (offs < alen)
                sw = ch[offs++];

            switch (sw)
            {
            case 'm':
            {
                ++i;
                if (i < argc)
                    ctx.m_code = skClamp<SKint64>(std::strtol(argv[i], 0, 16), 0, SK_NPOS);
            }
            break;
            case 'h':
                return -1;
            case 'x':
            {
                if (offs < alen)
                    sw = ch[offs++];
                if (sw == 'c')
                    skDebugger::setPrintFlag(skDebugger::PF_DISABLE_COLOR);
                else if (sw == 'a')
                    ctx.m_flags &= ~PF_ASCII;
                break;
            }
            default:
                printf("unknown argument '%c'\n", sw);
                return -1;
            }
        }
    }

    if (argc >= 2)
    {
        if (!b2Alloc(ctx, argv[argc - 1]))
            err = true;
    }
    else
        err = true;


    if (err)
    {
        printf("Unable to load file\n");
        return -1;
    }
    return 0;
}

bool b2Alloc(b2ProgramInfo &ctx, const char *fname)
{
    skFileStream &fp = ctx.m_stream;

    if (fp.isOpen())
        fp.close();

    fp.open(fname, skStream::READ);
    return fp.isOpen();
}

void b2Print(b2ProgramInfo &ctx)
{
    skFileStream &fp = ctx.m_stream;

    SKuint8 buffer[1025];

    SKsize len = fp.size(), br, tr=0;
    while (!fp.eof())
    {
        br = fp.read(buffer, 1024);
        if (br != SK_NPOS && br > 0)
        {
            buffer[br] = 0;
            b2DumpHex(buffer, tr, br, ctx.m_flags, (SKuint32)ctx.m_code);
            tr += br;
        }
    }
}


void b2Free(b2ProgramInfo &ctx)
{
    ctx.m_stream.close();
}
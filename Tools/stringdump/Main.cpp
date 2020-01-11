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
#include "Utils/skArray.h"
#include "Utils/skFileStream.h"
#include "Utils/skString.h"

struct b2ProgramInfo
{
    skFileStream m_stream;
    SKuint32     m_addressRange[2];
    SKint32      m_minLimit;
    bool         m_alphaNum;
};


void b2Usage(void);
int  b2ParseCommandLine(b2ProgramInfo &ctx, int argc, char **argv);
bool b2Alloc(b2ProgramInfo &ctx, const char *fname);
void b2Print(b2ProgramInfo &ctx);
void b2Free(b2ProgramInfo &ctx);


int main(int argc, char **argv)
{
    b2ProgramInfo ctx = {skFileStream(), {(SKuint32)-1, (SKuint32)-1}, -1, false};

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
    std::cout << "stringdump <options> <path to file>                               \n";
    std::cout << "                                                                  \n";
    std::cout << "  Options:                                                        \n";
    std::cout << "      -h          Display this help message.                      \n";
    std::cout << "      -l          Min number of characters to qualify as a string.\n";
    std::cout << "      -an         Match only alpha numeric characters.            \n";
    std::cout << "      -ar         Specify a start address and a range.            \n";
    std::cout << "                      Input is in base 10 [0, file-length].       \n";
    std::cout << "                                                                  \n";
    std::cout << "                      [offset, offset + range]                    \n";
    std::cout << "                      start = pointer + offset                    \n";
    std::cout << "                      end   = pointer + (offset + range)          \n";
    std::cout << "                                                                  \n";
}

int b2ParseCommandLine(b2ProgramInfo &ctx, int argc, char **argv)
{
    if (argc <= 1)
        return -1;

    size_t alen, offs;
    int    i;
    char * ch;
    char   sw;
    bool   err = false, use_addr = false;

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
            case 'a':
            {
                if (offs < alen)
                    sw = ch[offs++];

                if (sw == 'r')
                {
                    if (i + 2 < argc)
                    {
                        ctx.m_addressRange[0] = std::strtol(argv[++i], 0, 10);
                        ctx.m_addressRange[1] = std::strtol(argv[++i], 0, 10);
                        use_addr              = true;
                    }
                    else
                    {
                        printf("Missing arguments to -ar\n");
                        return -1;
                    }
                }
                else if (sw == 'n')
                    ctx.m_alphaNum = true;
                else
                {
                    printf("unknown argument 'a%c'\n", sw);
                    return -1;
                }
                break;
            }
            case 'l':
            {
                if (offs < alen)
                    sw = ch[offs++];
                ctx.m_minLimit = std::strtol(argv[++i], 0, 10);
                break;
            }
            case 'h':
                return -1;
            default:
                printf("unknown argument '%c'\n", sw);
                return -1;
            }
        }
    }

    if (argc >= 2)
    {
        if (!use_addr)
            ctx.m_addressRange[0] = 0;

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
    SKuint8       buffer[1025];

    SKsize n;
    SKsize a, r;
    n = fp.size();
    a = skClamp<SKsize>(ctx.m_addressRange[0], 0, n);
    r = skClamp<SKsize>(ctx.m_addressRange[1], 0, n);

    if (ctx.m_addressRange[0] != -1)
        fp.seek(a, SEEK_SET);
    else
    {
        a = 0;
        r = n;
    }

    bool     inASCII = false;
    skString tmp;
    tmp.reserve(1024);
    SKsize br, tr = 0, i, st = 0;
    while (!fp.eof() && tr < r)
    {
        br = fp.read(buffer, skMin<SKsize>(1024, r));
        if (br != -1 && br > 0)
        {
            buffer[br] = 0;
            for (i = 0; i < br; ++i)
            {
                SKbyte b = buffer[i];
                inASCII  = b >= 0x20 && b < 0x7F;
                if (inASCII && !ctx.m_alphaNum)
                    tmp.append(b);
                else if (inASCII && ctx.m_alphaNum)
                {
                    if (b >= 'a' && b <= 'z' || b >= 'A' && b <= 'Z' || b >= '0' && b <= '9')
                        tmp.append(b);
                }
                else if (!tmp.empty() && !inASCII)
                {
                    inASCII = false;
                    if (ctx.m_minLimit != -1 && tmp.size() < ctx.m_minLimit)
                    {
                        tmp.resize(0);
                    }
                    else
                    {
                        printf("%s\n", tmp.c_str());
                        tmp.resize(0);
                    }
                }
            }
            tr += br;
        }
    }
}


void b2Free(b2ProgramInfo &ctx)
{
    ctx.m_stream.close();
}

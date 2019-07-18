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
#include "Utils/skDebugger.h"
#include "skBinaryFile.h"
#include "skPrintUtils.h"
#include "ProgramInfo.h"




int main(int argc, char** argv)
{
    HexDump_ProgramInfo prog = {MS_EXIT, 0, PF_DEFAULT, -1};
    
    if (HexDump_ParseCommandLine(prog, argc, argv) == -1)
    {
        HexDump_Usage();
        
        if (prog.m_fp)
            delete prog.m_fp;

        return -1;
    }

    
    if (prog.m_state == MS_MAIN)
    {
        // enter into interactive mode
        skPrintUtils::clear();
        while (prog.m_state == MS_MAIN)
            HexDump_Interactive(prog);
    }
    else
    {
        if (prog.m_fp)
            HexDump_PrintAll(prog);
    }

    delete prog.m_fp;
    return 0;
}



void HexDump_Usage(void)
{
    std::cout << "bindump [options] -f [path to file]\n";
    std::cout << "                                   \n";
    std::cout << "  Options:                         \n";
    std::cout << "                                   \n";
    std::cout << "  -m    Mark specific code [0-255].\n";
    std::cout << "  -b    Convert output to binary.  \n";
    std::cout << "  -a    Display ASCII listings.    \n";
    std::cout << "  -d    Display disassembly.       \n";
    std::cout << "  -h    Display this help message. \n";
    std::cout << "  -i    Interactive mode.          \n";
}



int HexDump_ParseCommandLine(HexDump_ProgramInfo& prog, int argc, char** argv)
{
    if (argc <= 1)
        return -1;

    size_t alen, offs = 0;
    int    i;
    char*  ch = 0;
    char   sw;

    for (i = 0; i < argc; i++)
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
            case 0x66:
            {
                ++i;
                if (i < argc)
                {
                    prog.m_fileName = argv[i];

                    prog.m_fp = skBinaryFile::createInstance(argv[i]);
                }
                else
                {
                    skPrintf("Missing file argument!\n");
                    return -1;
                }
            }
            break;
            case 0x6D:
            {
                ++i;
                if (i < argc)
                    prog.m_code = (int)std::strtol(argv[i], 0, 16);
            }
            break;
            case 0x62:
                prog.m_flags |= PF_BINARY;
                break;
            case 0x63:
                break;
            case 0x61:
                prog.m_flags |= PF_ASCII;
                break;
            case 0x64:
                prog.m_flags |= PF_DISASEMBLE;
                break;
            case 0x68:
                HexDump_Usage();
                return -1;
            case 0x69:
                prog.m_state = MS_MAIN;
                break;
            case 0x78:
            {
                if (offs < alen)
                    sw = ch[offs++];

                if (sw == 0x68)
                {
                    prog.m_flags |= PF_HEX;
                    break;
                }
            }
            default:
                skPrintf("unknown argument '%c'\n", sw);
                return -1;
            }
        }
    }
    return 0;
}

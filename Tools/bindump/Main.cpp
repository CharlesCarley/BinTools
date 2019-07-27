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
#include "ProgramInfo.h"
#include "Utils/skDebugger.h"
#include "skBinaryFile.h"
#include "skPrintUtils.h"



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
        {
            switch (prog.m_opt)
            {
            case 2:
                HexDump_PrintSections(prog);
                break;
            case 3:
                HexDump_PrintSectionNames(prog);
                break;
            case 4:
                HexDump_PrintSymbols(prog);
                break;
            default:
                HexDump_PrintAll(prog);
                break;
            }
        }
    }

    delete prog.m_fp;
    skPrintUtils::writeColor(CS_WHITE);
    return 0;
}



void HexDump_Usage(void)
{
    std::cout << "bindump <options> <path to file>                                      \n";
    std::cout << "                                                                      \n";
    std::cout << "  Options:                                                            \n";
    std::cout << "      -m [0-255]  Mark specific code.                                 \n";
    std::cout << "                                                                      \n";
    std::cout << "      -a          Display ASCII table.                                \n";
    std::cout << "      -b          Convert output to binary.                           \n";
    std::cout << "      -d          Display disassembly in code sections.               \n";
    std::cout << "      -h          Display this help message.                          \n";
    std::cout << "                                                                      \n";
    std::cout << "      -o [1-4]    Interactive menu option.                            \n";
    std::cout << "                  - 1. Print a hex dump of the files contents.        \n";
    std::cout << "                  - 2. Print section headers and the hex dump of each.\n";
    std::cout << "                  - 3. List all loaded section names                  \n";
    std::cout << "                  - 4. List all loaded symbols                        \n";
    std::cout << "                                                                      \n";
    std::cout << "      -i          Run in interactive mode.                            \n";
}



int HexDump_ParseCommandLine(HexDump_ProgramInfo& prog, int argc, char** argv)
{
    if (argc <= 1)
        return -1;

    size_t alen, offs = 0;
    int    i;
    char*  ch = 0;
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
                    prog.m_code = skClamp((int) std::strtol(argv[i], 0, 16), 0, 255);
            }
            break;
            case 'o':
            {
                ++i;
                if (i < argc)
                    prog.m_opt = skClamp((int) std::strtol(argv[i], 0, 10), 1, 4);
            }
            break;
            case 'a':
                prog.m_flags |= PF_ASCII;
                break;
            case 'b':
                prog.m_flags |= PF_BINARY;
                break;
            case 'd':
                prog.m_flags |= PF_DISASEMBLE;
                break;
            case 'h':
                HexDump_Usage();
                return -1;
            case 'i':
                prog.m_state = MS_MAIN;
                break;
            default:
                skPrintf("unknown argument '%c'\n", sw);
                return -1;
            }
        }
    }

    if (argc >= 2)
    {
        prog.m_fileName = argv[argc-1];

        prog.m_fp = skBinaryFile::load(argv[argc - 1]);
        if (!prog.m_fp)
            err = true;
    }
    else
        err = true;

    
    if (err)
    {
        skPrintf("Missing file argument!\n");
        return -1;
    }
    return 0;
}

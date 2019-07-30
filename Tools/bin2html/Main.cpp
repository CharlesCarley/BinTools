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
#include <fstream>
#include <string>
#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skBinaryFile.h"
#include "Templates.h"

struct ProgramInfo
{
    skBinaryFile* m_fp;
    string        m_inputFile;
    string        m_outputFile;
};

int  bin2html_parseCommandLine(ProgramInfo& inf, int arcg, char** argv);
void bin2html_usage(void);
void bin2html_write(ProgramInfo& info);



int main(int argc, char** argv)
{

    ProgramInfo prog = {0, ""};



    if (bin2html_parseCommandLine(prog, argc, argv) == -1)
    {
        bin2html_usage();

        if (prog.m_fp)
            delete prog.m_fp;

        return -1;
    }


    bin2html_write(prog);
    delete prog.m_fp;
    return 0;
}



void bin2html_usage(void)
{
    std::cout << "bin2html -o <output html file> -i <path to executable>\n";
}



int bin2html_parseCommandLine(ProgramInfo& prog, int argc, char** argv)
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
            case 'i':
            {
                ++i;
                if (i < argc && argv[i][0] != '-')
                {
                    prog.m_inputFile = argv[i];
                }
                else
                {
                    std::cout << ("Missing file argument to 'i'\n");
                }
                break;
            }
            case 'o':
            {
                ++i;
                if (i < argc && argv[i][0] != '-')
                {
                    prog.m_outputFile = argv[i];
                }
                else
                {
                    std::cout << ("Missing file argument to 'o'\n");
                }
                break;
            }
            default:
                std::cout << "Unknown argument '"<<sw<<"'\n";
                return -1;
            }
        }
    }

    if (prog.m_outputFile.empty())
    {
        std::cout << ("\nNo output file was extracted from the command line\n\n");
        return -1;
    }

    if (!prog.m_inputFile.empty())
        prog.m_fp = skBinaryFile::load(prog.m_inputFile.c_str());
    return 0;
}

void bin2html_write(ProgramInfo& info)
{
    std::ofstream fs;
    fs.open(info.m_outputFile);

    if (!fs.is_open())
    {
        std::cout << ("Failed to open output file.\n");
        return;
    }


    fs << HEADER;

    fs << FOOTER;
}

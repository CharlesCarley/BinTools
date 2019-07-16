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
#include "skSection.h"


enum HexDump_MenuState
{
    MS_MAIN = 0,
    MS_EXIT,
};

struct HexDump_ProgramInfo
{
    int           m_state;
    skBinaryFile* m_fp;
    int           m_flags;
    int           m_code;
    string        m_fileName;
};

int  HexDump_ParseCommandLine(HexDump_ProgramInfo& prog, int argc, char** argv);
void HexDump_Usage(void);
void HexDump_Interactive(HexDump_ProgramInfo& prog);
void HexDump_PrintAll(HexDump_ProgramInfo& prog);



int main(int argc, char** argv)
{
    HexDump_ProgramInfo prog = {MS_EXIT, 0, PF_DEFAULT, -1};
    HexDump_ParseCommandLine(prog, argc, argv);



    if (prog.m_state == MS_MAIN)
    {
        // enter into interactive mode
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

int HexDump_ParseCommandLine(HexDump_ProgramInfo& prog, int argc, char** argv)
{
    size_t alen, offs = 0;
    char*  ch = 0;
    char   sw;

    for (int i = 0; i < argc; i++)
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
                    prog.m_fp       = skBinaryFile::createInstance(argv[i]);
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
            case 0x68:
                HexDump_Usage();
                return -1;
            case 'i':
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

void HexDump_Usage(void)
{
    skPrintf("Usage: bindump [options] -f [path to file]\n");
    skPrintf("  Options:\n");
    skPrintf("    -m [0-255] mark specific code.\n");
    skPrintf("    -b  -- convert output to binary.\n");
    skPrintf("    -a  -- display ASCII listings.\n");
    skPrintf("    -h  -- display this help message.\n");
    skPrintf("    -i  -- interactive mode.\n");
    skPrintf("    -c  -- separate output into colors in order to see it better.\n");
}


void HexDump_PrintAll(HexDump_ProgramInfo& prog)
{
    if (prog.m_fp)
    {
        skPrintUtils::dumpHex((void*)prog.m_fp->ptr(),
                              prog.m_fp->length(),
                              prog.m_flags,
                              prog.m_code);
    }
}


void HexDump_PrintSectionNames(HexDump_ProgramInfo& prog)
{
    if (prog.m_fp)
    {
        skBinaryFile::StringArray::Iterator it = prog.m_fp->getSectionHeaderNames().iterator();

        int i = 1;
        while (it.hasMoreElements())
        {
            skString& str = it.getNext();
            skPrintf("%-2i\t%s\n", i, str.c_str());
            ++i;
        }
        skPrintf("\n\n");
    }
}


void HexDump_PrintSections(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        skBinaryFile::SectionMap::Iterator it = bin->getSectionIterator();
        while (it.hasMoreElements())
        {
            skSection* sec = it.getNext().second;

            const skString& name = sec->getname();

            skPrintUtils::writeSeperator();
            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_DARKYELLOW);
            skPrintf("\t\t\tSectionInfo: %s\n", name.c_str());
            skPrintUtils::writeSeperator();

            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_WHITE);
            if (prog.m_flags & PF_DISASEMBLE)
                sec->dissemble(prog.m_flags);
            else
                skPrintUtils::dumpHex(sec->ptr(), sec->size(), prog.m_flags, prog.m_code);
        }
    }
}



void HexDump_PrintSection(HexDump_ProgramInfo& prog, const std::string& name)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        skBinaryFile* bin = prog.m_fp;

        skSection* sec = bin->getSection(name.c_str());
        if (sec != 0)
        {
            const skString& name = sec->getname();
            skPrintUtils::writeSeperator();
            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_DARKYELLOW);

            skPrintf("\t\t\tSectionInfo: %s\n", name.c_str());
            skPrintUtils::writeSeperator();


            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_WHITE);

            if (prog.m_flags & PF_DISASEMBLE)
                sec->dissemble(prog.m_flags);
            else
                skPrintUtils::dumpHex(sec->ptr(), sec->size(), prog.m_flags, prog.m_code);
        }
    }
}


void HexDump_Interactive(HexDump_ProgramInfo& prog)
{
    cout << "                                                     \n";
    cout << " Please Select From The Following Menu:              \n";
    cout << "                                                     \n";
    cout << "   1. Print hex dump.                                \n";
    cout << "   2. Print section headers and the hex dump of each.\n";
    cout << "   3. Print section names.                           \n";
    cout << "   4. Display specific section name.                 \n";
    cout << "      .bss, .init, .text, etc                        \n";
    cout << "   A. Display ascii                                  \n";
    cout << "   B. Display Binary                                 \n";
    cout << "   D. Display Disassembly                            \n";
    cout << "   H. Display Hex                                    \n";
    cout << "   M. Mark specific code.                            \n";
    cout << "   F. Load file.                                     \n";
    cout << "                                            Q-Exit.  \n\n";
    char opt;
    cout << prog.m_fileName << ">";
    cin >> opt;

    switch (opt)
    {
        break;
    case 'F':
    case 'f':
    {
        cout << "Path to file:";
        string path;
        cin >> path;
        if (prog.m_fp)
            delete prog.m_fp;

        prog.m_fp = skBinaryFile::createInstance(path.c_str());
        if (prog.m_fp)
            prog.m_fileName = path;

        skPrintUtils::clear();
    }
    break;
    case '1':
    {
        HexDump_PrintAll(prog);
        skPrintUtils::pause();
    }
    break;
    case '2':
    {
        HexDump_PrintSections(prog);
        skPrintUtils::pause();
    }
    break;
    case '3':
        HexDump_PrintSectionNames(prog);
        skPrintUtils::pause();
        break;
    case '4':
    {
        cout << "Section Name>";
        string sn;
        cin >> sn;
        cout << "\n";
        HexDump_PrintSection(prog, sn);
        skPrintUtils::pause();
        break;
    }
    case 'A':
    case 'a':
    {
        cout << "Display ASCII? (Y|N)>";
        char ans;
        cin >> ans;
        if (ans == 'y' || ans == 'Y')
            prog.m_flags |= PF_ASCII;
        else if (ans == 'n' || ans == 'N')
            prog.m_flags &= ~PF_ASCII;
        skPrintUtils::clear();
    }
    break;
    case 'B':
    case 'b':
    {
        cout << "Display binary? (Y|N)>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            prog.m_flags |= PF_BINARY;
        else if (ans == 'n' || ans == 'N')
            prog.m_flags &= ~PF_BINARY;
        skPrintUtils::clear();
    }
    break;
    case 'D':
    case 'd':
    {
        cout << "Display disassembly? (Y|N)>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            prog.m_flags |= PF_DISASEMBLE;
        else if (ans == 'n' || ans == 'N')
            prog.m_flags &= ~PF_DISASEMBLE;
        skPrintUtils::clear();
    }
    break;
    case 'H':
    case 'h':
    {
        cout << "Display hex? (Y|N)>";
        char ans;
        cin >> ans;


        if (ans == 'y' || ans == 'Y')
            prog.m_flags |= PF_HEX;
        else if (ans == 'n' || ans == 'N')
            prog.m_flags &= ~PF_HEX;

        skPrintUtils::clear();
    }
    break;
    case 'M':
    case 'm':
    {
        cout << "Mark code>";
        string sn;
        cin >> sn;

        prog.m_code = (int)std::strtol(sn.c_str(), 0, 16);
        skPrintUtils::clear();
    }
    break;
        break;
    case 'Q':
    case 'q':
        prog.m_state = MS_EXIT;
        break;
    default:
        skPrintUtils::clear();
        break;
    }
}

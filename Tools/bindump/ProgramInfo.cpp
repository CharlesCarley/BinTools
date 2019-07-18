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
#include "ELF/skElf.h"
#include "ELF/skElfSection.h"
#include "ELF/skElfUtils.h"

#include "PE/skPortableFile.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableUtils.h"

#include "skSection.h"


#include "ProgramInfo.h"



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


void HexDump_PrintSectionHeader(skBinaryFile *fp, skSection* section)
{
    if (!section || !fp)
        return;

    // access the derived class

    skFileFormat fileFormat = fp->getFormat();


    if (fileFormat == FF_ELF)
    {
        skElfSection* sectionHeader = static_cast<skElfSection*>(section);

        const skElfSectionHeader64& header = sectionHeader->getHeader();

        skElfUtils::printSectionHeader(header);

    }
    else if (fileFormat == FF_PE)
    {
        skPortableSection* pe = static_cast<skPortableSection*>(section);
        const COFFSectionHeader& header = pe->getHeader();

        skPortableUtils::printSectionHeader(header);
    }
}



void HexDump_PrintSection(HexDump_ProgramInfo& prog, skSection* section)
{
    if (!section || !prog.m_fp)
        return;

    const skString& name = section->getName();

    skPrintUtils::writeSeperator();
    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_DARKYELLOW);

    skPrintf("Section Header: %s\n", name.c_str());
    skPrintUtils::writeSeperator();
    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_LIGHT_GREY);

    HexDump_PrintSectionHeader(prog.m_fp, section);

    skPrintUtils::writeSeperator();

    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_WHITE);
    if (prog.m_flags & PF_DISASEMBLE)
        section->dissemble(prog.m_flags);
    else
        skPrintUtils::dumpHex(section->ptr(), section->size(), prog.m_flags, prog.m_code);
}


void HexDump_PrintSections(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        skBinaryFile::SectionMap::Iterator it = bin->getSectionIterator();
        while (it.hasMoreElements())
            HexDump_PrintSection(prog, it.getNext().second);
    }
}



void HexDump_PrintSection(HexDump_ProgramInfo& prog, const std::string& name)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        skSection* sec = bin->getSection(name.c_str());
        if (sec != 0)
            HexDump_PrintSection(prog, sec);
    }
}



void HexDump_Interactive(HexDump_ProgramInfo& prog)
{
    std::cout << "                                                    \n";
    std::cout << " Please Select From The Following Menu:             \n";
    std::cout << "                                                    \n";
    std::cout << " Print Options:                                     \n";
    std::cout << "   1. Print hex dump                                \n";
    std::cout << "   2. Print section headers and the hex dump of each\n";
    std::cout << "   3. Print section names                           \n";
    std::cout << "   4. Display specific section name                 \n";
    std::cout << "      .bss, .init, .text, etc                       \n";
    std::cout << " Print Options:                                     \n";
    std::cout << "   A. Display ASCII                                 \n";
    std::cout << "   B. Display Binary                                \n";
    std::cout << "   D. Display Disassembly                           \n";
    std::cout << "   H. Display Hex                                   \n";
    std::cout << "   M. Mark specific code                            \n";
    std::cout << " File Options:                                      \n";
    std::cout << "   F. Load file                                     \n";
    std::cout << "                                            Q-Exit. \n";
    char opt;
    cout << prog.m_fileName << ">";
    cin >> opt;

    switch (opt)
    {
        break;
    case 'F':
    case 'f':
    {
        cout << "Path to file: ";
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
        cout << "Section Name> ";
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
        cout << "Display ASCII ? (Y|N)>";
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
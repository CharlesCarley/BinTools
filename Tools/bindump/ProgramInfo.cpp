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
#include "ELF/skElfFile.h"
#include "ELF/skElfSection.h"
#include "ELF/skElfUtils.h"
#include "PE/skPortableFile.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableUtils.h"
#include "ProgramInfo.h"
#include "Utils/skDebugger.h"
#include "skBinaryFile.h"
#include "skPrintUtils.h"
#include "skSection.h"
#include "skSymbol.h"



void HexDump_PrintAll(HexDump_ProgramInfo& prog)
{
    if (prog.m_fp)
    {
        skPrintUtils::dumpHex((void*)prog.m_fp->getPointer(),
                              0,
                              prog.m_fp->getLength(),
                              prog.m_flags,
                              prog.m_code);
    }
}


void HexDump_PrintSectionNames(HexDump_ProgramInfo& prog)
{
    if (prog.m_fp)
    {
        skBinaryFile::SectionTable::Iterator it = prog.m_fp->getSectionIterator();


        if (prog.m_flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_DARKYELLOW);

        skPrintf("\nSections:\n\n");

        if (prog.m_flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_GREY);
       skPrintf(" Name                 Offset             Index\n\n");
        if (prog.m_flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_LIGHT_GREY);
        int i = 0;
        while (it.hasMoreElements())
        {
            skSection*      sec = it.getNext().second;
            const skString& str = sec->getName();
            SKuint64 offs = (SKuint64)sec->getStartAddress();
            skPrintf(" %-20s 0x%-16llx %-2u\n", str.c_str(), offs, i);
            ++i;
        }
        skPrintf("\n\n");
    }
}


void HexDump_PrintSectionHeader(skBinaryFile* fp, skSection* section)
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
        skPortableSection*       pe     = static_cast<skPortableSection*>(section);
        const COFFSectionHeader& header = pe->getHeader();

        skPortableUtils::printHeader(header);
    }
}



void HexDump_PrintSectionCommon(HexDump_ProgramInfo& prog, skSection* section)
{
    if (!section || !prog.m_fp)
        return;

    const skString& name = section->getName();

    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_DARKYELLOW);

    skPrintf("\nSection Header: %s\n\n", name.c_str());

    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_LIGHT_GREY);

    HexDump_PrintSectionHeader(prog.m_fp, section);
}


void HexDump_PrintSection(HexDump_ProgramInfo& prog, skSection* section)
{
    if (!section || !prog.m_fp)
        return;

    HexDump_PrintSectionCommon(prog, section);
    skPrintf("\n");

    if (prog.m_flags & PF_COLORIZE)
        skPrintUtils::writeColor(CS_WHITE);
    if (prog.m_flags & PF_DISASEMBLE)
        section->dissemble(prog.m_flags, prog.m_code);
    else
        skPrintUtils::dumpHex(section->getPointer(),
                              section->getStartAddress(),
                              section->getSize(),
                              prog.m_flags,
                              prog.m_code);
}

void HexDump_PrintHeadersCommon(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        // access the derived class
        skFileFormat fileFormat = bin->getFormat();

        if (fileFormat == FF_ELF)
        {
            skElfFile* elf = static_cast<skElfFile*>(bin);

            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_DARKYELLOW);

            skPrintf("File Header:\n\n");

            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_LIGHT_GREY);

            const skElfHeaderInfo64& header = elf->getHeader();
            skElfUtils::printElfHeader(header);
        }
        else if (fileFormat == FF_PE)
        {
            skPortableFile* pe = static_cast<skPortableFile*>(bin);

            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_DARKYELLOW);

            skPrintf("File Header:\n\n");

            if (prog.m_flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_LIGHT_GREY);

            const COFFHeader& header = pe->getCommonHeader();

            skPortableUtils::printHeader(header);


            // Print the varying header.
            skFileFormatType fpt = pe->getPlatformType();
            if (fpt == FFT_32BIT)
            {
                COFFOptionalHeader32 dest;
                pe->getOptionalHeader(dest);
                skPortableUtils::printHeader(dest);
            }
            else if (fpt == FFT_64BIT)
            {
                COFFOptionalHeader64 dest;
                pe->getOptionalHeader(dest);
                skPortableUtils::printHeader(dest);
            }
        }
    }
}


void HexDump_PrintSections(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {

        HexDump_PrintHeadersCommon(prog);

        skBinaryFile::SectionTable::Iterator it = bin->getSectionIterator();
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


void HexDump_PrintSymbols(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {

        skBinaryFile::SymbolTable::Iterator it = bin->getSymbolIterator();

        if (prog.m_flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_DARKYELLOW);

        skPrintf("\nSymbols:\n\n");

        if (prog.m_flags & PF_COLORIZE)
            skPrintUtils::writeColor(CS_GREY);

        if (!it.hasMoreElements())
            skPrintf("No symbols.\n");
        else
        {
            while (it.hasMoreElements())
            {
                skSymbol* sym = it.getNext().second;
                if (prog.m_flags & PF_COLORIZE)
                    skPrintUtils::writeColor(CS_LIGHT_GREY);
    
                
                skPrintf("%16llx   %s\n", sym->getAddress(), sym->getName().c_str());
            }
        }
    }
}


void HexDump_PrintAllHeaders(HexDump_ProgramInfo& prog)
{
    skBinaryFile* bin = prog.m_fp;
    if (bin)
    {
        HexDump_PrintHeadersCommon(prog);

        skBinaryFile::SectionTable::Iterator it = bin->getSectionIterator();
        while (it.hasMoreElements())
            HexDump_PrintSectionCommon(prog, it.getNext().second);
    }
}


void HexDump_Interactive(HexDump_ProgramInfo& prog)
{
    std::cout << "Please Select From The Following Menu:                \n";
    std::cout << "                                                      \n";
    std::cout << "  Print Options:                                      \n";
    std::cout << "    1. Print a hex dump of the files contents.        \n";
    std::cout << "    2. Print section headers and the hex dump of each.\n";
    std::cout << "    3. List all loaded section names.                 \n";
    std::cout << "    4. List all loaded symbols.                       \n";
    std::cout << "    5. Display only headers.                          \n";
    std::cout << "    6. Display specific section name.                 \n";
    std::cout << "         .bss, .init, .text, etc                      \n";
    std::cout << "                                                      \n";
    std::cout << "  Display Options:                                    \n";
    std::cout << "    A. Display ASCII                                  \n";
    std::cout << "    B. Display Binary                                 \n";
    std::cout << "    D. Display Disassembly                            \n";
    std::cout << "    H. Display Hex                                    \n";
    std::cout << "    M. Mark specific code.                            \n";
    std::cout << "                                                      \n";
    std::cout << "  File Options:                                       \n";
    std::cout << "    F. Path to file                                   \n";
    std::cout << "                                                      \n";
    std::cout << "                                        Q-Exit.       \n";

    char opt;

    cout << prog.m_fileName << ">";
    cin >> opt;

    switch (opt)
    {
    case 'F':
    case 'f':
    {
        cout << "Path to file: ";
        string path;
        cin >> path;
        if (prog.m_fp)
            delete prog.m_fp;

        prog.m_fp = skBinaryFile::load(path.c_str());
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
        HexDump_PrintSymbols(prog);
        skPrintUtils::pause();
        break;
    case '5':
        HexDump_PrintAllHeaders(prog);
        skPrintUtils::pause();
        break;
    case '6':
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

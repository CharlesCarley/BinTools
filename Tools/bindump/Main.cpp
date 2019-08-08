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
#include "ELF/skElfSymbol.h"
#include "ELF/skElfUtils.h"
#include "Extras/skElfPrintUtils.h"
#include "Extras/skPortablePrintUtils.h"
#include "Extras/skPrintUtils.h"
#include "PE/skPortableDirectory.h"
#include "PE/skPortableFile.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableUtils.h"
#include "Utils/skDebugger.h"
#include "skBinaryFile.h"


enum b2MenuState
{
    MS_MAIN = 0,
    MS_EXIT,
};


struct b2ProgramInfo
{
    int           m_state;
    skBinaryFile* m_fp;
    int           m_flags;
    int           m_code;
    int           m_opt;
    string        m_fileName;
};


int  b2ParseCommandLine(int argc, char** argv);
void b2Usage(void);
void b2Interactive(void);

void b2WriteColor(skConsoleColorSpace cs);
void b2WriteAddress(SKuint64 addr);
void b2DumpHex(void* ptr, size_t offset, size_t len, int flags = PF_DEFAULT, int mark = -1, bool nl = true);


void b2PrintAll(void);
void b2PrintSectionNames(void);
void b2PrintSections(void);
void b2PrintSymbols(void);


b2ProgramInfo ctx = {MS_EXIT, 0, PF_DEFAULT, -1, -1};


int main(int argc, char** argv)
{
    if (b2ParseCommandLine(argc, argv) == -1)
    {
        b2Usage();

        if (ctx.m_fp)
            delete ctx.m_fp;

        return -1;
    }

    if (ctx.m_state == MS_MAIN)
    {
        // enter into interactive mode
        skClear();
        while (ctx.m_state == MS_MAIN)
            b2Interactive();
    }
    else
    {
        if (ctx.m_fp)
        {
            switch (ctx.m_opt)
            {
            case 2:
                b2PrintSections();
                break;
            case 3:
                b2PrintSectionNames();
                break;
            case 4:
                b2PrintSymbols();
                break;
            default:
                b2PrintAll();
                break;
            }
        }
    }

    delete ctx.m_fp;

    b2WriteColor(CS_WHITE);
    return 0;
}



void b2Usage(void)
{
    std::cout << "bindump <options> <path to file>                                  \n";
    std::cout << "                                                                  \n";
    std::cout << "  Options:                                                        \n";
    std::cout << "      -m [0-255]  Mark specific code.                             \n";
    std::cout << "                                                                  \n";
    std::cout << "      -a          Remove the ASCII table in the hex dump output.  \n";
    std::cout << "      -b          Display a binary table in the hex dump output.  \n";
    std::cout << "      -d          Display disassembly in code sections.           \n";
    std::cout << "      -h          Display this help message.                      \n";
    std::cout << "      -xc         Remove color output.                            \n";
    std::cout << "                                                                  \n";
    std::cout << "      -o [1-5]    Interactive menu option.                        \n";
    std::cout << "                  - 1. Print a hex dump of the files contents.    \n";
    std::cout << "                  - 2. Print section headers and the dump of each.\n";
    std::cout << "                  - 3. List all loaded section names.             \n";
    std::cout << "                  - 4. List all loaded symbols.                   \n";
    std::cout << "                  - 5. Display only headers.                      \n";
    std::cout << "                                                                  \n";
    std::cout << "      -i          Run in interactive mode.                        \n";
}



int b2ParseCommandLine(int argc, char** argv)
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
                    ctx.m_code = skClamp((int)std::strtol(argv[i], 0, 16), 0, 255);
            }
            break;
            case 'o':
            {
                ++i;
                if (i < argc)
                    ctx.m_opt = skClamp((int)std::strtol(argv[i], 0, 10), 1, 5);
            }
            break;
            case 'a':
                ctx.m_flags &= ~PF_ASCII;
                break;
            case 'b':
                ctx.m_flags |= PF_BINARY;
                ctx.m_flags &= ~PF_ASCII;
                ctx.m_flags &= ~PF_HEX;
                break;
            case 'd':
                ctx.m_flags |= PF_DISASEMBLE;
                break;
            case 'h':
                return -1;
            case 'i':
                ctx.m_state = MS_MAIN;
                break;
            case 'x':
            {
                if (offs < alen)
                    sw = ch[offs++];
                if (sw == 'c')
                    skDebugger::setPrintFlag(skDebugger::PF_DISABLE_COLOR);
                break;
            }
            default:
                skPrintf("unknown argument '%c'\n", sw);
                return -1;
            }
        }
    }

    if (argc >= 2)
    {
        ctx.m_fileName = argv[argc - 1];

        ctx.m_fp = skBinaryFile::load(argv[argc - 1]);
        if (!ctx.m_fp)
            err = true;
    }
    else
        err = true;


    if (err)
    {
        skPrintf("Unable to load file\n");
        return -1;
    }
    return 0;
}



void b2WriteColor(skConsoleColorSpace cs)
{
    skDebugger::writeColor(cs);
}



void b2doMarkColor(int c, int mark);
void b2writeAscii(char* cp, SKsize offs, SKsize max, int flags, int mark);
void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark);


void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKsize j;
    for (j = 0; j < 16; ++j)
    {
        if (cp)
        {
            if (j % 8 == 0)
                skPrintf(" ");

            if (offs + j < max)
            {
                unsigned char np = (unsigned char)cp[offs + j];

                b2doMarkColor((int)np, mark);
                skPrintf("%02X ", (np));
            }
            else
                skPrintf("   ");
        }
    }
}



void b2writeAscii(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKsize j;
    if (!cp)
        return;


    b2WriteColor(CS_WHITE);

    skPrintf(" |");
    for (j = 0; j < 16; ++j)
    {
        if (offs + j < max)
        {
            unsigned char np = (unsigned char)cp[offs + j];


            b2doMarkColor((int)np, mark);

            if (np >= 0x20 && np < 0x7F)
                skPrintf("%c", np);
            else
                skPrintf(".");
        }
        else
            skPrintf(" ");
    }


    b2WriteColor(CS_WHITE);
    skPrintf("|");
}


void b2doMarkColor(int c, int mark)
{
    if (c == mark)
        b2WriteColor(CS_RED);
    else if (c == 0x00 || c == 0xCC)
        b2WriteColor(CS_GREY);
    else
        b2WriteColor(CS_WHITE);
}



void b2WriteAddress(SKuint64 addr)
{
    b2WriteColor(CS_GREY);
    skPrintf("%16llx  ", addr);
}


void b2DumpHex(void* ptr, size_t offset, size_t len, int flags, int mark, bool nl)
{
    char* cp = (char*)ptr;
    for (SKsize i = 0; i < len; i += 16)
    {
        if (flags & PF_ADDRESS)
            b2WriteAddress((size_t)(i + offset));
        if (flags & PF_HEX)
            b2WriteHex(cp, i, len, flags, mark);
        if (flags & PF_ASCII)
            b2writeAscii(cp, i, len, flags, mark);
        if (nl)
            skPrintf("\n");
    }
}


void b2PrintAll(void)
{
    if (ctx.m_fp)
    {
        skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();
        while (it.hasMoreElements())
        {
            skSection* sec = it.getNext().second;

            b2DumpHex(sec->getPointer(),
                      sec->getStartAddress(),
                      sec->getSize(),
                      ctx.m_flags,
                      ctx.m_code);

            skPrintf("%16s*\n", " ");
        }
    }
}


void b2PrintSectionNames(void)
{
    if (ctx.m_fp)
    {
        skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();

        b2WriteColor(CS_DARKYELLOW);
        skPrintf("\nSections:\n\n");


        b2WriteColor(CS_GREY);
        skPrintf(" Name                 Offset             Index\n\n");



        b2WriteColor(CS_LIGHT_GREY);
        int i = 0;
        while (it.hasMoreElements())
        {
            skSection* sec = it.getNext().second;

            const skString& str = sec->getName();

            SKuint64 offs = (SKuint64)sec->getStartAddress();
            skPrintf(" %-20s 0x%-16llx %-2u\n", str.c_str(), offs, i);
            ++i;
        }

        if (ctx.m_fp->getFormat() == FF_PE)
        {
            b2WriteColor(CS_DARKYELLOW);
            skPrintf("\nData Directories:\n\n");


            b2WriteColor(CS_GREY);
            it = ctx.m_fp->getSectionIterator();
            skPrintf(" Type                 RVA                Size\n\n");


            b2WriteColor(CS_LIGHT_GREY);
            while (it.hasMoreElements())
            {
                skPortableSection* sec = reinterpret_cast<skPortableSection*>(it.getNext().second);

                skPortableSection::Directories::Iterator dit = sec->getDirectoryIterator();
                while (dit.hasMoreElements())
                {
                    skPortableDirectory* dir = dit.getNext();
                    skPrintf(" %-20u 0x%-16x %-2u\n", dir->getType(), dir->getRVA(), dir->getSize());
                }
            }
        }

        skPrintf("\n\n");
    }
}


void b2PrintSectionHeader(skBinaryFile* fp, skSection* section)
{
    if (!section || !fp)
        return;

    skFileFormat fileFormat = fp->getFormat();


    if (fileFormat == FF_ELF)
    {
        skElfSection*               sectionHeader = static_cast<skElfSection*>(section);
        const skElfSectionHeader64& header        = sectionHeader->getHeader();
        skElfPrintUtils::printSectionHeader(header);
    }
    else if (fileFormat == FF_PE)
    {
        skPortableSection*       pe     = static_cast<skPortableSection*>(section);
        const COFFSectionHeader& header = pe->getHeader();

        skPortableUtils::printHeader(header);
    }
}



void b2PrintSectionCommon(skSection* section)
{
    if (!section || !ctx.m_fp)
        return;

    const skString& name = section->getName();


    b2WriteColor(CS_DARKYELLOW);
    skPrintf("\nSection Header: %s\n\n", name.c_str());


    b2WriteColor(CS_LIGHT_GREY);
    b2PrintSectionHeader(ctx.m_fp, section);
}


void b2PrintSection(skSection* section)
{
    if (!section || !ctx.m_fp)
        return;

    b2PrintSectionCommon(section);
    skPrintf("\n");


    b2WriteColor(CS_WHITE);
    if (ctx.m_flags & PF_DISASEMBLE)
        section->dissemble(ctx.m_flags, ctx.m_code);
    else
        b2DumpHex(section->getPointer(),
                  section->getStartAddress(),
                  section->getSize(),
                  ctx.m_flags,
                  ctx.m_code);
}

void b2PrintHeadersCommon(void)
{
    skBinaryFile* bin = ctx.m_fp;
    if (bin)
    {
        skFileFormat fileFormat = bin->getFormat();

        if (fileFormat == FF_ELF)
        {
            skElfFile* elf = static_cast<skElfFile*>(bin);


            b2WriteColor(CS_DARKYELLOW);
            skPrintf("File Header:\n\n");


            b2WriteColor(CS_LIGHT_GREY);
            const skElfHeaderInfo64& header = elf->getHeader();
            skElfPrintUtils::printElfHeader(header);
        }
        else if (fileFormat == FF_PE)
        {
            skPortableFile* pe = static_cast<skPortableFile*>(bin);


            b2WriteColor(CS_DARKYELLOW);
            skPrintf("File Header:\n\n");


            b2WriteColor(CS_LIGHT_GREY);
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


void b2PrintSections(void)
{
    skBinaryFile* bin = ctx.m_fp;
    if (bin)
    {
        b2PrintHeadersCommon();

        skBinaryFile::SectionTable::Iterator it = bin->getSectionIterator();
        while (it.hasMoreElements())
            b2PrintSection(it.getNext().second);
    }
}



void b2PrintSection(const std::string& name)
{
    skBinaryFile* bin = ctx.m_fp;
    if (bin)
    {
        skSection* sec = bin->getSection(name.c_str());
        if (sec != 0)
            b2PrintSection(sec);
    }
}


void b2PrintSymbols(void)
{
    skBinaryFile* bin = ctx.m_fp;
    if (bin)
    {
        skBinaryFile::SymbolTable::Iterator it = bin->getSymbolIterator();


        b2WriteColor(CS_DARKYELLOW);
        skPrintf("\nSymbols:\n\n");


        b2WriteColor(CS_GREY);
        if (!it.hasMoreElements())
            skPrintf("No symbols.\n");
        else
        {
            while (it.hasMoreElements())
            {
                skSymbol* sym = it.getNext().second;

                b2WriteColor(CS_DARKYELLOW);
                skPrintf("%s:0x%-16llx\n", sym->getName().c_str(), sym->getAddress());

                b2WriteColor(CS_LIGHT_GREY);
                if (bin->getFormat() == FF_ELF)
                {
                    skElfSymbol*         est  = reinterpret_cast<skElfSymbol*>(sym);
                    const skElfSymbol64& esym = est->getSymbol();

                    skPrintf("\tname   %u\n\tinfo   %u\n\tother  %u\n\tindex   %u\n\tsize   %llu\n",
                             esym.m_name,
                             esym.m_info & 0xF,
                             esym.m_other,
                             esym.m_sectionIdx,
                             esym.m_size);
                }
            }
        }
    }
}


void b2PrintAllHeaders(void)
{
    skBinaryFile* bin = ctx.m_fp;
    if (bin)
    {
        b2PrintHeadersCommon();

        skBinaryFile::SectionTable::Iterator it = bin->getSectionIterator();
        while (it.hasMoreElements())
            b2PrintSectionCommon(it.getNext().second);
    }
}


void b2Interactive(void)
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

    cout << ctx.m_fileName << ">";
    cin >> opt;

    switch (opt)
    {
    case 'F':
    case 'f':
    {
        cout << "Path to file: ";
        string path;
        cin >> path;
        if (ctx.m_fp)
            delete ctx.m_fp;

        ctx.m_fp = skBinaryFile::load(path.c_str());
        if (ctx.m_fp)
            ctx.m_fileName = path;
    }
    break;
    case '1':
    {
        b2PrintAll();

        skPause();
    }
    break;
    case '2':
    {
        b2PrintSections();
        skPause();
    }
    break;
    case '3':
        b2PrintSectionNames();
        skPause();
        break;
    case '4':
        b2PrintSymbols();
        skPause();
        break;
    case '5':
        b2PrintAllHeaders();
        skPause();
        break;
    case '6':
    {
        cout << "Section Name>";
        string sn;
        cin >> sn;
        cout << "\n";

        b2PrintSection(sn);
        skPause();
        break;
    }
    case 'A':
    case 'a':
    {
        cout << "Display ASCII ? (Y|N)>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            ctx.m_flags |= PF_ASCII;
        else if (ans == 'n' || ans == 'N')
            ctx.m_flags &= ~PF_ASCII;
    }
    break;
    case 'B':
    case 'b':
    {
        cout << "Display binary? (Y|N)>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            ctx.m_flags |= PF_BINARY;
        else if (ans == 'n' || ans == 'N')
            ctx.m_flags &= ~PF_BINARY;
    }
    break;
    case 'D':
    case 'd':
    {
        cout << "Display disassembly? (Y|N)>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            ctx.m_flags |= PF_DISASEMBLE;
        else if (ans == 'n' || ans == 'N')
            ctx.m_flags &= ~PF_DISASEMBLE;
    }
    break;
    case 'H':
    case 'h':
    {
        cout << "Display hex? (Y|N)>";
        char ans;
        cin >> ans;


        if (ans == 'y' || ans == 'Y')
            ctx.m_flags |= PF_HEX;
        else if (ans == 'n' || ans == 'N')
            ctx.m_flags &= ~PF_HEX;
    }
    break;
    case 'M':
    case 'm':
    {
        cout << "Mark code>";
        string sn;
        cin >> sn;

        ctx.m_code = (int)std::strtol(sn.c_str(), 0, 16);
    }
    break;
        break;
    case 'Q':
    case 'q':
        ctx.m_state = MS_EXIT;
        break;
    default:
        break;
    }

    if (ctx.m_state != MS_EXIT)
        skClear();
}

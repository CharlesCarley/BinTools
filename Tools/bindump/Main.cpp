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
#include "ELF/skElfPrintUtils.h"
#include "ELF/skElfSection.h"
#include "ELF/skElfSymbol.h"
#include "ELF/skElfUtils.h"
#include "PE/skPortableDirectory.h"
#include "PE/skPortableFile.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableUtils.h"
#include "Utils/skDebugger.h"
#include "Utils/skTimer.h"
#include "capstone/capstone.h"
#include "skBinaryFile.h"

enum b2MenuState
{
    MS_MAIN = 0,
    MS_EXIT,
};

enum b2PrintFlags
{
    PF_NONE       = 0,
    PF_COLORIZE   = (1 << 0),
    PF_HEX        = (1 << 1),
    PF_ASCII      = (1 << 2),
    PF_ADDRESS    = (1 << 3),
    PF_DISASEMBLE = (1 << 4),  // ignores hex in place of disassembly
    PF_DEFAULT    = PF_COLORIZE | PF_ADDRESS | PF_HEX | PF_ASCII,
    PF_HEXDIS     = PF_COLORIZE | PF_HEX,
};


struct b2ProgramInfo
{
    int           m_state;
    skBinaryFile* m_fp;
    int           m_flags;
    int           m_code;
    int           m_opt;
    size_t        m_handle;
    string        m_fname;
};


int  b2ParseCommandLine(int argc, char** argv);
void b2Usage(void);
void b2Interactive(void);


void b2Free(void);
bool b2Alloc(const char* prog);

void b2WriteColor(skConsoleColorSpace cs);
void b2WriteAddress(SKuint64 addr);

void b2DumpHex(void* ptr, SKuint32 offset, SKuint32 len, int flags = PF_DEFAULT, int mark = -1, bool nl = true);
void b2Dissemble(void* ptr, size_t offset, size_t len, int flags = PF_DEFAULT);

void b2MarkColor(int c, int mark);
void b2WriteAscii(char* cp, SKsize offs, SKsize max, int flags, int mark);
void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark);

void b2PrintAll(void);
void b2PrintSectionNames(void);
void b2PrintSections(void);
void b2PrintSymbols(void);


b2ProgramInfo ctx = {MS_EXIT, 0, PF_DEFAULT, -1, -1, SK_NPOS};


int main(int argc, char** argv)
{
    if (b2ParseCommandLine(argc, argv) == -1)
    {
        b2Usage();
        b2Free();
        return -1;
    }

    if (ctx.m_state == MS_MAIN)
    {
        // enter into interactive mode
        skConsoleClear();
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


    b2Free();
    b2WriteColor(CS_WHITE);
    return 0;
}



void b2Usage(void)
{
    std::cout << "bindump <options> <path to file>                                  \n";
    std::cout << "                                                                  \n";
    std::cout << "  Options:                                                        \n";
    std::cout << "      -h          Display this help message.                      \n";
    std::cout << "      -m [0-255]  Mark specific code.                             \n";
    std::cout << "      -d          Display disassembly in code sections.           \n";
    std::cout << "      -xc         Remove color output.                            \n";
    std::cout << "      -xa         Remove the ASCII table in the hex dump output.  \n";
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



void b2Free(void)
{
    // Cleanup capstone
    if (ctx.m_handle != SK_NPOS)
    {
        cs_close(&ctx.m_handle);
        ctx.m_handle = SK_NPOS;
    }

    if (ctx.m_fp)
    {
        delete ctx.m_fp;
        ctx.m_fp = 0;
    }
}

bool b2Alloc(const char* prog)
{
    skBinaryFile* fp;


    // Load the file
    fp = ctx.m_fp = skBinaryFile::load(prog);
    if (!ctx.m_fp)
        return false;

    // used for interactive mode
    ctx.m_fname = prog;


    // initialize the capstone engine
    if (ctx.m_handle != SK_NPOS)
        cs_close(&ctx.m_handle);



    cs_arch arch = CS_ARCH_ALL;
    switch (fp->getArchitecture())
    {
    case IS_SPARC:
        arch = CS_ARCH_SPARC;
        break;
    case IS_MPS:
        arch = CS_ARCH_MIPS;
        break;
    case IS_POWERPC:
        arch = CS_ARCH_PPC;
        break;
    case IS_S390:
        arch = CS_ARCH_SPARC;
        break;
    case IS_AARCH64:
        arch = CS_ARCH_ARM64;
        break;
    case IS_X8664:
    case IS_X86:
        arch = CS_ARCH_X86;
        break;
    default:
        break;
    }



    cs_mode mode = fp->getPlatformType() == FFT_32BIT ? CS_MODE_32 : CS_MODE_64;
    cs_err  err  = cs_open(arch, mode, (csh*)&ctx.m_handle);
    if (err != CS_ERR_OK)
    {
        printf("Capstone Error: cs_open returned %i\n", err);
        return false;
    }

    return true;
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
        if (!b2Alloc(argv[argc - 1]))
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



void b2WriteColor(skConsoleColorSpace cs)
{
    skDebugger::writeColor(cs);
}



void b2WriteHex(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKuint8 c;
    SKsize  j;

    if (!cp || offs == SK_NPOS || max == SK_NPOS)
        return;

    for (j = 0; j < 16; ++j)
    {
        if (j % 8 == 0)
            printf(" ");

        if (offs + j < max)
        {
            c = (SKint32)cp[offs + j];

            b2MarkColor(c, mark);
            printf("%02X ", c);
        }
        else
            printf("   ");
    }
}



void b2WriteAscii(char* cp, SKsize offs, SKsize max, int flags, int mark)
{
    SKuint8 c;
    SKsize  j;

    if (!cp || offs == SK_NPOS || max == SK_NPOS)
        return;


    b2WriteColor(CS_WHITE);
    printf(" |");
    for (j = 0; j < 16; ++j)
    {
        if (offs + j < max)
        {
            c = (SKuint8)cp[offs + j];


            b2MarkColor(c, mark);
            if (c >= 0x20 && c < 0x7F)
                printf("%c", c);
            else
                printf(".");
        }
        else
            printf(" ");
    }

    b2WriteColor(CS_WHITE);
    printf("|");
}



void b2MarkColor(int c, int mark)
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
    printf("%16llx  ", addr);
}


void b2DumpHex(void* ptr, SKuint32 offset, SKuint32 len, int flags, int mark, bool nl)
{
    if (!ptr || offset == SK_NPOS || len == SK_NPOS)
        return;


    SKsize i;
    char*  cp = (char*)ptr;


    for (i = 0; i < len; i += 16)
    {
        if (flags & PF_ADDRESS)
            b2WriteAddress((size_t)(i + offset));
        if (flags & PF_HEX)
            b2WriteHex(cp, i, len, flags, mark);
        if (flags & PF_ASCII)
            b2WriteAscii(cp, i, len, flags, mark);
        if (nl)
            printf("\n");
    }
}



void b2Dissemble(void* ptr, size_t offset, size_t len, int flags)
{
    // filter out invalid input
    if (!ptr || ctx.m_handle == SK_NPOS || offset == SK_NPOS || len == 0 || len == SK_NPOS)
        return;


    cs_insn* insn;
    size_t   count = cs_disasm(ctx.m_handle, (uint8_t*)ptr, len, offset, 0, &insn);
    if (count > 0)
    {
        size_t j;
        for (j = 0; j < count; j++)
        {
            cs_insn& i = insn[j];

            b2WriteColor(CS_LIGHT_GREY);
            b2WriteAddress(i.address);



            int fl = flags & ~(PF_ADDRESS | PF_ASCII);
            b2DumpHex(i.bytes, 0, i.size, fl, ctx.m_code, false);

            b2WriteColor(CS_WHITE);
            printf("%s\t%s\n", i.mnemonic, i.op_str);
        }

        cs_free(insn, count);
    }
}


void b2PrintAll(void)
{
    if (!ctx.m_fp)
        return;

    skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();
    while (it.hasMoreElements())
    {
        skSection* sec = it.getNext().second;

        b2DumpHex(sec->getPointer(),
                  sec->getStartAddress(),
                  sec->getSize(),
                  ctx.m_flags,
                  ctx.m_code);

        printf("%16s*\n", " ");
    }
}


void b2PrintSectionNames(void)
{
    if (!ctx.m_fp)
        return;

    skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();

    b2WriteColor(CS_DARKYELLOW);
    printf("\nSections:\n\n");


    b2WriteColor(CS_GREY);
    printf(" Name                 Offset             Index\n\n");



    b2WriteColor(CS_LIGHT_GREY);
    int i = 0;
    while (it.hasMoreElements())
    {
        skSection* sec = it.getNext().second;

        const skString& str = sec->getName();

        SKuint64 offs = (SKuint64)sec->getStartAddress();
        printf(" %-20s 0x%-16llx %-2u\n", str.c_str(), offs, i);
        ++i;
    }

    if (ctx.m_fp->getFormat() == FF_PE)
    {
        b2WriteColor(CS_DARKYELLOW);
        printf("\nData Directories:\n\n");


        b2WriteColor(CS_GREY);
        it = ctx.m_fp->getSectionIterator();
        printf(" Type                 RVA                Size\n\n");


        b2WriteColor(CS_LIGHT_GREY);
        while (it.hasMoreElements())
        {
            skPortableSection* sec = reinterpret_cast<skPortableSection*>(it.getNext().second);

            skPortableSection::Directories::Iterator dit = sec->getDirectoryIterator();
            while (dit.hasMoreElements())
            {
                skPortableDirectory* dir = dit.getNext();
                printf(" %-20u 0x%-16x %-2u\n", dir->getType(), dir->getRVA(), dir->getSize());
            }
        }
    }

    printf("\n\n");
}



template <typename T>
void b2PrintElfHeader(const skElfHeaderInfo<T>& inf)
{
    char tmpBuf[32];
    skElfPrintUtils::getPlatformId(inf, tmpBuf, 32);
    printf("  Class:                  %s\n", tmpBuf);
    skElfPrintUtils::getByteOrder(inf, tmpBuf, 32);
    printf("  Data:                   %s\n", tmpBuf);
    skElfPrintUtils::getVersion(inf, tmpBuf, 32);
    printf("  Version:                %s\n", tmpBuf);
    skElfPrintUtils::getABI(inf, tmpBuf, 32);
    printf("  OS/ABI:                 %s\n", tmpBuf);
    skElfPrintUtils::getType(inf, tmpBuf, 32);
    printf("  Type:                   %s\n", tmpBuf);
    skElfPrintUtils::getArch(inf, tmpBuf, 32);
    printf("  Architecture:           %s\n", tmpBuf);
    printf("  Entry:                  0x%llx\n", inf.m_entry);
    printf("  Program Offset:         0x%llx\n", inf.m_programOffset);
    printf("  Section Offset:         0x%llx\n", inf.m_sectionOffset);
    printf("  Flags:                  %u\n", (int)inf.m_flags);
    printf("  Program Header Size:    %u\n", (int)inf.m_headerSizeInBytes);
    printf("  Program Header Count:   %u\n", (int)inf.m_headerEntryCount);
    printf("  Section Entry:          %u\n", (int)inf.m_sectionTableEntrySize);
    printf("  Section Count:          %u\n", (int)inf.m_sectionTableEntryCount);
    printf("  Header Table Index:     %u\n", (int)inf.m_sectionTableIndex);
    printf("  sizeof:                 %u\n\n", (SKuint32)sizeof(inf));
}



template <typename T>
static void b2PrintElfSectionHeader(const skElfSectionHeader<T>& sh)
{
    char tmpBuf[32];
    printf("  Name:              %u\n", sh.m_name);
    skElfPrintUtils::getSectionType(sh, tmpBuf, 32);
    printf("  Type:              %s\n", tmpBuf);
    printf("  Flags:             %u\n", (int)sh.m_flags);
    printf("  Virtual Address:   %u\n", (int)sh.m_addr);
    printf("  Offset:            0x%llx\n", sh.m_offset);
    printf("  Size:              %u\n", (int)sh.m_size);
    printf("  Link:              %u\n", (int)sh.m_link);
    printf("  Extra Info:        %u\n", (int)sh.m_info);
    printf("  Alignment:         %u\n", (int)sh.m_addrAlign);
    printf("  Entry Table Size:  %u\n", (int)sh.m_entSize);
    printf("  sizeof:            %u\n", (SKuint32)sizeof(sh));
}



void b2PrintPEHeader(const COFFHeader& header, const COFFOptionalHeaderCommon& optional)
{
    SKuint32 bw;
    char     buf[32];


    skPortableUtils::getMachine(header, buf, 16);
    printf("  Machine:                    %s\n", buf);
    printf("  Section Count:              %u\n", header.m_sectionCount);

    bw = skGetTimeString(buf, 32, "%D %r", header.m_timeDateStamp);
    if (bw != SK_NPOS)
        printf("  Timestamp:                  %s\n", buf);
    else
        printf("  Timestamp:                  %u\n", header.m_timeDateStamp);

    printf("  Symbol Table Offset:        %u\n", header.m_symbolTableOffset);
    printf("  Number Of Symbols:          %u\n", header.m_symbolCount);
    printf("  Optional Header Size:       %u\n", header.m_optionalHeaderSize);
    printf("  Characteristics:            0x%x\n", header.m_characteristics);
    
    printf("\n");

    skPortableUtils::getPlatformId(optional, buf, 16);
    printf("  Magic:                      %s\n", buf);
    printf("  Version:                    %u.%u\n", optional.m_majorVersion, optional.m_minorVersion);
    printf("  Sizeof Code:                %u\n", optional.m_sizeofCode);
    printf("  Sizeof Init Data:           %u\n", optional.m_sizeofInitData);
    printf("  Sizeof BSS Data:            %u\n", optional.m_sizeofBSSData);
    printf("  Entry Point:                0x%X\n", optional.m_entryPoint);
    printf("  Base Of Code:               %u\n", optional.m_baseOfCode);

    printf("\n");
}

void b2PrintPESectionHeader(const COFFSectionHeader& header)
{
    printf("  Name:                       %s\n", header.m_name);
    printf("  Virtual Size:               %u\n", header.m_virtualSize);
    printf("  Virtual Address:            0x%x\n", header.m_virtualAddress);
    printf("  Size of Raw Data:           %u\n", header.m_sizeOfRawData);
    printf("  Pointer To Raw Data:        0x%x\n", header.m_pointerToRawData);
    printf("  Relocation Table:           %u\n", header.m_pointerToRelocations);
    printf("  Relocation Count:           %u\n", header.m_numberOfRelocations);
    printf("  Line Number Location:       0x%x\n", header.m_pointerToLineNumbers);
    printf("  Line Number Count:          %u\n", header.m_numberOfLineNumbers);
    printf("  Characteristics:            0x%x\n", header.m_characteristics);
    printf("  Size of Header:             %u\n", (SKuint32)sizeof(COFFSectionHeader));
}

void b2PrintDataDir(const char* msg, const COFFDataDirectory& dd)
{
    if (dd.m_size)
        printf("%s0x%x,%u\n", msg, dd.m_virtualAddress, dd.m_size);
    else
        printf("%s0\n", msg);
}

template <typename COFFOptionalHeaderVaryingBase, typename SKuintV>
void b2PrintPEVaryingHeader(const COFFOptionalHeader<COFFOptionalHeaderVaryingBase, SKuintV>& header)
{
    printf("  ImageBase:                  0x%llx\n", (SKuint64)header.m_imageBase);
    printf("  Section Alignment:          %u\n", header.m_sectionAlignment);
    printf("  File Alignment:             %u\n", header.m_fileAlignment);
    printf("  Operating System Version:   %u.%u\n", header.m_majOpSysVersion, header.m_minOpSysVersion);
    printf("  Image Version:              %u.%u\n", header.m_majImSysVersion, header.m_minImSysVersion);
    printf("  Sub System Version:         %u.%u\n", header.m_majSubSysVersion, header.m_minSubSysVersion);
    printf("  Image Size:                 0x%x,%u\n", header.m_sizeOfImage, header.m_sizeOfImage);
    printf("  Size of Headers:            %u\n", header.m_sizeOfHeaders);
    printf("  Checksum:                   %u\n", header.m_checkSum);
    printf("  Subsystem:                  %u\n", header.m_subsystem);
    printf("  DLL Characteristics:        0x%llx\n", (SKuint64)header.m_dllCharacteristics);
    printf("  Stack Reserve Size:         %llu\n", (SKuint64)header.m_sizeOfStackReserve);
    printf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfStackCommit);
    printf("  Stack Heap Reserve:         %llu\n", (SKuint64)header.m_sizeOfHeapReserve);
    printf("  Stack Commit Size:          %llu\n", (SKuint64)header.m_sizeOfHeapCommit);
    printf("  Loader Flags:               0x%x\n", header.m_loaderFlags);
    printf("  RVA and Size Count:         %u\n\n", header.m_numberOfRvaAndSizes);


    b2WriteColor(CS_DARKYELLOW);
    printf("Data Directories\n\n");
    b2WriteColor(CS_LIGHT_GREY);


    const COFFDataDirectories& dir = header.m_directories;



    b2PrintDataDir("  Export Table:               ", dir.m_exportTable);
    b2PrintDataDir("  Import Table:               ", dir.m_importTable);
    b2PrintDataDir("  Resource Table:             ", dir.m_resourceTable);
    b2PrintDataDir("  Exception Table:            ", dir.m_exceptionTable);
    b2PrintDataDir("  Certificate Table:          ", dir.m_certificateTable);
    b2PrintDataDir("  Base Relocation Table:      ", dir.m_baseRelocationTable);
    b2PrintDataDir("  Debug Table:                ", dir.m_debug);
    b2PrintDataDir("  Architecture (Reserved):    ", dir.m_architecture);
    b2PrintDataDir("  Global Pointer Register:    ", dir.m_globPtrReg);
    b2PrintDataDir("  Thread Local Storage:       ", dir.m_threadLocalStorage);
    b2PrintDataDir("  Loader Config Table:        ", dir.m_loadConfigTable);
    b2PrintDataDir("  Bound Import Table:         ", dir.m_boundImport);
    b2PrintDataDir("  Import Address Table:       ", dir.m_importAddressTable);
    b2PrintDataDir("  Delay Import Descriptor:    ", dir.m_delayImportDescriptor);
    b2PrintDataDir("  CRT Runtime Header:         ", dir.m_crtRuntimeHeader);
}


void b2PrintPE32Header(const COFFOptionalHeader32& header)
{
    b2PrintPEVaryingHeader<COFFOptionalHeaderCommonPE32, SKuint32>(header);
}


void b2PrintPE64Header(const COFFOptionalHeader64& header)
{
    b2PrintPEVaryingHeader<COFFOptionalHeaderCommonPE64, SKuint64>(header);
}



void b2PrintSectionHeader(skBinaryFile* fp, skSection* section)
{
    if (!section || !fp)
        return;

    skFileFormat format = fp->getFormat();

    switch (fp->getFormat())
    {
    case FF_ELF:
        b2PrintElfSectionHeader(static_cast<skElfSection*>(section)->getHeader());
        break;
    case FF_PE:
        b2PrintPESectionHeader(static_cast<skPortableSection*>(section)->getHeader());
        break;
    }
}



void b2PrintSectionCommon(skSection* section)
{
    if (!section || !ctx.m_fp)
        return;

    b2WriteColor(CS_DARKYELLOW);
    printf("\nSection Header: %s\n\n", section->getName().c_str());


    b2WriteColor(CS_LIGHT_GREY);
    b2PrintSectionHeader(ctx.m_fp, section);
}


void b2PrintSection(skSection* section)
{
    if (!section || !ctx.m_fp)
        return;

    b2PrintSectionCommon(section);
    printf("\n");


    b2WriteColor(CS_WHITE);
    if (ctx.m_flags & PF_DISASEMBLE && section->isExectuable())
    {
        b2Dissemble(section->getPointer(),
                    section->getStartAddress(),
                    section->getSize(),
                    ctx.m_flags);
    }
    else
    {
        b2DumpHex(section->getPointer(),
                  section->getStartAddress(),
                  section->getSize(),
                  ctx.m_flags,
                  ctx.m_code);
    }
}

void b2PrintHeadersCommon(void)
{
    if (!ctx.m_fp)
        return;


    skFileFormat fileFormat = ctx.m_fp->getFormat();

    if (fileFormat == FF_ELF)
    {
        skElfFile* elf = static_cast<skElfFile*>(ctx.m_fp);

        b2WriteColor(CS_DARKYELLOW);
        printf("File Header:\n\n");


        b2WriteColor(CS_LIGHT_GREY);
        b2PrintElfHeader(elf->getHeader());
    }
    else if (fileFormat == FF_PE)
    {
        skPortableFile* pe = static_cast<skPortableFile*>(ctx.m_fp);


        b2WriteColor(CS_DARKYELLOW);
        printf("File Header:\n\n");


        b2WriteColor(CS_LIGHT_GREY);
        b2PrintPEHeader(pe->getHeader(), pe->getCommonHeader());


        // Print the varying header.
        skFileFormatType fpt = pe->getPlatformType();
        if (fpt == FFT_32BIT)
        {
            COFFOptionalHeader32 dest;
            pe->getOptionalHeader(dest);
            b2PrintPE32Header(dest);
        }
        else if (fpt == FFT_64BIT)
        {
            COFFOptionalHeader64 dest;
            pe->getOptionalHeader(dest);
            b2PrintPE64Header(dest);
        }
    }
}


void b2PrintSections(void)
{
    if (!ctx.m_fp)
        return;

    b2PrintHeadersCommon();

    skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();
    while (it.hasMoreElements())
        b2PrintSection(it.getNext().second);
}



void b2PrintSection(const std::string& name)
{
    if (!ctx.m_fp)
        return;

    skSection* sec = ctx.m_fp->getSection(name.c_str());
    if (sec != 0)
        b2PrintSection(sec);
}



void b2PrintSymbols(void)
{
    if (!ctx.m_fp)
        return;

    skBinaryFile::SymbolTable::Iterator it = ctx.m_fp->getSymbolIterator();

    b2WriteColor(CS_DARKYELLOW);
    printf("\nSymbols:\n\n");


    b2WriteColor(CS_GREY);
    if (!it.hasMoreElements())
        printf("No symbols.\n");
    else
    {
        while (it.hasMoreElements())
        {
            skSymbol* sym = it.getNext().second;

            b2WriteColor(ctx.m_fp->getFormat() == FF_ELF ? CS_DARKYELLOW : CS_LIGHT_GREY);
            printf("\t0x%-16llx %s\n", sym->getAddress() , sym->getName().c_str());

            if (ctx.m_fp->getFormat() == FF_ELF)
            {
                skElfSymbol* est = reinterpret_cast<skElfSymbol*>(sym);

                const skElfSymbol64& esym = est->getSymbol();

                b2WriteColor(CS_LIGHT_GREY);
                printf("\t  name   %u\n\t  info   %u\n\t  other  %u\n\t  index  %u\n\t  size   %llu\n",
                       esym.m_name,
                       esym.m_info & 0xF,
                       esym.m_other,
                       esym.m_sectionIdx,
                       esym.m_size);
            }
        }
    }
}


void b2PrintAllHeaders(void)
{
    if (!ctx.m_fp)
        return;

    b2PrintHeadersCommon();

    skBinaryFile::SectionTable::Iterator it = ctx.m_fp->getSectionIterator();
    while (it.hasMoreElements())
        b2PrintSectionCommon(it.getNext().second);
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
    std::cout << "    D. Display Disassembly                            \n";
    std::cout << "    H. Display Hex                                    \n";
    std::cout << "    M. Mark specific code                             \n";
    std::cout << "                                                      \n";
    std::cout << "  File Options:                                       \n";
    std::cout << "    F. Path to file                                   \n";
    std::cout << "                                                      \n";
    std::cout << "                                        Q-Exit.       \n";

    char opt;

    cout << ctx.m_fname << ">";
    cin >> opt;

    switch (opt)
    {
    case 'F':
    case 'f':
    {
        cout << "Path to file: ";
        string path;
        cin >> path;
        b2Free();
        b2Alloc(path.c_str());
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
        cout << "Display ASCII ? y/n\n>";
        char ans;
        cin >> ans;

        if (ans == 'y' || ans == 'Y')
            ctx.m_flags |= PF_ASCII;
        else if (ans == 'n' || ans == 'N')
            ctx.m_flags &= ~PF_ASCII;
    }
    break;
    case 'D':
    case 'd':
    {
        cout << "Display disassembly? y/n\n>";
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
        cout << "Display hex? y/n\n>";
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
        skConsoleClear();
}

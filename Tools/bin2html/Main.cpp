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
#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ELF/skElfFile.h"
#include "ELF/skElfSection.h"
#include "ELF/skElfUtils.h"
#include "PE/skPortableDirectory.h"
#include "PE/skPortableFile.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableUtils.h"
#include "Templates.h"
#include "skBinaryFile.h"



struct b2ProgramInfo
{
    skBinaryFile* m_fp;
    string        m_inputFile;
    string        m_outputFile;
    stringstream  m_stream;
    ofstream      m_ofile;
};

#define Stream gs_ctx.m_stream

int  b2ParseCommandLine(int arcg, char** argv);
void b2Usage(void);
void b2Write(void);



// global static context
static b2ProgramInfo gs_ctx;



int main(int argc, char** argv)
{
    gs_ctx.m_fp         = 0;
    gs_ctx.m_inputFile  = "";
    gs_ctx.m_outputFile = "";


    if (b2ParseCommandLine(argc, argv) == -1)
    {
        b2Usage();

        if (gs_ctx.m_fp)
            delete gs_ctx.m_fp;
        return -1;
    }

    b2Write();

    if (gs_ctx.m_fp)
        delete gs_ctx.m_fp;
    return 0;
}



void b2Usage(void)
{
    std::cout << "bin2html -o <output html file> -i <path to executable>\n";
}


// Parse the command line arguments
// return -1 on failure 0 on success
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
            case 'i':
            {
                ++i;
                if (i < argc && argv[i][0] != '-')
                {
                    gs_ctx.m_inputFile = argv[i];
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
                    gs_ctx.m_outputFile = argv[i];
                }
                else
                {
                    std::cout << ("Missing file argument to 'o'\n");
                }
                break;
            }
            default:
                std::cout << "Unknown argument '" << sw << "'\n";
                return -1;
            }
        }
    }

    if (gs_ctx.m_outputFile.empty())
    {
        std::cout << ("\nNo output file was extracted from the command line\n\n");
        return -1;
    }

    if (!gs_ctx.m_inputFile.empty())
        gs_ctx.m_fp = skBinaryFile::load(gs_ctx.m_inputFile.c_str());
    return 0;
}



// Clear the string stream writer
void b2Clear(void)
{
    gs_ctx.m_stream.str("");
}


// Write the contents of the stream to the file
void b2Flush(void)
{
    if (gs_ctx.m_ofile.is_open())
    {
        gs_ctx.m_ofile << gs_ctx.m_stream.str();

        // clear the stream
        gs_ctx.m_stream.str("");
    }
}



void b2OpenSpan(const string& name)
{
    Stream << "<span class=\"" << name << "\">";
}


void b2CloseSpan(void)
{
    Stream << "</span>";
}



void b2OpenPre(void)
{
    Stream << "<pre>";
}

void b2PreBreak(void)
{
    Stream << "\n";
}



void b2ClosePre(void)
{
    Stream << "</pre>";
}

void b2hHeader(const string& name)
{
    Stream << "<h3 class=\"common_header\">" << name << "</h3>\n";
}



template <typename T>
void b2HeaderRow(const string& name, const T& dt)
{
    stringstream ss;
    ss.imbue(std::locale(""));
    ss << fixed << dec << dt;
    Stream << "<tr><td class=\"l\">" << name << "</td><td class=\"r\">" << ss.str() << "</td></tr>";
}


void b2HeaderRow(const string& name, const string& dt)
{
    Stream << "<tr><td class=\"l\">" << name << "</td><td class=\"r\">" << dt << "</td></tr>";
}

template <typename T>
void b2HeaderRowX(const string& name, const T& val, int w)
{
    Stream << "<tr><td class=\"l\">" << name << "</td><td class=\"r\">"
           << "0x" << std::setw(w) << std::setfill('0') << hex << val << "</td></tr>";
}



void b2HeaderRowX(const string& name, const SKuint32& val)
{
    b2HeaderRowX<SKuint32>(name, val, 8);
}

void b2HeaderRowX(const string& name, const SKuint64& val)
{
    b2HeaderRowX<SKuint64>(name, val, 16);
}

void b2HeaderRowX(const string& name, const SKuint16& val)
{
    b2HeaderRowX<SKuint16>(name, val, 4);
}



void b2HeaderRowT(const string& name, const SKuint32& val)
{
    char buf[32];

    std::strftime(buf, 32, "%D %r", std::localtime((time_t*)&val));
    Stream
        << "<tr><td class=\"l\">" << name << "</td><td class=\"r\">"
        << buf << "</td></tr>";
}



void b2SpanAddress(SKuint64 addr)
{
    SKuint64 iv = addr + (4 << 24);

    b2OpenSpan("address");
    Stream << right << hex << uppercase << setw(16) << setfill(' ') << iv;
    b2CloseSpan();
}



void b2TableOpen(const string& name)
{
    b2hHeader(name);
    Stream << "<table class=\"section_header\" cellpadding=\"0\" cellspacing=\"0\">\n";
    Stream << "<tr>\n";
}


void b2TableClose(void)
{
    Stream << "\n</tr></table>\n";
}



template <typename COFFOptionalHeaderVaryingBase, typename SKuintV>
void b2PEHeader(const COFFOptionalHeader<COFFOptionalHeaderVaryingBase, SKuintV>& header)
{
    b2HeaderRowX("ImageBase:", (SKuint64)header.m_imageBase);
    b2HeaderRow("Section Alignment:", header.m_sectionAlignment);
    b2HeaderRow("File Alignment:", header.m_fileAlignment);
    b2HeaderRow("Operating System Version: ", header.m_majOpSysVersion);
    b2HeaderRow("Image Version:", header.m_majImSysVersion);
    b2HeaderRow("Sub System Version:", header.m_majSubSysVersion);
    b2HeaderRow("Image Size: ", header.m_sizeOfImage);
    b2HeaderRow("Size of Headers:", header.m_sizeOfHeaders);
    b2HeaderRow("Checksum:", header.m_checkSum);
    b2HeaderRow("Subsystem:", header.m_subsystem);
    b2HeaderRowX("DLL Characteristics:", (SKuint64)header.m_dllCharacteristics);
    b2HeaderRow("Stack Reserve Size: ", (SKuint64)header.m_sizeOfStackReserve);
    b2HeaderRow("Stack Commit Size: ", (SKuint64)header.m_sizeOfStackCommit);
    b2HeaderRow("Stack Heap Reserve: ", (SKuint64)header.m_sizeOfHeapReserve);
    b2HeaderRow("Stack Commit Size:", (SKuint64)header.m_sizeOfHeapCommit);
    b2HeaderRowX("Loader Flags:", header.m_loaderFlags);
    b2HeaderRow("RVA and Size Count:", header.m_numberOfRvaAndSizes);
}



void b2FileHeader(void)
{
    skBinaryFile* bin = gs_ctx.m_fp;
    if (bin)
    {
        b2Clear();


        b2TableOpen("File Header" );
        skFileFormat fileFormat = bin->getFormat();

        if (fileFormat == FF_ELF)
        {
            skElfFile*               elf    = static_cast<skElfFile*>(bin);
            const skElfHeaderInfo64& header = elf->getHeader();


            char tmpBuf[32];
            skElfUtils::getPlatformId(header, tmpBuf, 32);
            b2HeaderRow("Class", tmpBuf);
            skElfUtils::getByteOrder(header, tmpBuf, 32);
            b2HeaderRow("Data", tmpBuf);
            skElfUtils::getVersion(header, tmpBuf, 32);
            b2HeaderRow("Version", tmpBuf);
            skElfUtils::getABI(header, tmpBuf, 32);
            b2HeaderRow("OS/ABI", tmpBuf);
            skElfUtils::getType(header, tmpBuf, 32);
            b2HeaderRow("Type", tmpBuf);
            skElfUtils::getArch(header, tmpBuf, 32);
            b2HeaderRow("Architecture", tmpBuf);
            b2HeaderRow("Entry", header.m_entry);
            b2HeaderRow("Program Offset", header.m_programOffset);
            b2HeaderRow("Section Offset", header.m_sectionOffset);
            b2HeaderRow("Flags", (int)header.m_flags);
            b2HeaderRow("Program Header Size", (int)header.m_headerSizeInBytes);
            b2HeaderRow("Program Header Count", (int)header.m_headerEntryCount);
            b2HeaderRow("Section Entry", (int)header.m_sectionTableEntrySize);
            b2HeaderRow("Section Count", (int)header.m_sectionTableEntryCount);
            b2HeaderRow("Header Table Index", (int)header.m_sectionTableIndex);
            b2HeaderRow("sizeof", (SKuint32)sizeof(header));
        }
        else if (fileFormat == FF_PE)
        {
            skPortableFile* pe = static_cast<skPortableFile*>(bin);


            const COFFHeader& header = pe->getCommonHeader();

            b2HeaderRow("Machine:", header.m_machine);
            b2HeaderRow("Section Count:", header.m_sectionCount);
            b2HeaderRowT("Timestamp:", header.m_timeDateStamp);

            b2HeaderRowX("SymbolTableOffset:", header.m_symbolTableOffset);
            b2HeaderRowX("NumberOfSymbols:", header.m_symbolCount);
            b2HeaderRowX("OptionalHeaderSize:", header.m_optionalHeaderSize);
            b2HeaderRowX("Characteristics:", header.m_characteristics);

            // Print the varying header.
            skFileFormatType fpt = pe->getPlatformType();
            if (fpt == FFT_32BIT)
            {
                COFFOptionalHeader32 header;
                pe->getOptionalHeader(header);
                b2PEHeader<COFFOptionalHeaderCommonPE32, SKuint32>(header);
            }
            else if (fpt == FFT_64BIT)
            {
                COFFOptionalHeader64 header;
                pe->getOptionalHeader(header);
                b2PEHeader<COFFOptionalHeaderCommonPE64, SKuint64>(header);
            }
        }

        b2TableClose();
        b2Flush();
    }
}


void b2WriteHexLine(SKuint8* ptr, SKuint64 i, SKuint64 size, SKuint64 perline)
{
    SKuint64 j, mid = perline / 2;
    for (j = 0; j < perline; j++)
    {
        if (j % mid == 0)
            Stream << " ";

        if (i + j < size)
        {
            int ch = ptr[i + j];
            if (ch == 0x00 || ch == 0xCC)
                b2OpenSpan("hex0");
            else
                b2OpenSpan("hex1");
            Stream
                << hex << uppercase << setfill('0') << setw(2)
                << ch;
            b2CloseSpan();
        }
        else
            Stream << "  ";

        Stream << " ";
    }
}



void b2WriteAsciiLine(SKuint8* ptr, SKuint64 i, SKuint64 size, SKuint64 perline)
{
    SKuint64 j;
    b2OpenSpan("ascii");
    Stream << "|";

    for (j = 0; j < perline; j++)
    {
        if (i + j < size)
        {
            int ch = ptr[i + j];
            if (ch == 0x00 || ch == 0xCC)
                b2OpenSpan("ascii0");
            else
                b2OpenSpan("ascii1");

            if (ch >= 0x20 && ch < 0x7F)
                Stream << (char)ch;
            else
                Stream << '.';

            b2CloseSpan();
        }
        else
        {
            Stream << ".";
        }
    }
    Stream << "|";
    b2CloseSpan();
}



void b2WriteHex(SKuint8* ptr, SKuint64 start, SKuint64 stop)
{
    skBinaryFile* bin = gs_ctx.m_fp;
    if (bin && ptr)
    {
        SKuint64 i, b = 16;
        b2OpenPre();

        for (i = 0; i < stop; i += b)
        {
            b2SpanAddress(i + start);
            b2WriteHexLine(ptr, i, stop, b);
            b2WriteAsciiLine(ptr, i, stop, b);

            b2PreBreak();
        }
        b2ClosePre();
    }
}



// Write out all sections
void b2Sections(void)
{
    skBinaryFile* bin = gs_ctx.m_fp;
    if (bin)
    {
        skFileFormat fmt = bin->getFormat();

        b2Clear();
        skBinaryFile::SectionTable::Iterator it = bin->getSectionIterator();

        while (it.hasMoreElements())
        {
            const skBinaryFile::SectionTable::Pair& pair = it.getNext();

            if (fmt == FF_PE)
            {
                skPortableSection* pes = reinterpret_cast<skPortableSection*>(pair.second);

                const COFFSectionHeader& csh = pes->getHeader();
                b2TableOpen(string("Section: ") + pair.first.c_str());

                b2HeaderRow("Virtual Size", csh.m_virtualSize);
                b2HeaderRowX("Virtual Address", csh.m_virtualAddress);
                b2HeaderRow("Size of Raw Data", csh.m_sizeOfRawData);
                b2HeaderRowX("Pointer To Raw Data", csh.m_pointerToRawData);
                b2HeaderRowX("Relocation Table", csh.m_pointerToRelocations);
                b2HeaderRow("Relocation Count", csh.m_numberOfRelocations);
                b2HeaderRowX("Line Number Location", csh.m_pointerToLineNumbers);
                b2HeaderRow("Line Number Count", csh.m_numberOfLineNumbers);
                b2HeaderRowX("Characteristics", csh.m_characteristics);
                b2HeaderRow("Size of Header", (SKuint32)sizeof(COFFSectionHeader));
                b2TableClose();


                if (pes->isExectuable())
                {
                }
                else
                {
                    b2WriteHex(
                        pes->getPointer(),
                        pes->getStartAddress(),
                        pes->getSize());
                }

            }
            else if (fmt == FF_ELF)
            {
                skElfSection* elfs = reinterpret_cast<skElfSection*>(pair.second);

                const skElfSectionHeader64& esh = elfs->getHeader();
                b2TableOpen(string("Section: ") + pair.first.c_str());


                char tmpBuf[32];
                b2HeaderRow("Name", esh.m_name);

                skElfUtils::getSectionType(esh, tmpBuf, 32);
                b2HeaderRow("Type", string(tmpBuf));

                b2HeaderRow("Flags", esh.m_flags);
                b2HeaderRowX("Virtual Address", esh.m_addr);
                b2HeaderRowX("Offset", esh.m_offset);
                b2HeaderRow("Size", esh.m_size);
                b2HeaderRow("Link", esh.m_link);
                b2HeaderRow("Extra Info", esh.m_info);
                b2HeaderRow("Alignment", esh.m_addrAlign);
                b2HeaderRow("Entry Table Size", esh.m_entSize);
                b2HeaderRow("Size Of", sizeof(esh));

                b2TableClose();


                if (elfs->isExectuable())
                {

                }
                else
                {
                    b2WriteHex(
                        elfs->getPointer(),
                        elfs->getStartAddress(),
                        elfs->getSize());
                }

            }
        }



        b2Flush();
    }
}



void b2Write(void)
{
    gs_ctx.m_ofile.open(gs_ctx.m_outputFile, std::ios::binary | std::ios::out | std::ios::trunc);

    if (!gs_ctx.m_ofile.is_open())
    {
        cout << "Failed to open output file.\n";
        return;
    }

    gs_ctx.m_ofile.write(HEADER, HEADER_SIZE);

    b2FileHeader();
    b2Sections();


    b2Flush();

    gs_ctx.m_ofile.write(FOOTER, FOOTER_SIZE);
    gs_ctx.m_ofile.close();
}

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
#include "ELF/skElfFile.h"
#include <stdio.h>
#include <string.h>
#include "ELF/skElfSection.h"
#include "ELF/skElfSymbol.h"
#include "ELF/skElfUtils.h"
#include "Utils/skFileStream.h"
#include "Utils/skString.h"
#include "skSection.h"


skElfFile::skElfFile(int flags) :
    skBinaryFile(flags),
    m_strtab(0)
{
    // Make sure this type of instance is visible in the base class.
    m_fileFormat = FF_ELF;

    skMemset(&m_header, 0, sizeof(skElfHeaderInfo64));
}

skElfFile::~skElfFile()
{
}


int skElfFile::loadImpl(skStream& stream)
{
    int     errorCode;
    SKuint8 desc;


    stream.seek(EMN_CLASS, SEEK_SET);
    stream.read(&desc, 1);
    stream.seek(0, SEEK_SET);

    if (desc == 1)
        m_fileFormatType = FFT_32BIT;
    else if (desc == 2)
        m_fileFormatType = FFT_64BIT;
    else
    {
        // Unknown class descriptor found in the file header
        return EC_UNKNOWN_FILE_FORMAT;
    }


    if (m_fileFormatType == FFT_32BIT)
    {
        // Read as 32 bit then store as a 64bit type
        skElfHeaderInfo32 header;
        stream.read(&header, sizeof(skElfHeaderInfo32));
        skElfUtils::copyHeader(m_header, header);
    }
    else
    {
        stream.read(&m_header, sizeof(skElfHeaderInfo64));
    }


    switch (m_header.m_machine)
    {
    case EIA_SPARC:
        m_arch = IS_SPARC;
        break;
    case EIA_X86:
        m_arch = IS_X86;
        break;
    case EIA_MPS:
        m_arch = IS_MPS;
        break;
    case EIA_POWERPC:
        m_arch = IS_POWERPC;
        break;
    case EIA_S390:
        m_arch = IS_S390;
        break;
    case EIA_SUPERH:
        m_arch = IS_SUPERH;
        break;
    case EIA_IA64:
        m_arch = IS_IA64;
        break;
    case EIA_X8664:
        m_arch = IS_X8664;
        break;
    case EIA_AARCH64:
        m_arch = IS_AARCH64;
        break;
    case EIA_RISCV:
        m_arch = IS_RISCV;
        break;
    case EIA_NONE:
    default:
        m_arch = IS_NONE;
        break;
    }

    if (m_arch == IS_NONE)
    {
        if (m_logFlags != LF_NONE)
            printf("Unknown machine architecture found in the file header\n");
        return EC_UNKNOWN_FILE_FORMAT;
    }

    if (m_fileFormatType == FFT_32BIT)
    {
        errorCode = loadSections<skElfSectionHeader32>(stream);
        if (errorCode != EC_OK)
            return errorCode;

        errorCode = loadSymbolTable<skElfSymbol32>(".dynstr", ".dynsym");
        if (errorCode != EC_OK)
            return errorCode;
        errorCode = loadSymbolTable<skElfSymbol32>(".strtab", ".symtab");
        if (errorCode != EC_OK)
            return errorCode;
    }
    else
    {
        errorCode = loadSections<skElfSectionHeader64>(stream);
        if (errorCode != EC_OK)
            return errorCode;

        errorCode = loadSymbolTable<skElfSymbol64>(".dynstr", ".dynsym");
        if (errorCode != EC_OK)
            return errorCode;
        errorCode = loadSymbolTable<skElfSymbol64>(".strtab", ".symtab");
        if (errorCode != EC_OK)
            return errorCode;
    }
    return EC_OK;
}


template <typename skElfSectionHeader>
int skElfFile::loadSections(skStream& stream)
{
    elf64 offs, offe, i;

    // The section headers are packed in m_data[start:end]
    offs = getSectionHeaderStart();
    offe = getSectionHeaderEnd();

    if (offe > m_len)
    {
        if (m_logFlags != LF_NONE)
            printf("Error - The section header's offset exceeds the amount of allocated memory.\n");
        return EC_OVERFLOW;
    }

    // Read the last section for resolving names.
    // TODO: Fix this. It shouldn't assume that the string table is last.
    stream.seek(offe - sizeof(skElfSectionHeader), SEEK_SET);
    skElfSectionHeader strtab;
    stream.read(&strtab, sizeof(skElfSectionHeader));
    m_strtab = strtab.m_offset;


    stream.seek(offs, SEEK_SET);
    for (i = 0; i < m_header.m_sectionTableEntryCount; ++i)
    {
        skElfSectionHeader sp;
        stream.read(&sp, sizeof(skElfSectionHeader));


        const SKsize sn = (SKsize)m_strtab + sp.m_name;
        if (sn < m_len)
        {
            SKsize loc = stream.position();
            char   tempBuf[64];
            stream.seek(sn, SEEK_SET);
            stream.read(tempBuf, 64);
            stream.seek(loc, SEEK_SET);

            char* name = (char*)tempBuf;

            // Skip the null entry
            if (name != 0 && (*name) == '\0')
                continue;

            if (m_sectionLookup.find(name) == m_sectionLookup.npos)
            {
                if (sp.m_size < m_len)
                {
                    skElfSectionHeader64 header;
                    skElfUtils::copyHeader(header, sp);

                    loc = stream.position();
                    stream.seek(sp.m_offset, SEEK_SET);

                    SKuint8* data = new SKuint8[(size_t)sp.m_size + 1];
                    stream.read(data, (size_t)sp.m_size);
                    data[sp.m_size] = 0;
                    stream.seek(loc, SEEK_SET);


                    skSection* section = new skElfSection(
                        this,
                        name,
                        data,
                        (SKsize)sp.m_size,
                        (size_t)sp.m_offset,
                        header);

                    if (!m_sectionLookup.insert(name, section))
                    {
                        if (m_logFlags != LF_NONE)
                            printf("Failed to store the section '%s'\n", name);
                    }
                }
                else
                {
                    if (m_logFlags != LF_NONE)
                    {
                        SKuint64 len = (SKuint64)sp.m_offset + (SKuint64)sp.m_size;
                        printf("Error - Section size exceeds the amount of allocated memory (%s, %llu).\n", name, len - m_len);
                    }
                    return EC_OVERFLOW;
                }
            }
            else
            {
                if (m_logFlags != LF_NONE)
                    printf("Error - duplicate section name!\n");
                return EC_DUPLICATE;
            }
        }
        else
            return EC_OVERFLOW;
    }
    return EC_OK;
}


template <typename skElfSymbolHeader>
int skElfFile::loadSymbolTable(const char* strLookup, const char* symLookup)
{
    SK_ASSERT(strLookup && symLookup);  // callers fault
    SKuint64 i;

    skElfSection* str = reinterpret_cast<skElfSection*>(getSection(strLookup));
    skElfSection* sym = reinterpret_cast<skElfSection*>(getSection(symLookup));

    if (sym && str)
    {
        const skElfSectionHeader64& hdr = sym->getHeader();

        skElfSymbolHeader* symPtr = (skElfSymbolHeader*)sym->getPointer();
        SKint8*            strPtr = (SKint8*)str->getPointer();

        if (hdr.m_entSize == 0)
        {
            if (m_logFlags != LF_NONE)
                printf("Error - No entries in the string table.\n");
            return EC_UNEXPECTED;
        }

        const SKuint64 entryLen = hdr.m_size / hdr.m_entSize;
        i = 0;
        while (i < entryLen)
        {
            const skElfSymbolHeader& syml = (*symPtr);

            if (syml.m_name >= str->getSize())
            {
                if (m_logFlags != LF_NONE)
                    printf("Error - The size of the symbol name exceeds the size of the string table.\n");
                return EC_OVERFLOW;
            }

            char* cp = (char*)(strPtr + syml.m_name);
            if (*cp == '\0')
                ++cp;

            skString strl = cp;
            if (!strl.empty())
            {
                const SKsize idx = m_symTable.find(strl);
                if (idx == m_symTable.npos)
                {
                    skElfSymbol64 sdp;
                    if (m_fileFormatType == FFT_32BIT)
                        skElfUtils::copyHeader(sdp, (*(skElfSymbol32*)symPtr));
                    else
                        skElfUtils::copyHeader(sdp, (*(skElfSymbol64*)symPtr));

                    skSymbol* elfSym = new skElfSymbol(this, strl, sdp);
                    m_symTable.insert(strl, elfSym);
                }
                else if (m_logFlags == LF_VERBOSE)
                    printf("Failed to locate the symbol '%s'\n", strl.c_str());
            }
            else
            {
                if (m_logFlags != LF_NONE)
                    printf("Empty symbol name found while parsing symbol table\n");
                return EC_UNEXPECTED;
            }
            ++symPtr;
            i++;
        }
    }
    return EC_OK;
}

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
#include "PE/skPortableFile.h"
#include <memory.h>
#include "PE/skPortableDirectory.h"
#include "PE/skPortableSection.h"
#include "PE/skPortableSymbol.h"
#include "Utils/skDebugger.h"
#include "Utils/skFileStream.h"
#include "Utils/skMemoryUtils.h"
#include "skSection.h"

const COFFOptionalHeaderCommon pfDefaultValue = {0, 0, 0, 0, 0, 0, 0, 0};
const COFFHeader               cHeader        = {0, 0, 0, 0, 0, 0, 0};



skPortableFile::skPortableFile(int flags, SKint16 dos_offset) :
    skBinaryFile(flags),
    m_header(cHeader),
    m_imageHeader(0),
    m_headerOffs(dos_offset),
    m_sectionStart(0)
{
    m_fileFormat = FF_PE;
}

skPortableFile ::~skPortableFile()
{
    delete m_imageHeader;
}


const COFFOptionalHeaderCommon &skPortableFile::getCommonHeader(void) const
{
    if (m_imageHeader)
        return (*m_imageHeader);
    return pfDefaultValue;
}

void skPortableFile::getOptionalHeader(COFFOptionalHeader32 &dest) const
{
    // The file format type is the only indicator for determining
    // how m_imageHeader can be cast to the correct value.
    if (m_imageHeader && m_fileFormatType == FFT_32BIT)
    {
        COFFOptionalHeader32 *h32 = (COFFOptionalHeader32 *)m_imageHeader;

        skMemcpy(&dest, h32, sizeof(COFFOptionalHeader32));
    }
    else
    {
        skMemset(&dest, 0, sizeof(COFFOptionalHeader32));
    }
}

void skPortableFile::getOptionalHeader(COFFOptionalHeader64 &dest) const
{
    // The file format type is the only indicator for determining
    // how m_imageHeader can be cast to the correct value.
    if (m_imageHeader && m_fileFormatType == FFT_64BIT)
    {
        COFFOptionalHeader64 *h64 = (COFFOptionalHeader64 *)m_imageHeader;
        skMemcpy(&dest, h64, sizeof(COFFOptionalHeader64));
    }
    else
    {
        skMemset(&dest, 0, sizeof(COFFOptionalHeader64));
    }
}

int skPortableFile::loadImpl(skStream &stream)
{
    SKuint16 optMagic, code = EC_OK;

    // Skip past the DOS stub program, and the PE signature
    // since it is not part of the defined structure (+4)
    stream.seek((SKint64)m_headerOffs + 4, SEEK_CUR);
    stream.read(&m_header, sizeof(COFFHeader));


    switch (m_header.m_machine)
    {
    case CMT_AMD64:
    case CMT_IA64:
    case CMT_I386:
        m_arch = IS_X86;
        break;
    default:
        break;
    }

    if (m_arch == IS_NONE)
    {
        if (m_logFlags != LF_NONE)
            printf("Invalid instruction set or conversion is not setup for:(0x%04x)\n", m_header.m_machine);
        return EC_UNKNOWN_FILE_FORMAT;
    }

    stream.read(&optMagic, 2);
    stream.seek(-2, SEEK_CUR);

    if (optMagic == COFF_MAG_PE32)
    {
        if (sizeof(COFFOptionalHeader32) != m_header.m_optionalHeaderSize)
        {
            if (m_logFlags != LF_NONE)
            {
                printf("COFFOptionalHeader32 not properly aligned %u-%u\n",
                       (SKuint32)sizeof(COFFOptionalHeader32),
                       m_header.m_optionalHeaderSize);
            }
            return EC_UNEXPECTED;
        }

        m_imageHeader = new COFFOptionalHeader32;
        stream.read(m_imageHeader, sizeof(COFFOptionalHeader32));

        m_sectionStart = 4 + (SKuint64)m_headerOffs + sizeof(COFFHeader) + sizeof(COFFOptionalHeader32);
    }
    else if (optMagic == COFF_MAG_PE64)
    {
        if (sizeof(COFFOptionalHeader64) != m_header.m_optionalHeaderSize)
        {
            if (m_logFlags != LF_NONE)
            {
                printf("COFFOptionalHeader64 not properly aligned %u-%u\n",
                         (SKuint32)sizeof(COFFOptionalHeader64),
                         m_header.m_optionalHeaderSize);
            }
            return EC_UNEXPECTED;
        }

        m_imageHeader = new COFFOptionalHeader64;
        stream.read(m_imageHeader, sizeof(COFFOptionalHeader64));
        m_sectionStart = 4 + (SKuint64)m_headerOffs + sizeof(COFFHeader) + sizeof(COFFOptionalHeader64);
    }
    else
    {
        if (m_logFlags != LF_NONE)
            printf("COFFOptionalHeader: Unknown header type!\n");
        return EC_UNKNOWN_FILE_FORMAT;
    }

    // Set the file format type now that it's known to be one or the other.
    m_fileFormatType = optMagic == COFF_MAG_PE32 ? FFT_32BIT : FFT_64BIT;

    stream.seek(m_sectionStart, SEEK_SET);

    SKuint16 i16;
    for (i16 = 0; i16 < m_header.m_sectionCount; ++i16)
    {
        COFFSectionHeader sh = {};
        stream.read(&sh, sizeof(COFFSectionHeader));

        char *name = reinterpret_cast<char *>(sh.m_name);
        if ((*name) == '\0' || name[7] != '\0')
            continue;

        if (m_sectionLookup.find(name) == m_sectionLookup.npos)
        {
            size_t size = sh.m_virtualSize;
            if (sh.m_virtualSize > sh.m_sizeOfRawData)
            {
                // The section data is zero padded.
                size = sh.m_sizeOfRawData;
            }

            if (size < m_len)
            {
                // Read the section data

                // save the location
                const SKsize pos = stream.position();
                stream.seek(sh.m_pointerToRawData, SEEK_SET);

                SKuint8 *data = new SKuint8[size + 1];
                stream.read(data, size);
                stream.seek(pos, SEEK_SET);
                data[size] = 0;

                skSection *section = new skPortableSection(
                    this,
                    name,
                    data,
                    (SKsize)size,
                    sh.m_pointerToRawData,
                    sh);

                m_sectionLookup.insert(name, section);
            }
            else
            {
                if (m_logFlags != LF_NONE)
                    printf("Error - Section size exceeds the amount of memory allocated.\n");
                return EC_OVERFLOW;
            }
        }
        else
        {
            if (m_logFlags != LF_NONE)
                printf("Error - duplicate symbol name!\n");
            return EC_UNEXPECTED;
        }
    }


    if (m_fileFormatType == FFT_32BIT)
    {
        sortDataDirectories<COFFOptionalHeader32>();
    }
    else if (m_fileFormatType == FFT_64BIT)
    {
        sortDataDirectories<COFFOptionalHeader64>();
    }



    SectionTable::Iterator it = m_sectionLookup.iterator();
    while (it.hasMoreElements())
    {
        skPortableSection *pes = reinterpret_cast<skPortableSection *>(it.getNext().second);

        code = EC_OK;

        skPortableSection::Directories::Iterator dit = pes->getDirectoryIterator();
        while (dit.hasMoreElements() && code == EC_OK)
        {
            skPortableDirectory *dir = dit.getNext();
            switch (dir->getType())
            {
            case CDE_RESOURCE:
                code = loadResourceDirectory(pes, dir);
                break;
            case CDE_IMPORT:
                code = loadImportDirectory(pes, dir);
                break;
            case CDE_IMPORT_ADDRESS_TABLE:
            case CDE_EXPORT:
            case CDE_EXCEPTION:
            case CDE_CERTIFICATE:
            case CDE_BASE_RELOCATION:
            case CDE_DEBUG:
            case CDE_ARCHITECTURE:
            case CDE_GLOBAL_PTR:
            case CDE_THREAD_LOCAL_STORAGE:
            case CDE_LOAD_CONFIG:
            case CDE_BOUND_IMPORT:
            case CDE_DELAY_IMPORT_DESC:
            case CDE_CRT_RUNTIME_HEADER:
            default:
                break;
            }
        }
    }
    return code;
}



int skPortableFile::loadResourceDirectory(skPortableSection *section, skPortableDirectory *resource)
{
    return EC_OK;
}



int skPortableFile::loadImportDirectory(skPortableSection *section, skPortableDirectory *directory)
{
    // resolve the relative virtual address.
    const SKuint32 addr = directory->getAddress();
    if (addr == (SKuint32)-1)
    {
        // meaning the directory has no owner.
        return EC_UNEXPECTED;
    }

    // Grab the initial pointer with the
    // offset to the directory table
    SKbyte *ptr = (SKbyte *)section->getPointer() + addr;



    // Cast the pointer to the import directory structure.
    COFFImportDirectoryTable *idata = reinterpret_cast<COFFImportDirectoryTable *>(ptr);

    const SKuint32 ival   = sizeof(COFFImportDirectoryTable);
    const SKsize   maxl   = section->getSize();
    const SKuint32 len    = directory->getSize();
    const SKsize   ubound = maxl - addr;

    SKuint32 i = 0;
    while (i < len)
    {
        const COFFImportDirectoryTable &cidt = (*idata);
        if (cidt.m_nameRVA == 0)
            break;

        char *         dllName   = 0;
        const SKuint32 addrOfDLL = cidt.m_nameRVA - directory->getRVA();
        if (addrOfDLL < maxl)
            dllName = (char *)ptr + addrOfDLL;


        SKuint32 addrOfHint = cidt.m_nameHintRVA, va;

        for (; addrOfHint; addrOfHint += 4)  // += 8 for PE32+
        {
            va = addrOfHint - directory->getRVA();
            if (va > ubound)
                break;

            // TODO: this needs to be SKuint64 for PE32+
            const SKuint32 ilt = *(SKuint32 *)(ptr + va);
            if (ilt == 0)
                break;

            // this needs to be (1 << 63) for PE32+
            if (ilt & (1 << 31))
            {
                // lookup by ordinal
                printf("Lookup by ordinal is not implemented\n");
                break;
            }
            else  // look up by name
            {
                const SKuint32 hint = ilt - directory->getRVA();
                if (hint == SK_NPOS32)
                    continue;

                SKuint16 *sp = (SKuint16 *)(ptr + hint);

                const SKuint16 exnt = *(sp++);
                SKbyte *       cp   = (SKbyte *)(sp);

                skString name = cp;
                if (!name.empty())
                {
                    const SKsize idx = m_symTable.find(name);
                    if (idx == SK_NPOS32)
                    {
                        skSymbol *sym = new skPortableSymbol(
                            this,
                            name,
                            dllName,
                            exnt);
                        m_symTable.insert(name, sym);
                    }
                }
            }
        }
        i += ival;
        ++idata;
    }
    return EC_OK;
}



template <typename COFFOptionalHeaderVaryingBase>
void skPortableFile::sortDataDirectories(void)
{
    // Find the locations of the data directories
    COFFOptionalHeaderVaryingBase *hdr = reinterpret_cast<COFFOptionalHeaderVaryingBase *>(m_imageHeader);

    int                    i;
    SectionTable::Iterator it = m_sectionLookup.iterator();
    while (it.hasMoreElements())
    {
        skPortableSection *      pes = reinterpret_cast<skPortableSection *>(it.getNext().second);
        const COFFSectionHeader &csh = pes->getHeader();

        for (i = 0; i < CDE_MAX; ++i)
        {
            COFFDataDirectory &dd = hdr->m_directories[i];

            if (dd.m_virtualAddress >= csh.m_virtualAddress &&
                dd.m_virtualAddress < csh.m_virtualAddress + csh.m_sizeOfRawData)
            {
                pes->_addDirectory((COFFDirectoryEnum)i, dd);
            }
        }
    }
}

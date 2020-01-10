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
#include <stdio.h>
#include <string.h>
#include <limits>
#include "ELF/skElfFile.h"
#include "PE/skPortableFile.h"
#include "Utils/skFileStream.h"
#include "skSection.h"
#include "skSymbol.h"


int skBinaryFile::load(const char *file, skBinaryFile **fp, int flags)
{
    skFileStream fs;
    char         magic[5];

    int status = EC_OK;
    if (!file || !(*file) || !fp)
    {
        if (flags != LF_NONE)
            printf("Invalid parameters supplied to load\n");
        status = EC_INVALID_POINTER;
    }
    else
    {
        fs.open(file, skStream::READ);
        if (fs.isOpen())
        {
            fs.read(magic, 4);
            (*fp) = 0;

            magic[4] = 0;
            fs.seek(0, SEEK_SET);

            if (strncmp("\177ELF", magic, 4) == 0)
                (*fp) = new skElfFile(flags);
            else if (strncmp("MZ", magic, 2) == 0)
            {
                // Seeks to the 4 byte variable containing the
                // offset to the PE signature
                fs.seek(0x3C, SEEK_SET);

                SKuint32 pe_offset;
                fs.read(&pe_offset, sizeof(SKuint32));

                if (pe_offset > 0 && pe_offset < std::numeric_limits<SKuint32>::max())
                {
                    fs.seek(pe_offset, SEEK_SET);
                    fs.read(magic, 4);
                    fs.seek(0, SEEK_SET);

                    if (strncmp("PE\0\0", magic, 4) == 0)
                        (*fp) = new skPortableFile(flags, pe_offset);
                    else
                    {
                        if (flags != LF_NONE)
                            printf("PE signature was not found.\n");
                        status = EC_UNEXPECTED;
                    }
                }
                else
                {
                    // Or perhaps a DOS program was passed in.
                    // This is looking for PE only right now.
                    if (flags != LF_NONE)
                        printf("Invalid PE offset.\n");
                    status = EC_UNEXPECTED;
                }
            }
            else
                status = EC_FILE_LOADING_FAILED;

            if (status == EC_OK)
            {
                if ((*fp))
                    status = (*fp)->load(fs);
            }
        }
        else
        {
            if (flags != LF_NONE)
                printf("Failed to load the file: %s\n", file);
            status = EC_FILE_LOADING_FAILED;
        }
    }
    return status;
}



skBinaryFile::skBinaryFile(int flags) :
    m_len(0),
    m_fileFormat(FF_UNKNOWN),
    m_fileFormatType(FFT_UNKNOWN),
    m_arch(IS_NONE),
    m_logFlags(flags)
{
}


skBinaryFile::~skBinaryFile()
{
    SectionTable::Iterator it = m_sectionLookup.iterator();
    while (it.hasMoreElements())
        delete it.getNext().second;

    SymbolTable::Iterator sym_it = m_symTable.iterator();
    while (sym_it.hasMoreElements())
        delete sym_it.getNext().second;
}


int skBinaryFile::load(skStream &fstream)
{
    m_len = fstream.size();
    if (m_len == 0 || m_len == SK_NPOS)
        return EC_FILE_LOADING_FAILED;
    return loadImpl(fstream);
}

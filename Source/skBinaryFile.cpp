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


skBinaryFile *skBinaryFile::load(const char *file)
{
    if (!file || !(*file))
        return 0;

    skFileStream fs;
    fs.open(file, skStream::READ);

    if (!fs.isOpen())
    {
        printf("failed to open file %s\n", file);
        return 0;
    }


    char magic[4];
    fs.read(magic, 4);

    skBinaryFile *rval = 0;

    // rewind
    fs.seek(0, SEEK_SET);

    if (strncmp("\177ELF", magic, 4) == 0)
        rval = new skElfFile();
    else if (strncmp("MZ", magic, 2) == 0)
    {
        // Seek to the 4 byte variable containing the
        // offset to the PE signature
        fs.seek(0x3C, SEEK_SET);

        SKuint32 pe_offset;
        fs.read(&pe_offset, sizeof(SKuint32));

        if (pe_offset > 0 && pe_offset < std::numeric_limits<SKuint32>::max())
        {
            fs.seek(pe_offset, SEEK_SET);
            fs.read(magic, 4);

            // Seek back to the start of the MZ signature
            fs.seek(0, SEEK_SET);

            if (strncmp("PE\0\0", magic, 4) == 0)
                rval = new skPortableFile(pe_offset);
            else
            {
                // defaults to the generic skDefaultFile.
                printf("skBinaryFile::load: - PE signature was not found.\n");
            }
        }
        else
        {
            // Or perhaps a DOS program was passed in.
            // This is looking for PE only right now.
            printf("Invalid PE offset\n");


            // Seek back to the start of the file and use 
            // skDefaultFile, so at least something will be print to screen. 
            fs.seek(0, SEEK_SET);
        }
    }

    if (rval)
        rval->load(fs);
    return rval;
}



skBinaryFile::skBinaryFile():
    m_len(0),
    m_fileFormat(FF_UNKNOWN),
    m_fileFormatType(FFT_UNKNOWN),
    m_arch(IS_NONE)

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


void skBinaryFile::load(skStream &fstream)
{
    if (!fstream.isOpen())
    {
        printf("load called on a closed file\n");
        return;
    }

    m_len  = fstream.size();
    loadImpl(fstream);
}

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
#
#include "skBinaryFile.h"
#if SK_PLATFORM == SK_PLATFORM_WIN32
#include <windows.h>
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "Utils/skDebugger.h"
#include "Utils/skFileStream.h"
#include "skElf.h"
#include "skPrintUtils.h"



skBinaryFile *skBinaryFile::createInstance(const char *file)
{
    if (!file || !(*file))
        return 0;


    skFileStream fs;
    fs.open(file, skStream::READ);

    if (!fs.isOpen())
    {
        skPrintf("failed to open file %s\n", file);
        return 0;
    }


    char magic[5];
    fs.read(magic, 4);


    skBinaryFile *rval;

    // TODO: separate into 32/64bit files
    //       this only loads 64 bit at the moment.

    if (strncmp("\177ELF", magic, 4) == 0)
    {
        // rewind
        fs.seek(0, SEEK_SET);

        rval = new skElfFile();
        rval->load(fs);

        return rval;
    }
    return 0;
}



skBinaryFile::skBinaryFile() :
    m_data(0),
    m_len(0)
{
}

skBinaryFile::~skBinaryFile()
{
    delete[] m_data;
    skPrintUtils::writeColor(CS_WHITE);
}

void skBinaryFile::load(skStream &fstream)
{
    if (!fstream.isOpen())
    {
        skPrintf("load called on a closed file\n");
        return;
    }


    m_len  = fstream.size();
    m_data = new char[m_len + 1];
    m_len  = fstream.read(m_data, m_len);

    m_data[m_len] = 0;
    

    // Convert the rest in derrived classes.
    loadImpl();
}

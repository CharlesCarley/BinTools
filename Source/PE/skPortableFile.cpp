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
#include "Utils/skDebugger.h"
#include "skSection.h"


skPortableFile::skPortableFile(SKint16 dos_offset) :
    m_sectionStart(0),
    m_imageHeader(0),
    m_imageBase(dos_offset)
{
    skMemset(&m_header, 0, sizeof(COFFHeader));
}

skPortableFile ::~skPortableFile()
{
    delete m_imageHeader;
}


void skPortableFile::loadImpl(void)
{
    if (m_data == 0)
    {
        skPrintf("No data was loaded prior to calling load\n");
        return;
    }

    // The PE signature is not part of the defined structure (+4)
    char *ptr = m_data + 4;
    skMemcpy(&m_header, m_data + 4, sizeof(COFFHeader));


    COFFMachineType mt = (COFFMachineType)m_header.m_machine;

    m_instructionSetType = IS_X86;
    m_fileFormatType     = FFT_32BIT;

    ptr += sizeof(COFFHeader);


    if (m_header.m_optionalHeaderSize > 0)  // it's not an object file
    {
        if (*(SKuint16 *)ptr == COFF_MAG_PE32)
        {
            // runtime sanity check
            if (sizeof(COFFOptionalHeader32) != m_header.m_optionalHeaderSize)
            {
                skPrintf("COFFOptionalHeader32 not properly aligned %u %u-%u\n",
                         sizeof(COFFOptionalHeader32),
                         m_header.m_optionalHeaderSize);

                return;
            }

            m_imageHeader = new COFFOptionalHeader32;
            skMemcpy(m_imageHeader, (COFFOptionalHeader32 *)ptr, sizeof(COFFOptionalHeader32));

            m_sectionStart = 4 + sizeof(COFFHeader) + sizeof(COFFOptionalHeader32);
        }
        else if (*(SKuint16 *)ptr == COFF_MAG_PE64)
        {
            // runtime sanity check
            if (sizeof(COFFOptionalHeader64) != m_header.m_optionalHeaderSize)
            {
                skPrintf("COFFOptionalHeader64 not properly aligned %u-%u\n",
                         sizeof(COFFOptionalHeader64),
                         m_header.m_optionalHeaderSize);

                return;
            }

            m_imageHeader = new COFFOptionalHeader64;
            skMemcpy(m_imageHeader, ptr, sizeof(COFFOptionalHeader64));

            m_sectionStart = 4 + sizeof(COFFHeader) + sizeof(COFFOptionalHeader64);
        }
        else
        {
            skPrintf("COFFOptionalHeader: Unknown header type!\n");
            return;
        }
    }


    COFFSectionHeader *sectionPtr = reinterpret_cast<COFFSectionHeader *>(m_data + m_sectionStart);
    m_sectionHeaders.reserve(m_header.m_sectionCount);


    SKuint16 i16;
    for (i16 = 0; i16 < m_header.m_sectionCount; ++i16, ++sectionPtr)
    {
        COFFSectionHeader hdr;
        skMemcpy(&hdr, sectionPtr, sizeof(COFFSectionHeader));
        m_sectionHeaders.push_back(hdr);
    }


    SKsize si = 0,
           sl = m_sectionHeaders.size(), sn;

    Sections::PointerType p = m_sectionHeaders.ptr();
    while (si < sl)
    {
        COFFSectionHeader &sh = p[si++];

        char *name = (char *)sh.m_name;
        if ((*name) == '\0')
            continue;

        if (m_sectionLookup.find(name) == SK_NPOS)
        {
            m_sectionTable.insert(name, sh);
            m_sectionHeaderStringTable.push_back(name);

            sn = sh.m_pointerToRawData - m_imageBase;

            if (sn < m_len)  // make sure the requested memory is in range
            {
                m_sectionLookup.insert(name,
                                       new skSection(this, name, m_data + sn, (SKsize)sh.m_virtualSize));
            }
        }
        else
        {
            // this is an error, it shouldn't have duplicate symbols
            skPrintf("Exception: duplicate symbol name!");
        }
    }
}
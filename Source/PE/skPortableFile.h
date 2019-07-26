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
#ifndef _skPortableFile_h_
#define _skPortableFile_h_

#include "PE/skPortableTypes.h"
#include "skBinaryFile.h"


class skPortableFile : public skBinaryFile
{
public:
    typedef skArray<COFFSectionHeader>            Sections;
    typedef skHashTable<char*, COFFSectionHeader> SectionTable;

private:
    COFFHeader                m_header;
    COFFOptionalHeaderCommon* m_imageHeader;
    SKuint16                  m_imageBase;
    SKuint64                  m_sectionStart;
    Sections                  m_sectionHeaders;
    SectionTable              m_sectionTable;


    friend class skBinaryFile;
    skPortableFile(SKint16 dos_offset);

public:
    virtual ~skPortableFile();


    // Returns the header that is common amongst both object files and image files.
    inline const COFFHeader& getCommonHeader(void)
    {
        return m_header;
    }



    // Returns access to the 32Bit optional header
    // Note that it is only valid when getPlatformType() == FFT_32BIT
    void getOptionalHeader(COFFOptionalHeader32& dest);


    // Returns access to the 32Bit optional header
    // Note that it is only valid when getPlatformType() == FFT_64BIT
    void getOptionalHeader(COFFOptionalHeader64& dest);




    SKuint64 getEntryPoint(void)
    {
        if (m_imageHeader != 0)
            return m_imageHeader->m_entryPoint - m_imageBase;
        return (SKuint64)-1;
    }

private:
    inline size_t getSectionOffset(COFFSectionHeader& header)
    {
        // m_imageBase is the offset past the DOS stub program.
        // m_pointerToRawData points to the section location in the file.
        // m_data only points to the PE data
        return (size_t)(header.m_pointerToRawData);
    }


    virtual void loadImpl(void);
};

#endif  //_skPortableFile_h_

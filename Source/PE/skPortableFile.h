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
private:
    COFFHeader                m_header;
    COFFOptionalHeaderCommon* m_imageHeader;
    SKuint16                  m_headerOffs;
    SKuint64                  m_sectionStart;


    friend class skBinaryFile;
    skPortableFile(SKint16 dos_offset);

public:
    virtual ~skPortableFile();

    
    // Returns the file header
    inline const COFFHeader& getHeader(void)
    {
        return m_header;
    }

    // Returns the header that is common amongst both object files and image files.
    COFFOptionalHeaderCommon getCommonHeader(void)
    {
        // This needs to create a copy since it is stored 
        // in a pointer that may or may not be valid.
        COFFOptionalHeaderCommon res = {};
        if (m_imageHeader)
            res = (*m_imageHeader);
        return res;
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
            return m_imageHeader->m_entryPoint;
        return (SKuint64)-1;
    }

private:


    void loadResourceDirectory(skPortableSection* section, skPortableDirectory* directory);
    void loadImportDirectory(skPortableSection* section, skPortableDirectory* directory);

    template <typename COFFOptionalHeaderVaryingBase>
    void sortDataDirectories(void);


    virtual void loadImpl(skStream& stream);
};

#endif  //_skPortableFile_h_

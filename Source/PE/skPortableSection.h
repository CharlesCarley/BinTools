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
#ifndef _skPortableSection_h
#define _skPortableSection_h

#include "skSection.h"

#include "PE/skPortableTypes.h"


class skPortableSection : public skSection
{
public:
    typedef skArray<skPortableDirectory *> Directories;


private:
    COFFSectionHeader m_header;
    Directories       m_directories;

public:
    skPortableSection(skBinaryFile *     owner,
                      const skString &   name,
                      void *             data,
                      size_t             size,
                      size_t             offset,
                      COFFSectionHeader &hdr);

    virtual ~skPortableSection();


    bool isExectuable(void)
    {
        return (m_header.m_characteristics & CSEC_MEM_CAN_EXEC) != 0;
    }

    bool isReadable(void)
    {
        return (m_header.m_characteristics & CSEC_MEM_CAN_READ) != 0;
    }

    bool isWritable(void)
    {
        return (m_header.m_characteristics & CSEC_MEM_CAN_WRITE) != 0;
    }

    inline const COFFSectionHeader &getHeader(void) const
    {
        return m_header;
    }



    Directories::Iterator getDirectoryIterator(void)
    {
        return m_directories.iterator();
    }


    void _addDirectory(COFFDirectoryEnum dir, const COFFDataDirectory &dd);

};

#endif  //_skPortableSection_h

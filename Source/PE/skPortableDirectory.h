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
#ifndef _skPortableDirectory_h
#define _skPortableDirectory_h

#include "PE/skPortableTypes.h"

class skPortableSection;


class skPortableDirectory
{
public:
    skPortableDirectory();
    skPortableDirectory(skPortableSection *owner, COFFDirectoryEnum en, const COFFDataDirectory &dd);
    
    // Returns the start address relative to the owning section.
    SKuint32 getAddress(void) const;
    

    // Returns the type of directory associated with the extracted structure
    inline COFFDirectoryEnum getType(void) const
    {
        return m_enum;
    }


    // Returns the type of directory associated with the extracted structure
    inline const COFFDataDirectory &getDirectory(void) const
    {
        return m_dir;
    }

    // Returns the relative virtual address
    inline SKuint32 getRVA(void) const
    {
        return m_dir.m_virtualAddress;
    }

    // Returns the relative virtual address
    inline SKuint32 getSize(void) const
    {
        return m_dir.m_size;
    }



    inline skPortableSection *getOwner(void) const
    {
        return m_owner;
    }

private:
    skPortableSection *m_owner;
    COFFDirectoryEnum  m_enum;
    COFFDataDirectory  m_dir;
};

#endif  //_skPortableDirectory_h

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
#include "PE/skPortableSection.h"
#include "PE/skPortableDirectory.h"


skPortableDirectory::skPortableDirectory() :
    m_owner(0),
    m_enum(CDE_UNKNOWN),
    m_dir()
{
}


skPortableDirectory::skPortableDirectory(skPortableSection *      owner,
                                         COFFDirectoryEnum        en,
                                         const COFFDataDirectory &dd) :
    m_owner(owner),
    m_enum(en),
    m_dir(dd)
{
}


SKuint32 skPortableDirectory::getAddress(void) const
{
    if (m_owner)
    {
        const COFFSectionHeader &hdr = m_owner->getHeader();
        // USE:  hdr.m_pointerToRawData + (m_dir.m_virtualAddress - hdr.m_virtualAddress)
        // If its relative to the start of the file data otherwise:
        return skClamp<SKuint32>(m_dir.m_virtualAddress - hdr.m_virtualAddress, 0, hdr.m_sizeOfRawData);
    }
    return -1;
}

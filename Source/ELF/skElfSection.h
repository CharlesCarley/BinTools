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
#ifndef _skElfSection_h_
#define _skElfSection_h_

#include "ELF/skElfTypes.h"
#include "skSection.h"


class skElfSection : public skSection
{
private:
    skElfSectionHeader64 m_header;

public:
    skElfSection(skBinaryFile*               owner,
                 const skString&             name,
                 void*                       data,
                 size_t                      size,
                 size_t                      offset,
                 const skElfSectionHeader64& hdr);

    virtual ~skElfSection();

    bool isExectuable(void)
    {
        return (m_header.m_flags & ESHF_EXEC_INST) != 0;
    }
    
    bool isReadable(void)
    {
        return (m_header.m_flags & ESHF_ALLOC) != 0;
    }

    bool isWritable(void)
    {
        return (m_header.m_flags & ESHF_WRITE) != 0;
    }


    // Returns the ELF section header structure for this section.
    inline const skElfSectionHeader64& getHeader(void) const
    {
        return m_header;
    }
};

#endif  //_skElfSection_h_

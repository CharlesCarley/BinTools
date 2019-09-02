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
#ifndef _skSection_h_
#define _skSection_h_

#include "Utils/skString.h"
#include "skBinaryFileCommon.h"


class skSection
{
protected:
    SKuint8 *     m_data;
    size_t        m_size;
    skBinaryFile *m_owner;
    size_t        m_startAddress;
    skString      m_name;

public:
    skSection(skBinaryFile *owner, const skString &name, void *data, size_t size, size_t offset);
    virtual ~skSection();


    virtual bool isExectuable(void) = 0;
    virtual bool isReadable(void)   = 0;
    virtual bool isWritable(void)   = 0;


    // Returns a pointer to the file that owns this section.
    inline skBinaryFile *getOwner(void) const
    {
        return m_owner;
    }


    // Returns the section name. (.init, .text, etc...)
    inline const skString &getName(void) const
    {
        return m_name;
    }


    // Returns direct access to the allocated memory for this section.
    inline SKuint8 *getPointer(void) const
    {
        return m_data;
    }

    // Returns the amount of allocated memory in bytes.
    inline size_t getSize(void) const
    {
        return m_size;
    }

    // Returns the starting location of this section in the file.
    inline size_t getStartAddress(void) const
    {
        return m_startAddress;
    }
};

#endif  //_skBinaryFile_h_

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
#ifndef _skSymbol_h_
#define _skSymbol_h_

#include "Utils/skString.h"
#include "skBinaryFileCommon.h"


class skSymbol
{
protected:
    skBinaryFile *m_owner;
    skString      m_name;


public:
    skSymbol(skBinaryFile *owner, const skString &name);
    virtual ~skSymbol();


    // Returns a pointer to the file that owns this section.
    inline skBinaryFile *getOwner(void) const
    {
        return m_owner;
    }


    // Returns the symbol name
    virtual inline const skString &getName(void)
    {
        return m_name;
    }


    virtual SKuint64 getAddress(void) = 0;
};

#endif  //_skSymbol_h_

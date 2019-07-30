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
#ifndef _skPortableSymbol_h_
#define _skPortableSymbol_h_

#include "PE/skPortableTypes.h"
#include "skSymbol.h"



class skPortableSymbol : public skSymbol
{
private:
    SKuint64 m_rva;
    skString m_name;
    skString m_dll;


public: 
    skPortableSymbol(skBinaryFile* owner, const skString& name, const skString& location, const SKuint64& rva);
    virtual ~skPortableSymbol();


    inline const skString& getName(void)
    {
        return m_name;
    }

    inline const skString& getDLL(void)
    {
        return m_dll;
    }


    inline SKuint64 getAddress(void)
    {
        return m_rva;
    }
};

#endif  //_skPortableSymbol_h_

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
#ifndef _skElfSymbol_h_
#define _skElfSymbol_h_

#include "skSymbol.h"
#include "ELF/skElfTypes.h"

class skElfSymbol : public skSymbol
{
protected:
    skElfSymbol64 m_symbol;



public:
    skElfSymbol(skBinaryFile *owner, const skString &name, const skElfSymbol64& sym);
    virtual ~skElfSymbol();


    inline const skElfSymbol64& getSymbol(void) const
    {
        return m_symbol;
    }

    inline SKuint64 getAddress(void)
    {
        return m_symbol.m_value;
    }
};

#endif  //_skElfSymbol_h_

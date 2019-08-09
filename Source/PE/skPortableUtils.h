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
#ifndef _skPortableUtils_h_
#define _skPortableUtils_h_

#include "PE/skPortableTypes.h"
class skString;

class skPortableUtils
{
public:
    static void getMachine(const COFFHeader& inf, char dest[], int len);
    static void getPlatformId(const COFFOptionalHeaderCommon& inf, char dest[], int len);
    static void getCharacteristicsString16(const SKuint16& inf, skString& dest);
    static void getSubsystem(const SKuint16& ss, char dest[], int len);
};


#endif  //_skPortableUtils_h_

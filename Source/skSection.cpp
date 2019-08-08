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
#include "skSection.h"
#include "Utils/skDebugger.h"
#include "Utils/skMemoryUtils.h"
#include "skBinaryFile.h"



skSection::skSection(skBinaryFile *owner, const skString &name, void *data, size_t size, size_t offset) :
    m_name(name),
    m_data(0),
    m_size(0),
    m_owner(owner),
    m_startAddress(offset),
    m_isExecutable(false)
{
    // passes ownership!
    m_data = (SKuint8 *)data;
    m_size = size;
}

skSection::~skSection()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = 0;
    }
    m_size = 0;
}


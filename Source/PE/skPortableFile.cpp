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
#include "skPortableFile.h"
#include "Utils/skDebugger.h"


skPortableFile::skPortableFile()
{
}

skPortableFile ::~skPortableFile()
{
}


void skPortableFile::loadImpl(void)
{
    if (m_data == 0)
    {
        skPrintf("No data was loaded prior to calling load\n");
        return;
    }

    // The PE signature is not part of the defined structure (+4)
    char* ptr = m_data + 4;
    skMemcpy(&m_header, m_data+4, sizeof(COFFHeader));


    COFFMachineType mt = (COFFMachineType)m_header.m_machine;

    ptr += sizeof(COFFHeader);


    if (m_header.m_optionalHeaderSize > 0) // it's an image 
    {
        COFFOptionalHeaderStandard* optstd = (COFFOptionalHeaderStandard*)ptr;


        skPrintf("%i\n", optstd->m_magic);


    }
}

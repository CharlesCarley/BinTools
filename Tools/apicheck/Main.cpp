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
#include<iostream>
#include<iomanip>
#include "skBinaryFile.h"
#include "Utils/skDebugger.h"
using namespace std;

#define bcDefaultFill setw(16) << setfill(' ')  << right

struct bcProgramInfo
{
    skBinaryFile *m_fp;

};


void bcPrintBinaryFile(void);


static bcProgramInfo gs_ctx;




int main(int argc, char **argv)
{
    gs_ctx.m_fp = 0;

    if (argc < 2)
    {
        printf("%s file\n", argv[0]);
        return -1;
    }

    gs_ctx.m_fp = skBinaryFile::load(argv[1]);
    
    bcPrintBinaryFile();


    delete gs_ctx.m_fp;
    return 0;
}



void bcPrintBinaryFile(void)
{
    skBinaryFile* fp = gs_ctx.m_fp;
    if (fp)
    {
        cout << "Architecture:  ";
        cout << bcDefaultFill  << (int)fp->getArchitecture() << endl;

        cout << "FileFormat:    ";
        cout << bcDefaultFill << (int) fp->getFormat() << endl;

        cout << "Length:        ";
        cout << bcDefaultFill << fp->getLength() << endl;

        cout << "Section Count: ";
        cout << bcDefaultFill << fp->getSectionCount() << endl;
    }
}

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
#include <iostream>
#include <string>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void b2Usage(void);
int  b2ParseCommandLine(int argc, char** argv);
void b2LoadData(void);
void b2PrintData(void);


int main(int argc, char** argv)
{
    if (b2ParseCommandLine(argc, argv) == -1)
    {
        b2Usage();
        return -1;
    }

    b2LoadData();
    b2PrintData();

    return 0;
}


void b2Usage(void)
{
    printf("Usage\n");
}

int b2ParseCommandLine(int argc, char** argv)
{
    printf("ParseComandLine\n");
    return 1;
}

void b2LoadData(void)
{
    printf("LoadFile\n");
}

void b2PrintData(void)
{
    printf("PrintFile\n");
}


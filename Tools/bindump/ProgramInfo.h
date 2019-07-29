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
#ifndef _ProgramInfo_h_
#define _ProgramInfo_h_

class skBinaryFile;


enum HexDump_MenuState
{
    MS_MAIN = 0,
    MS_EXIT,
};


struct HexDump_ProgramInfo
{
    int           m_state;
    skBinaryFile* m_fp;
    int           m_flags;
    int           m_code;
    int           m_opt;
    int           m_cache;
    string        m_fileName;
};


extern int  HexDump_ParseCommandLine(HexDump_ProgramInfo& prog, int argc, char** argv);
extern void HexDump_Usage(void);
extern void HexDump_Interactive(HexDump_ProgramInfo& prog);

extern void HexDump_PrintAll(HexDump_ProgramInfo& prog);
extern void HexDump_PrintSectionNames(HexDump_ProgramInfo& prog);
extern void HexDump_PrintSections(HexDump_ProgramInfo& prog);
extern void HexDump_PrintSymbols(HexDump_ProgramInfo& prog);


#endif//_ProgramInfo_h_

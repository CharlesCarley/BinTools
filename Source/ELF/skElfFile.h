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
#ifndef _skElfFileHeader_h_
#define _skElfFileHeader_h_

#include "ELF/skElfTypes.h"
#include "Utils/skArray.h"
#include "skBinaryFile.h"

class skElfFile : public skBinaryFile
{
private:
    skElfHeaderInfo64 m_header;
    elf64             m_strtab;


    friend class skBinaryFile;
    skElfFile(int flags);

public:
    virtual ~skElfFile();

    // Return access to the ELF file header 
    inline const skElfHeaderInfo64& getHeader(void) const
    {
        return m_header;
    }

    inline bool is64Bit(void)
    {
        return m_header.m_id[EMN_CLASS] == 0x02;
    }

    inline ElfInstructionArch getInstructionArchitecture(void) const
    {
        return (ElfInstructionArch)m_header.m_machine;
    }

    inline ElfType getElfType(void) const
    {
        return (ElfType)m_header.m_type;
    }

    inline SKuint64 getEntryPoint(void)
    {
        return m_header.m_entry;
    }


    /// Returns the starting offset to the section header
    inline elf64 getSectionHeaderStart(void) const
    {
        return m_header.m_sectionOffset;
    }

    /// Returns the starting offset to the section header
    inline elf64 getSectionHeaderEnd(void) const
    {
        return m_header.m_sectionOffset + (elf64)m_header.m_sectionTableEntryCount * (elf64)m_header.m_sectionTableEntrySize;
    }


private:

    template <typename skElfSectionHeader>
    int loadSections(skStream &stream);

    template <typename skElfSymbolHeader>
    int loadSymbolTable(const char* str, const char* sym);

    virtual int loadImpl(skStream& stream);
};

#endif  //_skElfFileHeader_h_

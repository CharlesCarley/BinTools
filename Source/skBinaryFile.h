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
#ifndef _skBinaryFile_h_
#define _skBinaryFile_h_

#include "Utils/skArray.h"
#include "Utils/skString.h"
#include "skBinaryFileCommon.h"


class skBinaryFile
{
public:
    typedef skHashTable<skString, skSection *> SectionTable;
    typedef skHashTable<skString, skSymbol *>  SymbolTable;

protected:
    SKsize                m_len;
    skFileFormat          m_fileFormat;
    skFileFormatType      m_fileFormatType;
    skMachineArchitecture m_arch;
    SectionTable          m_sectionLookup;
    SymbolTable           m_symTable;
    
    // Protected constructor. Load files via the static interface
    // skBinaryFile::load(file).
    skBinaryFile();

    int load(skStream &fstream);

public:
    virtual ~skBinaryFile();


    // Create instances based on the supplied file's magic.
    // Returns skDefaultFile if the file format is not supported, or NULL if the
    // file can not be loaded. Instances created through this function sill must be
    // deleted when no longer needed.
    static int load(const char *file, skBinaryFile **fp);


    // Returns the format of the loaded file.
    inline skFileFormat getFormat(void)
    {
        return m_fileFormat;
    }

    // Returns the files platform type
    inline skFileFormatType getPlatformType(void)
    {
        return m_fileFormatType;
    }

    // Returns an abstract enumeration representing
    // the underlying files architecture.
    inline skMachineArchitecture getArchitecture(void)
    {
        return m_arch;
    }


    // Returns the number of loaded sections.
    inline SKsize getSectionCount(void)
    {
        return m_sectionLookup.size();
    }


    // Returns the section at the specified index or NULL if the supplied index is out of bounds. 
    skSection* getSection(SKsize idx)
    {
        if (idx < m_sectionLookup.size())
            return m_sectionLookup.at(idx);
        return 0;
    }


    // Returns an iterator for all sections
    inline SectionTable::Iterator getSectionIterator(void)
    {
        return m_sectionLookup.iterator();
    }


    // Returns an iterator for converted symbols
    inline SymbolTable::Iterator getSymbolIterator(void)
    {
        return m_symTable.iterator();
    }


    // Search for a named section.
    // returns NULL if the requested section name is not found.
    skSection *getSection(const char *sinf)
    {
        if (sinf)
        {
            SKsize idx = m_sectionLookup.find(const_cast<char *>(sinf));
            if (idx != SK_NPOS)
                return m_sectionLookup[idx];
        }
        return 0;
    }


    // Returns the size in bytes for the loaded file.
    inline SKsize getLength(void)
    {
        return m_len;
    }

    virtual SKuint64 getEntryPoint(void) = 0;

protected:
    // Called from load.
    // loadImpl is responsible for converting the underlying data into this interface.
    virtual int loadImpl(skStream &stream) = 0;
};

#endif  //_skBinaryFile_h_

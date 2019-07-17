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
#ifndef _skElfTypes_h_
#define _skElfTypes_h_

// Definitions are referenced from /usr/include/elf.h.

typedef unsigned char      elf8;
typedef unsigned short     elf16;
typedef unsigned int       elf32;
typedef unsigned long long elf64;
typedef char*              elfName;

class skBinaryFile;


enum ElfMagicNumbers
{
    EMN_0X7F  = 0,
    EMN_CLASS = 4,
    EMN_ENDIAN,
    EMN_VERSION,
    EMN_ABI,
    EMN_ABI_VER,
};

enum ElfType
{
    ET_NONE  = 0x00,
    ET_REL   = 0x01,
    ET_EXEC  = 0x02,
    ET_DYN   = 0x03,
    ET_CORE  = 0x04,
    ET_LOPRC = 0xFF00,
    ET_HIPRC = 0xFFFF,
};


enum ElfInstructionArch
{
    EIA_NONE    = 0x00,
    EIA_SPARC   = 0x02,
    EIA_X86     = 0x03,
    EIA_MPS     = 0x08,
    EIA_POWERPC = 0x14,
    EIA_S390    = 0x16,
    EIA_ARM     = 0x28,
    EIA_SUPERH  = 0x2A,
    EIA_IA64    = 0x32,
    EIA_X8664   = 0x3E,
    EIA_AARCH64 = 0xB7,
    EIA_RISCV   = 0xF3
};

enum ElfProgramHeaderType
{
    EPHT_NONE   = 0x00,
    EPHT_LOAD   = 0x00,
    EPHT_DYN    = 0x00,
    EPHT_INTERP = 0x00,
    EPHT_NOTE   = 0x00,
    EPHT_SHLIB  = 0x00,
    EPHT_HEADER = 0x00,
};


enum ElfSectionHeaderType
{
    EST_NONE     = 0x00,
    EST_PROGBITS = 0x01,
    EST_SYMTAB   = 0x02,
    EST_STARTTAB = 0x03,
    EST_RELA     = 0x04,
    EST_HASH     = 0x05,
    EST_DYNAMIC  = 0x06,
    EST_NOTE     = 0x07,
    EST_NOBITS   = 0x08,
    EST_REL      = 0x09,
    EST_SHLIB    = 0x0A,
    EST_DYNSYM   = 0x0B,
    EST_NUM      = 0x0C,
    EST_LOPROC   = 0x70000000,
    EST_HIPROC   = 0x7FFFFFFF,
    EST_LOUSR    = 0x80000000,
    EST_HIUSR    = 0xFFFFFFFF,
};


enum ElfSectionHeaderFlags
{
    ESHF_NONE          = 0x00,
    ESHF_WRITE         = 0x1,  // can write at runtime
    EPHT_ALLOC         = 0x2,  // virtual memory
    EPHT_EXEC_INST     = 0x4,  // is executable
    EPHT_LIVE_PATCH    = 0x00100000,
    EPHT_RO_AFTER_INIT = 0x00200000,
    EPHT_MASK_PROC     = 0xF0000000,
};


// Short hand to generalize definitions
template <typename elfT>
struct skElfHeaderInfo
{
    elf8  m_id[16];
    elf16 m_type, m_machine;
    elf32 m_ver;
    elfT  m_entry;  // entry point
    elfT  m_programOffset;
    elfT  m_sectionOffset;
    elf32 m_flags;
    elf16 m_headerSizeInBytes;
    elf16 m_headerEntrySize;
    elf16 m_headerEntryCount;
    elf16 m_sectionTableEntrySize;
    elf16 m_sectionTableEntryCount;
    elf16 m_sectionTableIndex;
};

template <typename elfT>
struct skElfProgramHeader
{
    elf32 m_type;
    elf32 m_flags;
    elfT  m_offset;
    elfT  m_vadr;
    elfT  m_padr;
    elfT  m_fileSize;
    elfT  m_memSize;
    elfT  m_align;
};

template <typename elfT>
struct skElfSectionHeader
{
    elf32 m_name;
    elf32 m_type;
    elfT  m_flags;
    elfT  m_addr;
    elfT  m_offset;
    elfT  m_size;
    elf32 m_link;
    elf32 m_info;
    elfT  m_addrAlign;
    elfT  m_entSize;
};

typedef skElfHeaderInfo<elf64>    skElfHeaderInfo64;
typedef skElfSectionHeader<elf64> skElfSectionHeader64;
typedef skElfProgramHeader<elf64> skElfProgramHeader64;

typedef skElfHeaderInfo<elf32>    skElfHeaderInfo32;
typedef skElfSectionHeader<elf32> skElfSectionHeader32;
typedef skElfProgramHeader<elf32> skElfProgramHeader32;

#endif  //_skElfTypes_h_

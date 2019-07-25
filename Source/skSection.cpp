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
#include "capstone/capstone.h"
#include "skBinaryFile.h"
#include "skPrintUtils.h"



cs_arch skSection_getCapStoneArch(skMachineArchitecture set);



skSection::skSection(skBinaryFile *owner, const skString &name, void *data, size_t size, size_t offset) :
    m_name(name),
    m_data(0),
    m_size(0),
    m_owner(owner),
    m_handle(-1),
    m_startAddress(offset),
    m_isExecutable(false)
{
    initialize(data, size);
}

skSection::~skSection()
{
    if (m_handle != -1)
        cs_close(&m_handle);

    if (m_data)
    {
        delete[] m_data;
        m_data = 0;
    }
    m_size = 0;
}


void skSection::initialize(void *ptr, size_t size)
{
    if (!ptr || size == 0)
        return;

    m_data = new ValueType[size + 1];
    skMemcpy(m_data, ptr, size);

    m_data[size] = 0;
    m_size       = size;


    cs_arch arch = skSection_getCapStoneArch(m_owner->getArchitecture());

    cs_mode mode = m_owner->getType() == FFT_32BIT ? CS_MODE_32 : CS_MODE_64;

    cs_err err = cs_open(arch, mode, (csh *)&m_handle);
    if (err != CS_ERR_OK)
    {
        skPrintf("Capstone Error: cs_open returned %i\n", err);
        return;
    }
}



void skSection::dissemble(int flags)
{
    if (m_handle == -1)
    {
        skPrintf("dissemble was called with an invalid handle.\n");
        return;
    }

    // filter out non executable sections.
    if (!m_isExecutable)
    {
        skPrintUtils::dumpHex(m_data, m_startAddress, m_size, PF_DEFAULT, -1);
        return;

    }

    cs_insn *insn;

    size_t count = cs_disasm(m_handle, (uint8_t *)m_data, (size_t)m_size, m_startAddress, 0, &insn);
    if (count > 0)
    {
        size_t j;
        for (j = 0; j < count; j++)
        {
            cs_insn &i = insn[j];
            if (flags & PF_COLORIZE)
                skPrintUtils::writeColor(CS_LIGHT_GREY);

            skPrintUtils::writeAddress(i.address);
            skPrintUtils::dumpHex(i.bytes, 0, i.size, PF_HEXDIS, -1, false);

            skPrintUtils::writeColor(CS_WHITE);
            skPrintf("%s\t%s\n", i.mnemonic, i.op_str);
        }

        cs_free(insn, count);
    }
}

cs_arch skSection_getCapStoneArch(skMachineArchitecture set)
{
    switch (set)
    {
    case IS_SPARC:
        return CS_ARCH_SPARC;
    case IS_MPS:
        return CS_ARCH_MIPS;
    case IS_POWERPC:
        return CS_ARCH_PPC;
    case IS_S390:
        return CS_ARCH_SPARC;
    case IS_AARCH64:
        return CS_ARCH_ARM64;
    case IS_X8664:
    case IS_X86:
        return CS_ARCH_X86;
    default:
        break;
    }
    return CS_ARCH_ALL;
}

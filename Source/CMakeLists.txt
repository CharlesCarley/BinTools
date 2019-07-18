# -----------------------------------------------------------------------------
#
#   Copyright (c) 2019 Charles Carley.
#
#   Contributor(s): none yet
#
# ------------------------------------------------------------------------------
#   This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.
# ------------------------------------------------------------------------------
set(TargetName          BinaryFile)
set(TargetName_GRP      "")

set(TargetName_SRC
    skBinaryFile.h
    skBinaryFileCommon.h
    skDefaultFile.h
    skPrintUtils.h
    skSection.h

    ELF/skElf.h
    ELF/skElfTypes.h
    ELF/skElfUtils.h
    ELF/skElfSection.h

    PE/skPortableFile.h
    PE/skPortableTypes.h
    PE/skPortableSection.h
    PE/skPortableUtils.h
    
    skBinaryFile.cpp
    skDefaultFile.cpp
    skPrintUtils.cpp
    skSection.cpp

    ELF/skElfUtils.cpp
    ELF/skElf.cpp
    ELF/skElfSection.cpp

    PE/skPortableFile.cpp
    PE/skPortableTypes.cpp
    PE/skPortableSection.cpp
    PE/skPortableUtils.cpp
)


include_directories(. 
    ${CMAKE_SOURCE_DIR}/Extern
    ${CAPSTONE_INCLUDE}
)

add_library(${TargetName} ${TargetName_SRC})
target_link_libraries(${TargetName}
    ${CAPSTONE_LIB}
)


if (BinTools_USE_FOLDERS)
    set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetName_GRP}")
endif()



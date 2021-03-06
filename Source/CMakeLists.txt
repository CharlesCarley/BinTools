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



set(TargetName_HDR
    skBinaryFile.h
    skBinaryFileCommon.h
    skSection.h
    skSymbol.h

    # ELF
    ELF/skElfFile.h
    ELF/skElfTypes.h
    ELF/skElfUtils.h
    ELF/skElfSection.h
    ELF/skElfSymbol.h
    ELF/skElfPrintUtils.h

    # PE
    PE/skPortableDirectory.h
    PE/skPortableTypes.h
    PE/skPortableFile.h
    PE/skPortableSection.h
    PE/skPortableSymbol.h
    PE/skPortableUtils.h

)


set(TargetName_SRC
    skBinaryFile.cpp
    skSection.cpp
    skSymbol.cpp
    
    # ELF 
    ELF/skElfFile.cpp
    ELF/skElfSection.cpp
    ELF/skElfSymbol.cpp
    ELF/skElfUtils.cpp

    #  PE  
    PE/skPortableDirectory.cpp
    PE/skPortableFile.cpp
    PE/skPortableSection.cpp
    PE/skPortableSymbol.cpp
    PE/skPortableUtils.cpp
)



include_directories(. 
    ${BinTools_Utils_INCLUDE}
)



add_library(${TargetName} 
	${TargetName_SRC}
	${TargetName_HDR}
)

if (BinTools_USE_FOLDERS)
    set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetName_GRP}")
endif()


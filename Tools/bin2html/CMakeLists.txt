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
set(TargetName          bin2html)
set(TargetName_GRP      "Tools")

#include (Data2Array)
#add_templates_text(Templates.h Footer.html Header.html)

set(TargetName_SRC 
    Main.cpp
    Templates.h 
    Header.html
    Readme.md
    Footer.html
)

include_directories(
    ${BINARYFILE_INCLUDE}
    ${UTILS_INCLUDE}
    ${CAPSTONE_INCLUDE}

)

add_executable(${TargetName} ${TargetName_SRC})

target_link_libraries(
    ${TargetName} 
    ${BINFILE_LIB}
    ${UTILS_LIB}
    ${CAPSTONE_LIB}
)

if (BinTools_USE_FOLDERS)
    set_target_properties(${TargetName} PROPERTIES FOLDER "${TargetName_GRP}")
endif()


if (NOT WIN32)
    add_custom_command(TARGET ${TargetName} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${TargetName} "${CMAKE_BINARY_DIR}/${TargetName}"
    )
endif()


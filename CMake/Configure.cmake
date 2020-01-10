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
option(BinTools_USE_FOLDERS     "Organize into folders" OFF)




# -----------------------------------------------------------------------------
#                            Source Configuration
# -----------------------------------------------------------------------------

if (BinTools_Utils_EXTERNAL)
   set(BinTools_Utils_INCLUDE ${Utils_INCLUDE})
   set(BinTools_Utils_LIBRARY ${Utils_LIBRARY})
else()
   set(BinTools_Utils_EXTERNAL FALSE)
   set(BinTools_Utils_INCLUDE  ${BinTools_SOURCE_DIR}/Extern)
   set(BinTools_Utils_LIBRARY  Utils)

    set(UTILS_TestDir      ${BinTools_SOURCE_DIR}/Extern/Utils/CMake)
    if (NOT IS_DIRECTORY ${UTILS_TestDir})

        message(STATUS "Cloning Utils into ${BinTools_SOURCE_DIR}/Extern/Utils")
        execute_process( 
            WORKING_DIRECTORY ${BinTools_SOURCE_DIR} 
            COMMAND git submodule update --init --checkout
            ERROR_FILE ${BinTools_BINARY_DIR}/GetSubmoduleUpdate.txt
            OUTPUT_QUIET
            ERROR_QUIET
            )
        if (NOT IS_DIRECTORY ${UTILS_TestDir})
            message("Failed to clone Utils. See ${BinTools_BINARY_DIR}/GetSubmoduleUpdate.txt for more information.")
        endif()
    endif()


endif()


set(BINARYFILE_INCLUDE   ${BinTools_SOURCE_DIR}/Source)
set(CAPSTONE_INCLUDE     ${BinTools_SOURCE_DIR}/Extern/capstone/include)

set(CAPSTONE_LIB        capstone-static)
set(BINFILE_LIB         BinaryFile)



# -----------------------------------------------------------------------------
#                            Show config
# -----------------------------------------------------------------------------
macro(log_config)
    message(STATUS "-----------------------------------------------------------")
    message(STATUS " ")
    message(STATUS "---------- Utils --------")
    message(STATUS "Is external                             : ${BinTools_Utils_EXTERNAL}")
    message(STATUS "Include                                 : ${BinTools_Utils_INCLUDE}")
    message(STATUS "Library                                 : ${BinTools_Utils_LIBRARY}")
    message(STATUS "Print using printf                      : ${Utils_NO_DEBUGGER}")
    message(STATUS "Using standard strings                  : ${Utils_USE_STD_STRING_FUNCS}")
    message(STATUS "Using assert in debug                   : ${Utils_USE_DEBUG_ASSERT}")
    message(STATUS "Using assert in iterators               : ${Utils_USE_ITERATOR_DEBUG}")
    message(STATUS " ")
    message(STATUS "---------- BinTools ----------")
    message(STATUS "Sorting Into Folders                    : ${BinTools_USE_FOLDERS}")
    message(STATUS " ")
    message(STATUS "-----------------------------------------------------------")
endmacro(log_config)

if (BinTools_USE_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
endif()

include(StaticRuntime)

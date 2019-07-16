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
option(BinTools_USE_FOLDERS "Organize into folders" ON)


# -----------------------------------------------------------------------------
#                            Source Configuration
# -----------------------------------------------------------------------------
set(BINARYFILE_INCLUDE  ${BinTools_SOURCE_DIR}/Source)
set(UTILS_INCLUDE       ${BinTools_SOURCE_DIR}/Extern)
set(CAPSTONE_INCLUDE    ${BinTools_SOURCE_DIR}/Extern/capstone/include)

set(CAPSTONE_LIB        capstone-static)
set(BINFILE_LIB         BinaryFile)
set(UTILS_LIB           Utils)



# -----------------------------------------------------------------------------
#                            Show config
# -----------------------------------------------------------------------------
macro(log_config)
    message(STATUS "-----------------------------------------------------------")
    message(STATUS " ")
    message(STATUS "---------- Utils --------")
    message(STATUS "Print using printf                      : ${Utils_NO_DEBUGGER}")
    message(STATUS "Using Memory allocator                  : ${Utils_USE_MEMORY_ALLOCATOR}")
    message(STATUS "Using int size_t                        : ${Utils_USE_SIGNED_SIZE_T}")
    message(STATUS "Using standard strings                  : ${Utils_USE_STD_STRING_FUNCS}")
    message(STATUS "Using assert in debug                   : ${Utils_USE_DEBUG_ASSERT}")
    message(STATUS "Using assert in iterators               : ${Utils_USE_ITERATOR_DEBUG}")
    message(STATUS "Using double scalar type                : ${Utils_USE_DOUBLE}")
    message(STATUS "Add assertion macros                    : ${Utils_OP_CHECKS}")
    message(STATUS " ")
    message(STATUS "---------- BinTools ----------")
    message(STATUS "Sorting Into Folders                    : ${BinTools_USE_FOLDERS}")
    message(STATUS " ")
    message(STATUS "-----------------------------------------------------------")
endmacro(log_config)


log_config()

if (BinTools_USE_FOLDERS)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON) 
endif()



include(StaticRuntime)

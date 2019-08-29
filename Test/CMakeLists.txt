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
set(TargetName_GRP      "TestFiles")


add_executable(helloworld test.c)
add_library(helloworld_shared SHARED sharedlib.c)


if (MSVC)
    add_executable(buffer_overflow buffer_overflow.c)
    set_target_properties(buffer_overflow PROPERTIES COMPILE_FLAGS "/GS-")
endif()

if (BinTools_USE_FOLDERS)
    set_target_properties(helloworld        PROPERTIES FOLDER "${TargetName_GRP}")
    set_target_properties(helloworld_shared PROPERTIES FOLDER "${TargetName_GRP}")
    set_target_properties(buffer_overflow   PROPERTIES FOLDER "${TargetName_GRP}")
endif()

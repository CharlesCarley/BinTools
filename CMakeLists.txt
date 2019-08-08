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
cmake_minimum_required(VERSION 3.6)
project(BinTools)

set(CMAKE_MODULE_PATH 
    ${BinTools_SOURCE_DIR}/CMake
    ${BinTools_SOURCE_DIR}/Extern/Utils/CMake
    ${BinTools_SOURCE_DIR}/Tools/Data2Array/CMake
)
subdirs(CMake)


# This is used for controlling the external Utils project without 
# having to modify the build scripts
set(FOLDER_NAME         "External")
set(USE_FOLDERS         TRUE)
set(Utils_SOURCE_DIR    "${BinTools_SOURCE_DIR}/Extern/Utils")


include(Configure)
include (ConfigureUtils)

log_config()


subdirs(Extern/Utils)
subdirs(Extern/capstone)

subdirs(Source)
subdirs(Test)               # Test binaries to load and inspect
subdirs(Tools/bindump)      # Test output for writing 

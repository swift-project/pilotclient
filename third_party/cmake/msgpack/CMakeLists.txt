# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# for simplicity, we are using our own CMake file to provide the msgpack-cxx target

add_library(msgpack-cxx INTERFACE)

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/third_party/msgpack/include/)
    message(FATAL_ERROR "msgpack not found. Did you forget to initialize the git submodules?")
endif()

target_include_directories(msgpack-cxx INTERFACE ${PROJECT_SOURCE_DIR}/third_party/msgpack/include)

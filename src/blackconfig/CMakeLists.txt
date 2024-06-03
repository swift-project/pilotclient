# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

configure_file(buildconfig_gen.inc.in buildconfig_gen.inc)
configure_file(buildconfig_gen.cpp.in buildconfig_gen.cpp)

add_library(blackconfig STATIC
        buildconfig.cpp
        buildconfig.h
        buildconfig.inc
        ${CMAKE_CURRENT_BINARY_DIR}/buildconfig_gen.inc
        ${CMAKE_CURRENT_BINARY_DIR}/buildconfig_gen.cpp
        )

target_link_libraries(blackconfig PUBLIC Qt::Core)

target_include_directories(blackconfig
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
)
set_property(TARGET blackconfig PROPERTY POSITION_INDEPENDENT_CODE ON)

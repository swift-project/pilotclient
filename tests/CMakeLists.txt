# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(tests_test INTERFACE test.h)
target_link_libraries(tests_test INTERFACE Qt::Core Qt::Test)
target_include_directories(tests_test INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(misc)

if(SWIFT_BUILD_FSX_PLUGIN)
    add_subdirectory(swiftsimpluginfsxp3d)
endif()

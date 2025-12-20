# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(plugincommon SHARED
        simulatorplugincommon.cpp
        simulatorplugincommon.h
        simulatorplugincommonexport.h
)

target_include_directories(plugincommon PUBLIC ${PROJECT_SOURCE_DIR}/src)

set_target_properties(plugincommon PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
set_target_properties(plugincommon PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)

force_asserts(plugincommon)
target_link_libraries(plugincommon
        PUBLIC
        Qt::Core
        core
        gui
)

target_compile_definitions(plugincommon PRIVATE BUILD_SIMULATORPLUGINCOMMON_LIB)

install(TARGETS plugincommon
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

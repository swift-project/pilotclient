# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatormsfsconfig SHARED
        simulatormsfsconfig.cpp
        simulatormsfsconfig.h
        simulatormsfsconfig.json
)

set_target_properties(simulatormsfsconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatormsfsconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatormsfsconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatormsfsconfig
        PUBLIC
        gui
        misc
        Qt::Core
        PRIVATE
        fsxcommon
)

install(TARGETS simulatormsfsconfig
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

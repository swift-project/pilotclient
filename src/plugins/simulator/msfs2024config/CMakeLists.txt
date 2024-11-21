# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatormsfs2024config SHARED
        simulatormsfs2024config.cpp
        simulatormsfs2024config.h
        simulatormsfs2024config.json
)

set_target_properties(simulatormsfs2024config PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatormsfs2024config PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatormsfs2024config PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatormsfs2024config
        PUBLIC
        gui
        misc
        Qt::Core
        PRIVATE
        fsxcommon
)

install(TARGETS simulatormsfs2024config
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatormsfs2024 SHARED
        simulatormsfs2024.cpp
        simulatormsfs2024.h
        simulatormsfs2024.json
        simulatormsfs2024factory.cpp
        simulatormsfs2024factory.h
)

set_target_properties(simulatormsfs2024 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatormsfs2024 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatormsfs2024 PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatormsfs2024
        PUBLIC
        fscommon
        fsxcommon
        core
        misc
        Qt::Core
)

install(TARGETS simulatormsfs2024
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
        )

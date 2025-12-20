# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorfsxconfig SHARED
        simulatorfsxconfig.cpp
        simulatorfsxconfig.h
        simulatorfsxconfig.json
)

set_target_properties(simulatorfsxconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorfsxconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

force_asserts(simulatorfsxconfig)
target_include_directories(simulatorfsxconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorfsxconfig
        PUBLIC
        gui
        misc
        Qt::Core
        PRIVATE
        fsxcommon
)

install(TARGETS simulatorfsxconfig
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

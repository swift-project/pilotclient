# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorfsx SHARED
        simulatorfsx.cpp
        simulatorfsx.h
        simulatorfsx.json
        simulatorfsxfactory.cpp
        simulatorfsxfactory.h
)

set_target_properties(simulatorfsx PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorfsx PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorfsx PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorfsx
        PUBLIC
        fscommon
        fsxcommon
        core
        misc
        Qt::Core
)

install(TARGETS simulatorfsx
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

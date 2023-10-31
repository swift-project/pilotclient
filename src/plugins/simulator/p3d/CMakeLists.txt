# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorp3d SHARED
        simulatorp3d.h
        simulatorp3d.cpp
        simulatorp3d.json
        simulatorp3dfactory.cpp
        simulatorp3dfactory.h
)

set_target_properties(simulatorp3d PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorp3d PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorp3d PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorp3d
        PUBLIC
        fscommon
        fsxcommon
        core
        misc
        Qt::Core
)

install(TARGETS simulatorp3d
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorp3dconfig SHARED
        simulatorp3dconfig.cpp
        simulatorp3dconfig.h
        simulatorp3dconfig.json
)

set_target_properties(simulatorp3dconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorp3dconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorp3dconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorp3dconfig
        PUBLIC
        gui
        misc
        Qt::Core
        PRIVATE
        fsxcommon
)

install(TARGETS simulatorp3dconfig
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

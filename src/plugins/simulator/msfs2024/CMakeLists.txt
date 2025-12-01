# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatormsfs2024 SHARED
        simulatormsfs2024.cpp
        simulatormsfs2024.h
        simulatormsfs2024.json
        simulatormsfs2024factory.cpp
        simulatormsfs2024factory.h
        simconnectsymbolsmsfs2024.cpp
        simconnectsymbolsmsfs2024.h
        simulatormsfs2024common.cpp
        simulatormsfs2024common.h
        simconnectobjectmsfs2024.cpp
        simconnectobjectmsfs2024.h
        simconnectdatadefinitionmsfs2024.cpp 
        simconnectdatadefinitionmsfs2024.h
        simulatormsfs2024simconnectproc.cpp
        msfs2024export.h
)

set_target_properties(simulatormsfs2024 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatormsfs2024 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatormsfs2024 PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(simulatormsfs2024 PRIVATE BUILD_MSFS2024_LIB)

target_link_libraries(simulatormsfs2024
        PUBLIC
        fscommon
        core
        misc
        Qt::Core
)

install(TARGETS simulatormsfs2024
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
        )

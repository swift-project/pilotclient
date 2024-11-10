# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorfs9 SHARED
        callbackwrapper.h
        directplayerror.cpp
        directplayerror.h
        directplaypeer.cpp
        directplaypeer.h
        directplayutils.h
        fs9.h
        fs9client.cpp
        fs9client.h
        fs9host.cpp
        fs9host.h
        hostnode.cpp
        hostnode.h
        lobbyclient.cpp
        lobbyclient.h
        multiplayerpacketparser.cpp
        multiplayerpacketparser.h
        multiplayerpackets.h
        registermetadata.cpp
        registermetadata.h
        simulatorfs9.cpp
        simulatorfs9.h
        simulatorfs9.json
)

set_target_properties(simulatorfs9 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorfs9 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorfs9 PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorfs9
        PUBLIC
        Qt::Core
        uuid
        ole32
        misc
        core
        fscommon
        config
        externals_dplay
)

install(TARGETS simulatorfs9
        LIBRARY DESTINATION bin/plugins/simulator
        RUNTIME DESTINATION bin/plugins/simulator
)

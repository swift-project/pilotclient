# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorflightgear SHARED
        fgswiftbusserviceproxy.cpp
        fgswiftbusserviceproxy.h
        fgswiftbustrafficproxy.cpp
        fgswiftbustrafficproxy.h
        flightgearmpaircraft.cpp
        flightgearmpaircraft.h
        simulatorflightgear.cpp
        simulatorflightgear.h
        )

set_target_properties(simulatorflightgear PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorflightgear PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorflightgear PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorflightgear
        PUBLIC
        misc
        dbus-1
        plugincommon
        )

install(TARGETS simulatorflightgear
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

if(APPLE)
        set_target_properties(simulatorflightgear PROPERTIES INSTALL_RPATH @loader_path/../../../lib)
elseif(UNIX)
        set_target_properties(simulatorflightgear PROPERTIES INSTALL_RPATH \$ORIGIN/../../../lib)
endif()

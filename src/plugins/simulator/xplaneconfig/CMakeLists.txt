# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatorxplaneconfig SHARED
        simulatorxplaneconfig.cpp
        simulatorxplaneconfig.h
        simulatorxplaneconfigwindow.cpp
        simulatorxplaneconfigwindow.h
        simulatorxplaneconfigwindow.ui
)

set_target_properties(simulatorxplaneconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorxplaneconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorxplaneconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorxplaneconfig
        PUBLIC
        gui
        misc
        Qt::Core
)

install(TARGETS simulatorxplaneconfig
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

if(APPLE)
    set_target_properties(simulatorxplaneconfig PROPERTIES INSTALL_RPATH @loader_path/../../../lib)
elseif(UNIX)
    set_target_properties(simulatorxplaneconfig PROPERTIES INSTALL_RPATH \$ORIGIN/../../../lib)
endif()

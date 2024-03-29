# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatoremulatedconfig SHARED
        simulatoremulatedconfig.cpp
        simulatoremulatedconfig.h
        simulatoremulatedconfigwidget.cpp
        simulatoremulatedconfigwidget.h
        simulatoremulatedconfigwidget.ui
        simulatoremulatedconfig.json
)

set_target_properties(simulatoremulatedconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatoremulatedconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatoremulatedconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatoremulatedconfig
        PUBLIC
        gui
)

install(TARGETS simulatoremulatedconfig
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

if(APPLE)
    set_target_properties(simulatoremulatedconfig PROPERTIES INSTALL_RPATH @loader_path/../../../lib)
elseif(UNIX)
    set_target_properties(simulatoremulatedconfig PROPERTIES INSTALL_RPATH \$ORIGIN/../../../lib)
endif()

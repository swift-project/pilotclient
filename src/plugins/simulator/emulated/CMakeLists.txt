# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(simulatoremulated SHARED
        simulatoremulated.cpp
        simulatoremulated.h
        simulatoremulatedfactory.cpp
        simulatoremulatedfactory.h
        simulatoremulatedmonitordialog.cpp
        simulatoremulatedmonitordialog.h
        simulatoremulatedmonitordialog.ui
        simulatoremulated.json
)

set_target_properties(simulatoremulated PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatoremulated PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

force_asserts(simulatoremulated)
target_include_directories(simulatoremulated PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatoremulated
        PUBLIC
        Qt::Core
        misc
        core
        Qt::Widgets
        plugincommon
        PRIVATE
        gui
)


install(TARGETS simulatoremulated
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

if(APPLE)
    set_target_properties(simulatoremulated PROPERTIES INSTALL_RPATH @loader_path/../../../lib)
elseif(UNIX)
    set_target_properties(simulatoremulated PROPERTIES INSTALL_RPATH \$ORIGIN/../../../lib)
endif()

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(fsxcommon SHARED
        simconnectsymbols.cpp
        simconnectsymbols.h
        fsxsettingscomponent.cpp
        fsxsettingscomponent.h
        fsxsettingscomponent.ui
        simconnectdatadefinition.cpp
        simconnectdatadefinition.h
        simconnectobject.cpp
        simconnectobject.h
        simconnectsettingscomponent.cpp
        simconnectsettingscomponent.h
        simconnectsettingscomponent.ui
        simconnectwindows.h
        simulatorfsxcommon.cpp
        simulatorfsxcommon.h
        simulatorfsxconfigwindow.cpp
        simulatorfsxconfigwindow.h
        simulatorfsxconfigwindow.ui
        simulatorfsxsimconnectproc.cpp
        simconnect.rc
        #SimConnect_RTM.manifest
        #SimConnect_SP1.manifest
        #SimConnect_XPack.manifest
        fsxcommonexport.h
)

set_target_properties(fsxcommon PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
set_target_properties(fsxcommon PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)

target_link_libraries(fsxcommon
        PUBLIC
            Qt::Core
            misc
            Qt::Widgets
            externals_simconnect
        PRIVATE
            gui
            fscommon
            config
)

force_asserts(fsxcommon)
target_compile_definitions(fsxcommon PRIVATE BUILD_FSXCOMMON_LIB)

install(TARGETS fsxcommon
        LIBRARY DESTINATION bin
        RUNTIME DESTINATION bin
)

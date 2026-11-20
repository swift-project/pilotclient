# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

set(FSXCOMMON_SOURCES
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
# Creates a dedicated 'fsxcommon' target library for a specific SimConnect SDK interface target,
# ensuring that each plugin (P3D/MSFS/MSFS2024) is compiled against the appropriate SimConnect.h.
function(add_fsxcommon_variant TARGET_NAME SIMCONNECT_INTERFACE)
    add_library(${TARGET_NAME} SHARED ${FSXCOMMON_SOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
    set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)

    target_link_libraries(${TARGET_NAME}
            PUBLIC
                Qt::Core
                misc
                Qt::Widgets
                ${SIMCONNECT_INTERFACE}
            PRIVATE
                gui
                fscommon
                config
    )

    force_asserts(${TARGET_NAME})
    target_compile_definitions(${TARGET_NAME} PRIVATE BUILD_FSXCOMMON_LIB)

    install(TARGETS ${TARGET_NAME}
            LIBRARY DESTINATION bin
            RUNTIME DESTINATION bin
    )
endfunction()

if(SWIFT_BUILD_P3D_PLUGIN)
    add_fsxcommon_variant(fsxcommon_p3d externals_simconnect_p3d)
endif()

if(SWIFT_BUILD_MSFS_PLUGIN)
    add_fsxcommon_variant(fsxcommon_msfs externals_simconnect_msfs)
endif()

if(SWIFT_BUILD_MSFS2024_PLUGIN)
    add_fsxcommon_variant(fsxcommon_msfs2024 externals_simconnect_msfs2024)
endif()

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_subdirectory(plugincommon)

if(SWIFT_BUILD_EMULATED_PLUGIN)
    add_subdirectory(emulated)
    add_subdirectory(emulatedconfig)
endif()

if(SWIFT_BUILD_FLIGHTGEAR_PLUGIN)
    add_subdirectory(flightgear)
    add_subdirectory(flightgearconfig)
endif()

if(SWIFT_BUILD_XPLANE_PLUGIN)
    add_subdirectory(xplane)
    add_subdirectory(xplaneconfig)
endif()

if(SWIFT_BUILD_FS9_PLUGIN OR SWIFT_BUILD_FSX_PLUGIN OR SWIFT_BUILD_P3D_PLUGIN OR SWIFT_BUILD_MSFS_PLUGIN OR SWIFT_BUILD_MSFS2024_PLUGIN)
    add_subdirectory(fscommon)
endif()

if(SWIFT_BUILD_FSX_PLUGIN OR SWIFT_BUILD_P3D_PLUGIN OR SWIFT_BUILD_MSFS_PLUGIN OR SWIFT_BUILD_MSFS2024_PLUGIN)
    add_subdirectory(fsxcommon)
endif()

if(SWIFT_WIN32 AND (SWIFT_BUILD_FS9_PLUGIN OR SWIFT_BUILD_FSX_PLUGIN OR SWIFT_BUILD_P3D_PLUGIN))
    add_subdirectory(fsuipc32)
endif()

if(SWIFT_WIN64 AND (SWIFT_BUILD_P3D_PLUGIN OR SWIFT_BUILD_MSFS_PLUGIN OR SWIFT_BUILD_MSFS2024_PLUGIN))
    add_subdirectory(fsuipc64)
endif()

if(SWIFT_BUILD_FS9_PLUGIN)
    add_subdirectory(fs9)
endif()

if(SWIFT_BUILD_P3D_PLUGIN)
    add_subdirectory(p3d)
    add_subdirectory(p3dconfig)
endif()

if(SWIFT_BUILD_MSFS_PLUGIN)
    add_subdirectory(msfs)
    add_subdirectory(msfsconfig)
endif()

if(SWIFT_BUILD_MSFS2024_PLUGIN)
    add_subdirectory(msfs2024)
    add_subdirectory(msfs2024config)
endif()

if(SWIFT_BUILD_FSX_PLUGIN)
    add_subdirectory(fsx)
    add_subdirectory(fsxconfig)
endif()

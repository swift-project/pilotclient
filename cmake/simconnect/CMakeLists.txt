# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# Checks if variable for SDK path is set and SimConnect.h does exist in the include path.
# If not, disable building the simulator plugin and emit warning message

if(SWIFT_BUILD_P3D_PLUGIN)
    add_library(externals_simconnect_p3d INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(externals_simconnect_p3d INTERFACE SIMCONNECT_H_NOMANIFEST)
    target_compile_definitions(externals_simconnect_p3d INTERFACE BUID_SIMCONNECT_LIB)
    target_include_directories(externals_simconnect_p3d INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/simconnect/P3D-v4)
    target_compile_definitions(externals_simconnect_p3d INTERFACE P3D_SDK_VERSION=440)
endif()

if(SWIFT_BUILD_MSFS_PLUGIN)
    add_library(externals_simconnect_msfs INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(externals_simconnect_msfs INTERFACE SIMCONNECT_H_NOMANIFEST)
    target_compile_definitions(externals_simconnect_msfs INTERFACE BUID_SIMCONNECT_LIB)
    target_include_directories(externals_simconnect_msfs INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/simconnect/MSFS)
endif()

if(SWIFT_BUILD_MSFS2024_PLUGIN)
    add_library(externals_simconnect_msfs2024 INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(externals_simconnect_msfs2024 INTERFACE SIMCONNECT_H_NOMANIFEST)
    target_compile_definitions(externals_simconnect_msfs2024 INTERFACE BUID_SIMCONNECT_LIB)
    target_include_directories(externals_simconnect_msfs2024 INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/simconnect/MSFS2024)
endif()

if(SWIFT_BUILD_P3D_PLUGIN OR SWIFT_BUILD_MSFS_PLUGIN OR SWIFT_BUILD_MSFS2024_PLUGIN)
    add_custom_target(external_simconnects_dlls)

    macro(add_install_command LIB)
        add_custom_command(TARGET external_simconnects_dlls
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB} ${PROJECT_BINARY_DIR}/out/bin/
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        install(FILES ${LIB} DESTINATION bin)
    endmacro()

    list(APPEND LIBS SimConnect.MSFS.dll)
    list(APPEND LIBS SimConnect.MSFS2024.dll)
    list(APPEND LIBS SimConnect.P3D-v4.0.dll)
    list(APPEND LIBS SimConnect.P3D-v4.1.dll)
    list(APPEND LIBS SimConnect.P3D-v4.2.dll)
    list(APPEND LIBS SimConnect.P3D-v4.3.dll)
    list(APPEND LIBS SimConnect.P3D-v4.4.dll)
    list(APPEND LIBS SimConnect.P3D-v4.5.dll)

    foreach (LIB IN LISTS LIBS)
        add_install_command( ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/${LIB})
    endforeach()

    if(SWIFT_BUILD_P3D_PLUGIN)
        add_dependencies(externals_simconnect_p3d external_simconnects_dlls)
    endif()
    if(SWIFT_BUILD_MSFS_PLUGIN)
        add_dependencies(externals_simconnect_msfs external_simconnects_dlls)
    endif()
    if(SWIFT_BUILD_MSFS2024_PLUGIN)
        add_dependencies(externals_simconnect_msfs2024 external_simconnects_dlls)
    endif()
endif()

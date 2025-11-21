# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# Checks if variable for SDK path is set and SimConnect.h does exist in the include path.
# If not, disable building the simulator plugin and emit warning message

if(SWIFT_BUILD_FSX_PLUGIN OR SWIFT_BUILD_P3D_PLUGIN OR SWIFT_BUILD_MSFS2020_PLUGIN OR SWIFT_BUILD_MSFS2024_PLUGIN)
    add_library(externals_simconnect INTERFACE IMPORTED GLOBAL)
    target_compile_definitions(externals_simconnect INTERFACE SIMCONNECT_H_NOMANIFEST)
    target_compile_definitions(externals_simconnect INTERFACE BUID_SIMCONNECT_LIB)
    if(SWIFT_WIN64)
        target_include_directories(externals_simconnect INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/simconnect/P3D-v4)
        target_compile_definitions(externals_simconnect INTERFACE P3D_SDK_VERSION=400)
    elseif(SWIFT_WIN32)
        target_include_directories(externals_simconnect INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include/simconnect/FSX-XPack)
    endif()

    add_custom_target(external_simconnects_dlls)
    add_dependencies(externals_simconnect external_simconnects_dlls)

    macro(add_install_command LIB)
        add_custom_command(TARGET external_simconnects_dlls
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIB} ${PROJECT_BINARY_DIR}/out/bin/
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
        install(FILES ${LIB} DESTINATION bin)
    endmacro()

    if(SWIFT_WIN32)
        add_install_command(${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/SimConnect.dll)
    elseif(SWIFT_WIN64)
        list(APPEND LIBS SimConnect.MSFS.dll)
        list(APPEND LIBS SimConnect.MSFS2024.dll)
        list(APPEND LIBS SimConnect.P3D-v4.0.dll)
        list(APPEND LIBS SimConnect.P3D-v4.1.dll)
        list(APPEND LIBS SimConnect.P3D-v4.2.dll)
        list(APPEND LIBS SimConnect.P3D-v4.3.dll)
        list(APPEND LIBS SimConnect.P3D-v4.3.dll)
        list(APPEND LIBS SimConnect.P3D-v4.5.dll)

        foreach (LIB IN LISTS LIBS)
            add_install_command( ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/${LIB})
        endforeach()
    endif()
endif()

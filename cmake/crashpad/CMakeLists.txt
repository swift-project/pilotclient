# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_crashpad INTERFACE)
target_include_directories(externals_crashpad INTERFACE
    ${PROJECT_SOURCE_DIR}/externals/common/include/crashpad
    ${PROJECT_SOURCE_DIR}/externals/common/include/crashpad/mini_chromium
)

if(SWIFT_WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        target_link_libraries(externals_crashpad INTERFACE
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/based.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/clientd.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/utild.lib
        )
    else()
        target_link_libraries(externals_crashpad INTERFACE
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/base.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/client.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/util.lib
        )
    endif()
elseif(SWIFT_WIN64)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        target_link_libraries(externals_crashpad INTERFACE
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/based.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/clientd.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/utild.lib
        )
    else()
        target_link_libraries(externals_crashpad INTERFACE
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/base.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/client.lib
            ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/util.lib
        )
    endif()
elseif(APPLE)
    target_link_libraries(externals_crashpad INTERFACE
            ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libbase.a
            ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libclient.a
            ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libutil.a
            )
else()
    target_link_libraries(externals_crashpad INTERFACE
        ${PROJECT_SOURCE_DIR}/externals/linux-g++/64/lib/libbase.a
        ${PROJECT_SOURCE_DIR}/externals/linux-g++/64/lib/libclient.a
        ${PROJECT_SOURCE_DIR}/externals/linux-g++/64/lib/libutil.a
    )
endif()

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_event SHARED IMPORTED GLOBAL)

target_include_directories(externals_event INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)

if(SWIFT_WIN64)
    target_include_directories(externals_event INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/include)

    set_target_properties(externals_event PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/event_core.lib)
    set_target_properties(externals_event PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/event_core.dll)
elseif(APPLE)
    target_include_directories(externals_event INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/include)
    set_target_properties(externals_event PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/libevent_core.2.1.8.dylib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(libevent_core REQUIRED libevent_core)

    # Adapted from CMake file() documentation to resolve symbolic link
    file(READ_SYMLINK ${libevent_core_LINK_LIBRARIES} LIBEVENT_CORE_ABS)
    if(NOT IS_ABSOLUTE "${LIBEVENT_CORE_ABS}")
        get_filename_component(dir "${libevent_core_LINK_LIBRARIES}" DIRECTORY)
        set(LIBEVENT_CORE_ABS "${dir}/${LIBEVENT_CORE_ABS}")
    endif()

    set_target_properties(externals_event PROPERTIES IMPORTED_LOCATION ${LIBEVENT_CORE_ABS})
endif()

install(IMPORTED_RUNTIME_ARTIFACTS externals_event DESTINATION xswiftbus/64 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

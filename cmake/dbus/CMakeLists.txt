# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_dbus SHARED IMPORTED GLOBAL)


if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(DBUS REQUIRED dbus-1)
    target_include_directories(externals_dbus INTERFACE ${DBUS_INCLUDE_DIRS})
    set_target_properties(externals_dbus PROPERTIES IMPORTED_LOCATION ${DBUS_LINK_LIBRARIES})
elseif(SWIFT_WIN32)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/include)

    set_target_properties(externals_dbus PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/dbus-1.lib)
    set_target_properties(externals_dbus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/dbus-1-3.dll)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION xswiftbus/64)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/bin/dbus-daemon.exe DESTINATION bin)
elseif(SWIFT_WIN64)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/include)

    set_target_properties(externals_dbus PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/dbus-1.lib)
    set_target_properties(externals_dbus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/dbus-1-3.dll)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION xswiftbus/64)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/bin/dbus-daemon.exe DESTINATION bin)
elseif(APPLE)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)
    target_include_directories(externals_dbus INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/include/)

    set_target_properties(externals_dbus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/libdbus-1.3.dylib)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION xswiftbus/64 PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_dbus DESTINATION lib PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
    install(FILES ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/bin/dbus-daemon DESTINATION bin)
    install(FILES ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/bin/dbus-uuidgen DESTINATION bin)
endif()

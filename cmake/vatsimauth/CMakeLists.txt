# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_vatsim_auth SHARED IMPORTED GLOBAL)

target_include_directories(externals_vatsim_auth INTERFACE ${PROJECT_SOURCE_DIR}/externals/common/include/)

if(UNIX AND NOT APPLE)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/linux-g++/64/lib/libvatsimauth.so)
elseif(APPLE)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libvatsimauth.dylib)
elseif(SWIFT_WIN32)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/vatsimauth.lib)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/vatsimauth.dll)
elseif(SWIFT_WIN64)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/vatsimauth.lib)
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/vatsimauth.dll)
endif()

if(UNIX AND NOT APPLE)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/19860#note_642079
    set_target_properties(externals_vatsim_auth PROPERTIES IMPORTED_NO_SONAME TRUE)
endif()

install(IMPORTED_RUNTIME_ARTIFACTS externals_vatsim_auth
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

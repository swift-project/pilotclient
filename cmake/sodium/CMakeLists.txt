# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_library(externals_sodium SHARED IMPORTED GLOBAL)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(SODIUM REQUIRED libsodium)

    file(READ_SYMLINK ${SODIUM_LINK_LIBRARIES} SODIUM_ABS)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_SONAME ${SODIUM_ABS})

    string(REGEX MATCH "libsodium.so.[1-9]+" SOMAJOR_VERSION ${SODIUM_ABS})
    get_filename_component(dir "${SODIUM_LINK_LIBRARIES}" DIRECTORY)
    set(SOMAJOR_VERSION_ABS "${dir}/${SOMAJOR_VERSION}")
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${SOMAJOR_VERSION_ABS})
else()
    target_include_directories(externals_sodium INTERFACE ${PROJECT_SOURCE_DIR}/third_party/externals/common/include)
endif()

if(APPLE)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/macx-clang/64/lib/libsodium.23.dylib)
elseif(SWIFT_WIN32)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/sodium.lib)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/32/lib/libsodium.dll)
elseif(SWIFT_WIN64)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/sodium.lib)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/third_party/externals/win32-msvc/64/lib/libsodium.dll)
endif()

install(IMPORTED_RUNTIME_ARTIFACTS externals_sodium
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

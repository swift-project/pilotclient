add_library(externals_sodium SHARED IMPORTED GLOBAL)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(SODIUM REQUIRED libsodium)

    # Adapted from CMake file() documentation to resolve symbolic link
    file(READ_SYMLINK ${SODIUM_LINK_LIBRARIES} SODIUM_ABS)
    if(NOT IS_ABSOLUTE "${SODIUM_ABS}")
        get_filename_component(dir "${SODIUM_LINK_LIBRARIES}" DIRECTORY)
        set(SODIUM_ABS "${dir}/${SODIUM_ABS}")
    endif()

    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${SODIUM_ABS})
else()
    target_include_directories(externals_sodium INTERFACE ${PROJECT_SOURCE_DIR}/externals/common/include)
endif()

if(APPLE)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libsodium.23.dylib)
elseif(SWIFT_WIN32)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/sodium.lib)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/libsodium.dll)
elseif(SWIFT_WIN64)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/sodium.lib)
    set_target_properties(externals_sodium PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/libsodium.dll)
endif()

install(IMPORTED_RUNTIME_ARTIFACTS externals_sodium
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

add_library(externals_opus SHARED IMPORTED GLOBAL)


if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(OPUS REQUIRED opus)
    set_target_properties(externals_opus PROPERTIES IMPORTED_LOCATION ${OPUS_LINK_LIBRARIES})
else()
    target_include_directories(externals_opus INTERFACE ${PROJECT_SOURCE_DIR}/externals/common/include)
endif()

if(APPLE)
    set_target_properties(externals_opus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/macx-clang/64/lib/libopus.0.dylib)
endif()

if(SWIFT_WIN32)
    set_target_properties(externals_opus PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/opus.lib)
    set_target_properties(externals_opus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/32/lib/opus.dll)
elseif(SWIFT_WIN64)
    set_target_properties(externals_opus PROPERTIES IMPORTED_IMPLIB ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/opus.lib)
    set_target_properties(externals_opus PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/externals/win32-msvc/64/lib/opus.dll)
endif()

if(WIN32 OR APPLE)
    install(IMPORTED_RUNTIME_ARTIFACTS externals_opus
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

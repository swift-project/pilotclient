# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

cmake_policy(VERSION 3.21)

cmake_minimum_required(VERSION 3.26)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif()

set(SWIFT_LANGUAGES C CXX)
if(APPLE)
    list(APPEND SWIFT_LANGUAGES OBJC OBJCXX)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

project(swift LANGUAGES ${SWIFT_LANGUAGES})

include(cmake/tools.cmake)
include(cmake/pch_config.cmake)
include(CMakeDependentOption)
get_version_number()
configure_target_platform()

# CMake config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/lib)

# Build related options
option(SWIFT_BUILD_SAMPLES "Build samples" ON)
option(SWIFT_BUILD_UNIT_TESTS "Build unit tests" ON)
option(SWIFT_BUILD_DOCUMENTATION "Build documentation" ON)

cmake_dependent_option(SWIFT_BUILD_FS9_PLUGIN "Build FS9 plugin" ON SWIFT_WIN32 OFF)
cmake_dependent_option(SWIFT_BUILD_FSX_PLUGIN "Build FSX plugin" ON SWIFT_WIN32 OFF)
cmake_dependent_option(SWIFT_BUILD_P3D_PLUGIN "Build P3D plugin" ON WIN32 OFF)
option(SWIFT_BUILD_XPLANE_PLUGIN "Build X-Plane plugin" ON)
cmake_dependent_option(SWIFT_BUILD_XSWIFTBUS "Build xswiftbus" ON "NOT SWIFT_WIN32" OFF)
option(SWIFT_BUILD_FLIGHTGEAR_PLUGIN "Build Flightgear plugin" ON)
option(SWIFT_BUILD_EMULATED_PLUGIN "Build Emulated plugin" ON)
cmake_dependent_option(SWIFT_BUILD_MSFS_PLUGIN "Build MSFS plugin" ON WIN32 OFF)
cmake_dependent_option(SWIFT_BUILD_MSFS2024_PLUGIN "Build MSFS2024 plugin" ON WIN32 OFF)
option(SWIFT_MINIFY_DEBUG_SYMBOLS "Minify debug symbols" OFF)

option(SWIFT_USE_CRASHPAD "Use crashpad" OFF)

# VATSIM related options
option(SWIFT_VATSIM_SUPPORT "Build with VATSIM support" ON)
set(VATSIM_KEY_JSON "" CACHE STRING "Path to JSON containing VATSIM key")
load_vatsim_key()
if(SWIFT_VATSIM_SUPPORT)
    add_compile_definitions(SWIFT_VATSIM_SUPPORT)
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not defined. Falling back to Debug")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(SWIFT_MINIFY_DEBUG_SYMBOLS)
    if(MSVC)
        add_link_options(/OPT:REF /OPT:ICF)
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        add_compile_options(-g1 -gz)
    elseif(${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
        add_compile_options(-gline-tables-only)
    endif()
endif()

if(MSVC)
    # This also installs some dlls which we do not need
    # but our script to create the installer does not pack them.
    # Further they are removed with cleanup.cmake
    # see https://gitlab.kitware.com/cmake/cmake/-/issues/17725
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION vcredist)
    include(InstallRequiredSystemLibraries)
    if (SWIFT_WIN32)
        install(FILES ${MSVC_REDIST_DIR}/vc_redist.x86.exe DESTINATION vcredist)
    else()
        install(FILES ${MSVC_REDIST_DIR}/vc_redist.x64.exe DESTINATION vcredist)
    endif()
endif()

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Core DBus Gui Multimedia Network OpenGL Concurrent Xml Qml Widgets Svg Test)
find_package(Opus REQUIRED)
find_package(libsodium REQUIRED)
find_package(nlohmann_json REQUIRED)
if(UNIX AND NOT APPLE)
    add_subdirectory(cmake/dbus)
else()
    find_package(DBus1 REQUIRED)
endif()

if(SWIFT_VATSIM_SUPPORT)
    find_package(VATSIMAuth REQUIRED)
endif()

# Global compiler options
if(MSVC)
    add_compile_options(/W3 /w34100 /w34189 /wd4661)

    # https://doc.qt.io/qt-6/qt-disable-unicode-defines.html
    add_compile_definitions(UNICODE)
    add_compile_definitions(_UNICODE)

    add_compile_options(/utf-8)
    add_compile_options(/permissive-)
    add_compile_options(/Zc:lambda)

    # Workaround C1128 error
    add_compile_options(/bigobj)

    # Debug iterators used by Qt 5
    add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    add_compile_options(-Wall -Wextra)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-Woverloaded-virtual;-Wzero-as-null-pointer-constant;-Wsuggest-override>")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES ".*Clang")
    add_compile_options(-Wall -Wextra)
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX,OBJCXX>:-Woverloaded-virtual;-Wzero-as-null-pointer-constant;-Wno-return-std-move>")
endif()

if(SWIFT_USE_CRASHPAD)
    add_compile_definitions(SWIFT_USE_CRASHPAD)
endif()

if(APPLE)
    set(CMAKE_INSTALL_RPATH @loader_path/../../../../lib)
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH \$ORIGIN/../lib)
endif()

if(UNIX AND NOT APPLE AND ${CMAKE_BUILD_TYPE} STREQUAL Debug)
    # For swift::misc::getStackTrace
    add_link_options(-rdynamic)
endif()

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

# Add external folders
add_subdirectory(cmake/simconnect)
add_subdirectory(third_party)


if(SWIFT_BUILD_XSWIFTBUS)
    find_package(XPSDK REQUIRED)
    find_package(Libevent REQUIRED)
endif()

# Add source folders
add_subdirectory(src)
add_subdirectory(resources)

if(SWIFT_BUILD_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SWIFT_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(SWIFT_BUILD_DOCUMENTATION)
    add_custom_target(documentation env DOXY_SRC_ROOT=${swift_SOURCE_DIR} doxygen Doxyfile WORKING_DIRECTORY ${swift_SOURCE_DIR}/docs)
endif()

include(cmake/install.cmake)

if(MSVC)
    install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/cleanup.cmake)
endif()

# Copy externals to build directory to allow running swift and tests
# TODO This should be put into the corresponding targets responsible for these files and may use TARGET_RUNTIME_DLLS
if(WIN32)
    add_custom_target(copy_externals_to_build_dir)
    if(SWIFT_WIN32)
        set(ARCH_DIR 32)
    elseif(SWIFT_WIN64)
            set(ARCH_DIR 64)
    endif()
    add_custom_command(TARGET copy_externals_to_build_dir
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different third_party/externals/win32-msvc/${ARCH_DIR}/bin ${PROJECT_BINARY_DIR}/out/bin
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_command(TARGET copy_externals_to_build_dir
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different third_party/externals/win32-msvc/${ARCH_DIR}/lib ${PROJECT_BINARY_DIR}/out/bin
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

message(STATUS "Simulators:")
message(STATUS "\t FS9: ${SWIFT_BUILD_FS9_PLUGIN}")
message(STATUS "\t FSX: ${SWIFT_BUILD_FSX_PLUGIN}")
message(STATUS "\t P3D: ${SWIFT_BUILD_P3D_PLUGIN}")
message(STATUS "\t MSFS: ${SWIFT_BUILD_MSFS_PLUGIN}")
message(STATUS "\t MSFS2024: ${SWIFT_BUILD_MSFS2024_PLUGIN}")
message(STATUS "\t XPLANE: ${SWIFT_BUILD_XPLANE_PLUGIN}")
message(STATUS "\t XSWIFTBUS: ${SWIFT_BUILD_XPLANE_PLUGIN}")
message(STATUS "\t FLIGHTGEAR: ${SWIFT_BUILD_FLIGHTGEAR_PLUGIN}")
message(STATUS "\t EMULATED: ${SWIFT_BUILD_EMULATED_PLUGIN}")

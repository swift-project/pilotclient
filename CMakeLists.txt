cmake_policy(VERSION 3.21)

cmake_minimum_required(VERSION 3.21)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

project(swift)

include(cmake/tools.cmake)
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

option(SWIFT_BUILD_FS9_PLUGIN "Build FS9 plugin" ON)
option(SWIFT_BUILD_FSX_PLUGIN "Build FSX plugin" ON)
option(SWIFT_BUILD_P3D_PLUGIN "Build P3D plugin" ON)
option(SWIFT_BUILD_FSUIPC_PLUGIN "Build FSUIPC plugin" ON)
option(SWIFT_BUILD_XPLANE_PLUGIN "Build X-Plane plugin" ON)
option(SWIFT_BUILD_XSWIFTBUS "Build xswiftbus" ON)
option(SWIFT_BUILD_FLIGHTGEAR_PLUGIN "Build Flightgear plugin" ON)
option(SWIFT_BUILD_MSFS_PLUGIN "Build MSFS plugin" ON)

option(SWIFT_BUILD_BLACKCORE "Build Blackcore" ON)
option(SWIFT_BUILD_BLACKSOUND "Build Blacksound" ON)
option(SWIFT_BUILD_BLACKINPUT "Build Blackinput" ON)
option(SWIFT_BUILD_BLACKGUI "Build Blackgui" ON)

option(SWIFT_USE_CRASHPAD "Use crashpad" OFF)

# VATSIM related options
option(SWIFT_VATSIM_SUPPORT "Build with VATSIM support" ON)
set(VATSIM_KEY_JSON "" CACHE STRING "Path to JSON containing VATSIM key")
load_vatsim_key()

if(SWIFT_WIN32)
    message(WARNING "xswiftbus disabled with 32-bit build!")
    set(SWIFT_BUILD_XSWIFTBUS OFF)
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
find_package(Qt5 REQUIRED COMPONENTS Core DBus Gui Multimedia Xml Qml Widgets Svg Test)

# Global compiler options
if(MSVC)
    add_compile_options(/wd4351 /wd4661)

    # https://doc.qt.io/qt-6/qt-disable-unicode-defines.html
    add_compile_definitions(UNICODE)
    add_compile_definitions(_UNICODE)

    add_compile_options(/permissive-)
    add_compile_options(/Zc:lambda)
    add_compile_options(/FI${PROJECT_SOURCE_DIR}/src/permissiveworkarounds.h)
    add_compile_options(/bigobj)
endif()

if(UNIX AND NOT APPLE)
    add_compile_options("-Wno-deprecated")
endif()

if(UNIX)
    add_compile_options("-Wno-deprecated-declarations")
endif()

if(SWIFT_USE_CRASHPAD)
    add_compile_definitions(BLACK_USE_CRASHPAD)
endif()

if(APPLE)
    set(CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    set(CMAKE_INSTALL_RPATH \$ORIGIN/../lib)
endif()

# Add external folders
add_subdirectory(cmake/vatsimauth)
add_subdirectory(cmake/qwt_plot)
add_subdirectory(cmake/dbus)
add_subdirectory(cmake/sodium)
add_subdirectory(cmake/msgpack)
add_subdirectory(cmake/rapidjson)
add_subdirectory(cmake/opus)
add_subdirectory(cmake/crashpad)
add_subdirectory(cmake/tls)

if(SWIFT_BUILD_XSWIFTBUS)
    add_subdirectory(cmake/xp)
    add_subdirectory(cmake/event)
endif()

# Add source folders
add_subdirectory(src)
add_subdirectory(resources)

if(SWIFT_BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

if(SWIFT_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

if(SWIFT_BUILD_DOCUMENTATION)
    add_custom_target(documentation env DOXY_SRC_ROOT=${swift_SOURCE_DIR} doxygen Doxyfile.qmake WORKING_DIRECTORY ${swift_SOURCE_DIR}/docs)
endif()

include(cmake/install.cmake)

if(MSVC)
    install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/cleanup.cmake)
endif()

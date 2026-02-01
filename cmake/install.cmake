# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# Readme, License etc.
set(GENERAL_FILES
        ${swift_SOURCE_DIR}/LICENSES/LicenseRef-swift-pilot-client-1.txt
        )
install(FILES ${GENERAL_FILES} DESTINATION bin)

# Crashpad
if(UNIX AND NOT APPLE)
    set(crashpad_handler_path ${swift_SOURCE_DIR}/third_party/externals/linux-g++/64/bin/swift_crashpad_handler)
elseif(SWIFT_WIN32)
    set(crashpad_handler_path ${swift_SOURCE_DIR}/third_party/externals/win32-msvc/32/bin/swift_crashpad_handler.exe)
elseif(SWIFT_WIN64)
    set(crashpad_handler_path ${swift_SOURCE_DIR}/third_party/externals/win32-msvc/64/bin/swift_crashpad_handler.exe)
elseif(APPLE)
    set(crashpad_handler_path ${swift_SOURCE_DIR}/third_party/externals/macx-clang/64/bin/swift_crashpad_handler)
endif()
install(PROGRAMS ${crashpad_handler_path} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

# Deploy qt libs

# Workaround to get Qt paths for deployment (until switching to Qt6 deployment tool)
if(UNIX AND NOT APPLE)
    execute_process(COMMAND qmake6 -query QT_INSTALL_LIBS OUTPUT_VARIABLE QT_INSTALL_LIBS)
    execute_process(COMMAND qmake6 -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE QT_INSTALL_PLUGINS)

    # Avoid line breaks
    string(STRIP ${QT_INSTALL_LIBS} QT_INSTALL_LIBS)
    string(STRIP ${QT_INSTALL_PLUGINS} QT_INSTALL_PLUGINS)

    function(installLib path lib)
        # Follow symlink
        file(REAL_PATH ${path}/${lib} followedLib)
        install(FILES ${followedLib} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib RENAME ${lib})
    endfunction()


    installLib(${QT_INSTALL_LIBS} libQt6Core.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6DBus.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Gui.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Multimedia.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6MultimediaWidgets.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Concurrent.so.6) # see https://github.com/swift-project/pilotclient/issues/385
    installLib(${QT_INSTALL_LIBS} libQt6Network.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6OpenGL.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Qml.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Svg.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Widgets.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6XcbQpa.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Xml.so.6)

    macro(resolve_symlink lib_name)
        # Adapted from CMake file() documentation
        file(READ_SYMLINK ${ICU_${lib_name}_LIBRARIES} ICU_${lib_name}_ABS)

        if(NOT IS_ABSOLUTE "${ICU_${lib_name}_ABS}")
          get_filename_component(dir "${ICU_${lib_name}_LIBRARIES}" DIRECTORY)
          set(ICU_${lib_name}_ABS "${dir}/${ICU_${lib_name}_ABS}")
        endif()
    endmacro()


    if(EXISTS ${QT_INSTALL_LIBS}/libicui18n.so.73 AND EXISTS ${QT_INSTALL_LIBS}/libicuuc.so.73 AND EXISTS ${QT_INSTALL_LIBS}/libicudata.so.73)
        # Use specific requested version that is found in the Qt library dir (at least in the Github Actions environment)
        message(STATUS "Install specific libicu version from Qt dir")
        installLib(${QT_INSTALL_LIBS} libicui18n.so.73)
        installLib(${QT_INSTALL_LIBS} libicuuc.so.73)
        installLib(${QT_INSTALL_LIBS} libicudata.so.73)
    else()
        message(STATUS "Using systems libicu library")
        # This might not work for creating the installer as the version can differ
        find_package(ICU REQUIRED COMPONENTS i18n uc data)
        resolve_symlink(I18N)
        resolve_symlink(UC)
        resolve_symlink(DATA)
        install(FILES ${ICU_I18N_ABS} ${ICU_UC_ABS} ${ICU_DATA_ABS} TYPE LIB)
    endif()

    file(GLOB iconEnginePlugins ${QT_INSTALL_PLUGINS}/iconengines/*.so)
    install(FILES ${iconEnginePlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/iconengines)

    file(GLOB imageformatsPlugins ${QT_INSTALL_PLUGINS}/imageformats/*.so)
    install(FILES ${imageformatsPlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/imageformats)

    install(FILES ${QT_INSTALL_PLUGINS}/platforms/libqxcb.so DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/platforms)

    file(GLOB platforminputcontextsPlugins ${QT_INSTALL_PLUGINS}/platforminputcontexts/*.so)
    install(FILES ${platforminputcontextsPlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/platforminputcontexts)

    file(GLOB platformthemesPlugins ${QT_INSTALL_PLUGINS}/platformthemes/*.so)
    install(FILES ${platformthemesPlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/platformthemes)

    file(GLOB tlsPlugins ${QT_INSTALL_PLUGINS}/tls/*.so)
    install(FILES ${tlsPlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/tls)

    file(GLOB xcbglintegrationsPlugins ${QT_INSTALL_PLUGINS}/xcbglintegrations/*.so)
    install(FILES ${xcbglintegrationsPlugins} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/xcbglintegrations)

elseif(WIN32)
    install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/qt_deploy_win.cmake)
elseif(APPLE)
    install(SCRIPT ${PROJECT_SOURCE_DIR}/cmake/qt_deploy_mac.cmake)

    # Install plugins
    execute_process(COMMAND qmake6 -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
    string(STRIP ${QT_INSTALL_PLUGINS} QT_INSTALL_PLUGINS)

    install(DIRECTORY ${QT_INSTALL_PLUGINS}/iconengines DESTINATION bin FILES_MATCHING PATTERN "*.dylib")
    install(DIRECTORY ${QT_INSTALL_PLUGINS}/imageformats DESTINATION bin FILES_MATCHING PATTERN "*.dylib")
    install(FILES ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib DESTINATION bin/platforms)
    install(DIRECTORY ${QT_INSTALL_PLUGINS}/platforminputcontexts DESTINATION bin FILES_MATCHING PATTERN "*.dylib")
    install(DIRECTORY ${QT_INSTALL_PLUGINS}/platformthemes DESTINATION bin FILES_MATCHING PATTERN "*.dylib")
    install(DIRECTORY ${QT_INSTALL_PLUGINS}/tls DESTINATION bin FILES_MATCHING PATTERN "*.dylib")

endif()

macro(CheckPathExists PATH LIBS)
    foreach (LIB IN LISTS LIBS)
        if (NOT EXISTS ${PATH}/${LIB})
            message(WARNING "Expected deploy directory ${PATH}/${LIB} not found. Installation will not work!")
        endif ()
    endforeach ()
endmacro()

set(SWIFT_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR})
if(SWIFT_SYSTEM_PROCESSOR STREQUAL "AMD64")
    set(SWIFT_SYSTEM_PROCESSOR "x86_64")
endif()

set(CONAN_DEPLOY_DIR ${PROJECT_SOURCE_DIR}/build_conan/full_deploy/host)
set(OPUS_PATH ${CONAN_DEPLOY_DIR}/opus/1.3.1/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
set(EVENT_PATH ${CONAN_DEPLOY_DIR}/libevent/2.1.12/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
set(SODIUM_PATH ${CONAN_DEPLOY_DIR}/libsodium/1.0.18/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
set(DBUS_PATH ${CONAN_DEPLOY_DIR}/dbus/1.15.8/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
set(EXPAT_PATH ${CONAN_DEPLOY_DIR}/expat/2.7.1/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
set(NLOHMANN_JSON_PATH ${CONAN_DEPLOY_DIR}/nlohmann_json/3.11.3)

CheckPathExists(${SODIUM_PATH} licenses/LICENSE)
install(FILES ${SODIUM_PATH}/licenses/LICENSE DESTINATION licenses RENAME LICENSE.LIBSODIUM.txt)

CheckPathExists(${OPUS_PATH} licenses/COPYING)
install(FILES ${OPUS_PATH}/licenses/COPYING DESTINATION licenses RENAME COPYING.OPUS.txt)

# xswiftbus licenses
if(SWIFT_BUILD_XSWIFTBUS)
    CheckPathExists(${PROJECT_SOURCE_DIR} LICENSES/GPL-3.0-or-later.txt)
    CheckPathExists(${PROJECT_SOURCE_DIR} LICENSES/LicenseRef-swift-pilot-client-1.txt)
    install(FILES ${PROJECT_SOURCE_DIR}/LICENSES/GPL-3.0-or-later.txt DESTINATION xswiftbus/licenses RENAME LICENSE.XSWIFTBUS.GPL-3.0-or-later.txt)
    install(FILES ${PROJECT_SOURCE_DIR}/LICENSES/LicenseRef-swift-pilot-client-1.txt DESTINATION xswiftbus/licenses RENAME LICENSE.XSWIFTBUS.LicenseRef-swift-pilot-client-1.txt.txt)

    CheckPathExists(${EVENT_PATH} licenses/LICENSE)
    install(FILES ${EVENT_PATH}/licenses/LICENSE DESTINATION xswiftbus/licenses RENAME LICENSE.LIBEVENT.txt)

    CheckPathExists(${NLOHMANN_JSON_PATH} licenses/LICENSE.MIT)
    install(FILES ${NLOHMANN_JSON_PATH}/licenses/LICENSE.MIT DESTINATION xswiftbus/licenses RENAME LICENSE.NLOHMANN_JSON.txt)

    if(APPLE OR WIN32)
        CheckPathExists(${DBUS_PATH} licenses/COPYING)
        install(FILES ${DBUS_PATH}/licenses/COPYING DESTINATION xswiftbus/licenses RENAME LICENSE.DBUS.txt)
        install(FILES ${DBUS_PATH}/licenses/AFL-2.1.txt DESTINATION xswiftbus/licenses RENAME LICENSE.DBUS.AFL-2.1.txt)
        install(FILES ${DBUS_PATH}/licenses/GPL-2.0-or-later.txt DESTINATION xswiftbus/licenses RENAME LICENSE.DBUS.GPL-2.0-or-later.txt)

        CheckPathExists(${EXPAT_PATH} licenses/COPYING)
        install(FILES ${EXPAT_PATH}/licenses/COPYING DESTINATION xswiftbus/licenses RENAME LICENSE.EXPAT.txt)
    endif()

    string(REPLACE "\\" "/" XP_SDK_PATH_ESCAPED "${XP_SDK_PATH}")
    CheckPathExists(${XP_SDK_PATH_ESCAPED} license.txt)
    install(FILES ${XP_SDK_PATH_ESCAPED}/license.txt DESTINATION xswiftbus/licenses RENAME LICENSE.XPSDK.txt)

    CheckPathExists(${PROJECT_SOURCE_DIR}/src/xswiftbus/xplanemp2 LICENSE.md)
    install(FILES ${PROJECT_SOURCE_DIR}/src/xswiftbus/xplanemp2/LICENSE.md DESTINATION xswiftbus/licenses RENAME LICENSE.XPLANEMP2.txt)
endif()

# dbus licenses
if(APPLE OR WIN32)
    CheckPathExists(${DBUS_PATH} licenses/COPYING)
    install(FILES ${DBUS_PATH}/licenses/COPYING DESTINATION licenses RENAME LICENSE.DBUS.txt)
    install(FILES ${DBUS_PATH}/licenses/AFL-2.1.txt DESTINATION licenses RENAME LICENSE.DBUS.AFL-2.1.txt)
    install(FILES ${DBUS_PATH}/licenses/GPL-2.0-or-later.txt DESTINATION licenses RENAME LICENSE.DBUS.GPL-2.0-or-later.txt)
endif()

if (UNIX AND NOT APPLE)
    # Opus
    set(OPUS_LIBS libopus.so.0.8.0 libopus.so.0 libopus.so)
    CheckPathExists(${OPUS_PATH}/lib ${OPUS_LIBS})
    foreach (LIB IN LISTS OPUS_LIBS)
        install(FILES ${OPUS_PATH}/lib/${LIB} DESTINATION lib)
    endforeach ()

    # sodium
    set(SODIUM_LIBS libsodium.so.23.3.0 libsodium.so.23 libsodium.so)
    CheckPathExists(${SODIUM_PATH}/lib ${SODIUM_LIBS})
    foreach (LIB IN LISTS SODIUM_LIBS)
        install(FILES ${SODIUM_PATH}/lib/${LIB} DESTINATION lib)
    endforeach ()

    # libevent
    if(SWIFT_BUILD_XSWIFTBUS)
        set(EVENT_LIBS
            libevent_core-2.1.so.7.0.1
            libevent_core-2.1.so.7
            libevent_core-2.1.so
            libevent_core.so
        )
        CheckPathExists(${EVENT_PATH}/lib ${EVENT_LIBS})
        foreach (LIB IN LISTS EVENT_LIBS)
            install(FILES ${EVENT_PATH}/lib/${LIB} DESTINATION xswiftbus/64)
        endforeach ()
    endif()

elseif (APPLE)
    # Opus
    set(OPUS_LIBS libopus.0.8.0.dylib libopus.0.dylib libopus.dylib)
    CheckPathExists(${OPUS_PATH}/lib ${OPUS_LIBS})
    foreach (LIB IN LISTS OPUS_LIBS)
        install(FILES ${OPUS_PATH}/lib/${LIB} DESTINATION lib)
    endforeach ()

    # sodium
    set(SODIUM_LIBS libsodium.23.dylib libsodium.dylib)
    CheckPathExists(${SODIUM_PATH}/lib ${SODIUM_LIBS})
    foreach (LIB IN LISTS SODIUM_LIBS)
        install(FILES ${SODIUM_PATH}/lib/${LIB} DESTINATION lib)
    endforeach ()

    # DBus
    set(CONAN_DEPLOY_DIR_DBUS_SHARED ${PROJECT_SOURCE_DIR}/build_conan_dbus_shared/full_deploy/host)
    set(DBUS_PATH_SHARED ${CONAN_DEPLOY_DIR_DBUS_SHARED}/dbus/1.15.8/${CMAKE_BUILD_TYPE}/${SWIFT_SYSTEM_PROCESSOR})
    set(DBUS_LIBS libdbus-1.3.dylib libdbus-1.dylib)
    CheckPathExists(${DBUS_PATH_SHARED}/lib ${DBUS_LIBS})
    foreach (LIB IN LISTS DBUS_LIBS)
        install(FILES ${DBUS_PATH_SHARED}/lib/${LIB} DESTINATION lib)
    endforeach ()
    install(FILES ${DBUS_PATH_SHARED}/bin/dbus-daemon DESTINATION bin)
    install(FILES ${DBUS_PATH_SHARED}/bin/dbus-uuidgen DESTINATION bin)

elseif (SWIFT_WIN64)
    # Opus
    set(OPUS_LIBS opus.dll)
    CheckPathExists(${OPUS_PATH}/bin ${OPUS_LIBS})
    foreach (LIB IN LISTS OPUS_LIBS)
        install(FILES ${OPUS_PATH}/bin/${LIB} DESTINATION bin)
    endforeach ()

    # sodium
    set(SODIUM_LIBS libsodium.dll)
    CheckPathExists(${SODIUM_PATH}/bin ${SODIUM_LIBS})
    foreach (LIB IN LISTS SODIUM_LIBS)
        install(FILES ${SODIUM_PATH}/bin/${LIB} DESTINATION bin)
    endforeach ()

    # libevent
    if(SWIFT_BUILD_XSWIFTBUS)
        set(EVENT_LIBS event_core.dll)
        CheckPathExists(${EVENT_PATH}/bin ${EVENT_LIBS})
        foreach (LIB IN LISTS EVENT_LIBS)
            install(FILES ${EVENT_PATH}/bin/${LIB} DESTINATION xswiftbus/64)
        endforeach ()
    endif()

    # DBus
    set(DBUS_LIBS dbus-1-3.dll)
    CheckPathExists(${DBUS_PATH}/bin ${DBUS_LIBS})
    foreach (LIB IN LISTS DBUS_LIBS)
        install(FILES ${DBUS_PATH}/bin/${LIB} DESTINATION bin)
        if(SWIFT_BUILD_XSWIFTBUS)
            install(FILES ${DBUS_PATH}/bin/${LIB} DESTINATION xswiftbus/64)
        endif()
    endforeach ()
    install(FILES ${DBUS_PATH}/bin/dbus-daemon.exe DESTINATION bin)
    if(SWIFT_BUILD_XSWIFTBUS)
        install(FILES ${DBUS_PATH}/bin/dbus-daemon.exe DESTINATION xswiftbus/64)
    endif()
endif ()

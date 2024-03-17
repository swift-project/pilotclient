# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

# Readme, License etc.
set(GENERAL_FILES
        ${swift_SOURCE_DIR}/LICENSES/LicenseRef-swift-pilot-client-1.txt
        )
install(FILES ${GENERAL_FILES} DESTINATION bin)

# 7za
if(SWIFT_WIN32)
    install(FILES ${swift_SOURCE_DIR}/third_party/externals/win32-msvc/32/bin/7za.exe DESTINATION bin)
elseif(SWIFT_WIN64)
    install(FILES ${swift_SOURCE_DIR}/third_party/externals/win32-msvc/64/bin/7za.exe DESTINATION bin)
elseif(APPLE)
    install(FILES ${swift_SOURCE_DIR}/third_party/externals/macx-clang/64/bin/7za DESTINATION bin)
endif()

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

# create_dbus_machineid
if(APPLE)
    install(FILES ${swift_SOURCE_DIR}/third_party/externals/macx-clang/64/bin/create_dbus_machineid.scpt DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

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
    installLib(${QT_INSTALL_LIBS} libQt6Core5Compat.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6DBus.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Gui.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6Multimedia.so.6)
    installLib(${QT_INSTALL_LIBS} libQt6MultimediaWidgets.so.6)
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

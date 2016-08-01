############### Readme, License etc. #####

text_files_target.path = $${PREFIX}/bin
text_files_target.files *= README.md
text_files_target.files *= LICENSE

INSTALLS += text_files_target

############### Install Qt5 ##############

win32 {
    QT5_LIBRARIES *= Qt5Core$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Gui$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Network$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5DBus$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Xml$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Multimedia$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Svg$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Widgets$${DLL_DEBUG_SUFFIX}.dll

    qt5_target.path = $${PREFIX}/bin
    QT5_LIBRARY_DIR = $$[QT_INSTALL_BINS]
}
else:macx {
    # OSX workaround using rsync. Otherwise all headers are also copied.
    qt5_target.path = $${PREFIX}/lib/QtCore.framework
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtCore.framework/ $${PREFIX}/lib/QtCore.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtGui.framework/ $${PREFIX}/lib/QtGui.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtNetwork.framework/ $${PREFIX}/lib/QtNetwork.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtDBus.framework/ $${PREFIX}/lib/QtDBus.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtXml.framework/ $${PREFIX}/lib/QtXml.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtMultimedia.framework/ $${PREFIX}/lib/QtMultimedia.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtSvg.framework/ $${PREFIX}/lib/QtSvg.framework/ &&
    qt5_target.extra += rsync -avz --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtWidgets.framework/ $${PREFIX}/lib/QtWidgets.framework/
}
else:unix: {
    QT5_LIBRARIES *= libQt5Core.so.5
    QT5_LIBRARIES *= libQt5DBus.so.5
    QT5_LIBRARIES *= libQt5Gui.so.5
    QT5_LIBRARIES *= libQt5Multimedia.so.5
    QT5_LIBRARIES *= libQt5MultimediaWidgets.so.5
    QT5_LIBRARIES *= libQt5Network.so.5
    QT5_LIBRARIES *= libQt5OpenGL.so.5
    QT5_LIBRARIES *= libQt5Svg.so.5
    QT5_LIBRARIES *= libQt5Widgets.so.5
    QT5_LIBRARIES *= libQt5XcbQpa.so.5
    QT5_LIBRARIES *= libQt5Xml.so.5
    QT5_LIBRARIES *= libqgsttools_p.so.1

# Todo: ICU is necessary on Linux. Uncomment lines below when 5.6.1 is built with ICU enabled on Jenkins
#    QT5_LIBRARIES *= libicui18n.so.56
#    QT5_LIBRARIES *= libicuuc.so.56
#    QT5_LIBRARIES *= libicudata.so.56

    qt5_target.path = $${PREFIX}/lib
    QT5_LIBRARY_DIR = $$[QT_INSTALL_LIBS]
}

for (LIBRARY, QT5_LIBRARIES) {
    LIBRARY_PATH = $${QT5_LIBRARY_DIR}/$${LIBRARY}
    !exists($$LIBRARY_PATH): error("Cannot find $${LIBRARY_PATH}")
    qt5_target.files *= $${LIBRARY_PATH}
}

INSTALLS += qt5_target

############### Install Qt5 audio plugins ##############

win32: qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*.dylib
else:unix:qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*.so
qt5_audio_plugins_target.path = $${PREFIX}/bin/audio
INSTALLS += qt5_audio_plugins_target

############### Install Qt5 iconengines plugins ##############

win32: qt5_iconengines_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/iconengines/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_iconengines_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/iconengines/*.dylib
else:unix:qt5_iconengines_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/iconengines/*.so
qt5_iconengines_plugins_target.path = $${PREFIX}/bin/iconengines
INSTALLS += qt5_iconengines_plugins_target

############### Install Qt5 imageformats plugins ##############

win32: qt5_imageformats_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/imageformats/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_imageformats_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/imageformats/*.dylib
else:unix:qt5_imageformats_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/imageformats/*.so
qt5_imageformats_plugins_target.path = $${PREFIX}/bin/imageformats
INSTALLS += qt5_imageformats_plugins_target

############### Install Qt5 mediaservice plugins ##############

win32: qt5_mediaservice_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/mediaservice/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_mediaservice_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/mediaservice/*.dylib
else:unix:qt5_mediaservice_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/mediaservice/*.so
qt5_mediaservice_plugins_target.path = $${PREFIX}/bin/mediaservice
INSTALLS += qt5_mediaservice_plugins_target

############### Install Qt5 platforms plugins ##############

win32: qt5_platforms_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforms/qwindows$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_platforms_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforms/libqcocoa.dylib
else:unix:qt5_platforms_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforms/libqxcb.so
qt5_platforms_plugins_target.path = $${PREFIX}/bin/platforms
INSTALLS += qt5_platforms_plugins_target

############### Install Qt5 platforminputcontexts plugins ##############

win32: qt5_platforminputcontexts_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforminputcontexts/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_platforminputcontexts_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforminputcontexts/*.dylib
else:unix:qt5_platforminputcontexts_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platforminputcontexts/*.so
qt5_platforminputcontexts_plugins_target.path = $${PREFIX}/bin/platforminputcontexts
INSTALLS += qt5_platforminputcontexts_plugins_target

############### Install Qt5 platformthemes plugins ##############

win32: qt5_platformthemes_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platformthemes/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_platformthemes_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platformthemes/*.dylib
else:unix:qt5_platformthemes_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/platformthemes/*.so
qt5_platformthemes_plugins_target.path = $${PREFIX}/bin/platformthemes
INSTALLS += qt5_platformthemes_plugins_target

############### Install Qt5 xcbglintegrations plugins ##############

unix:!macx {
    qt5_xcbglintegrations_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/xcbglintegrations/*.so
    qt5_xcbglintegrations_plugins_target.path = $${PREFIX}/bin/xcbglintegrations
    INSTALLS += qt5_xcbglintegrations_plugins_target
}

############### Install DBus ##############

win32-g++ {
    DBUS_BINARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_BINARIES *= dbus-daemon.exe
    dbus_binary_target.path = $${PREFIX}/bin

    DBUS_LIBARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_LIBRARIES *= libdbus-1-3.dll
    dbus_library_target.path = $${PREFIX}/bin

    DBUS_CONFIG_SOURCE_DIR = $$SourceRoot
    DBUS_CONFIG_FILES *= etc/dbus-1/*
    dbus_config_target.path = $${PREFIX}/etc/dbus-1
}

win32-msvc2015 {
    DBUS_BINARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_BINARIES *= dbus-daemon.exe
    dbus_binary_target.path = $${PREFIX}/bin

    DBUS_LIBARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_LIBRARIES *= expat.dll
    DBUS_LIBRARIES *= dbus-1-3.dll
    dbus_library_target.path = $${PREFIX}/bin

    DBUS_CONFIG_SOURCE_DIR = $$SourceRoot
    DBUS_CONFIG_FILES *= etc/dbus-1/*
    dbus_config_target.path = $${PREFIX}/etc/dbus-1
}

macx {
    DBUS_BINARY_SOURCE_DIR = /usr/local/bin
    DBUS_BINARIES *= dbus-daemon
    dbus_binary_target.path = $${PREFIX}/bin

    DBUS_LIBARY_SOURCE_DIR = /usr/local/lib
    DBUS_LIBRARIES *= libdbus-1.3.dylib
    dbus_library_target.path = $${PREFIX}/lib

    DBUS_CONFIG_SOURCE_DIR = /usr/local/share/dbus-1
    DBUS_CONFIG_FILES *= session.conf
    DBUS_CONFIG_FILES *= system.conf
    dbus_config_target.path = $${PREFIX}/etc/dbus-1
}

for (BINARY, DBUS_BINARIES) {
    BINARY_PATH = $${DBUS_BINARY_SOURCE_DIR}/$${BINARY}
    !exists($$BINARY_PATH): error("Cannot find $${BINARY_PATH}")
    dbus_binary_target.files *= $${BINARY_PATH}
}

for (LIBRARY, DBUS_LIBRARIES) {
    LIBRARY_PATH = $${DBUS_LIBARY_SOURCE_DIR}/$${LIBRARY}
    !exists($$LIBRARY_PATH): error("Cannot find $${LIBRARY_PATH}")
    dbus_library_target.files *= $${LIBRARY_PATH}
}

for (DBUS_CONFIG_FILE, DBUS_CONFIG_FILES) {
    DBUS_CONFIG_FILE_PATH = $${DBUS_CONFIG_SOURCE_DIR}/$${DBUS_CONFIG_FILE}
    !exists($$DBUS_CONFIG_FILE_PATH): error("Cannot find $${DBUS_CONFIG_FILE_PATH}")
    dbus_config_target.files *= $${DBUS_CONFIG_FILE_PATH}
}

win32|macx: INSTALLS += dbus_binary_target dbus_library_target dbus_config_target

############### Install VC runtime ##############

win32-msvc2015 {
    equals(WORD_SIZE,64) {
         vc_redist_target.files *= $$(VS140COMNTOOLS)../../VC/redist/1033/vcredist_x64.exe
    }
    equals(WORD_SIZE,32) {
        vc_redist_target.files *= $$(VS140COMNTOOLS)../../VC/redist/1033/vcredist_x86.exe
    }
    vc_redist_target.path *= $${PREFIX}/vcredist
    INSTALLS += vc_redist_target
}

win32-g++ {
    VC_RUNTIME_LIBS *= libgcc_s_dw2-1
    VC_RUNTIME_LIBS *= libwinpthread-1

    vc_runtime_target.path *= $${PREFIX}/bin

    for (LIBRARY, VC_RUNTIME_LIBS) {
        vc_runtime_target.files *= $$[QT_INSTALL_BINS]/$${LIBRARY}.$${QMAKE_EXTENSION_SHLIB}
    }

    INSTALLS += vc_runtime_target

    # libstdc++-6.dll needs a workaround since copy does not accept a filepath with '+' in it
    vc_runtime_target.depends += copy_libstdc
    copy_libstdc.target = copy_libstdc
    source_path = $$[QT_INSTALL_BINS]/libstdc++-6.dll
    dest_path = $$vc_runtime_target.path
    copy_libstdc.commands = xcopy /Y $$shell_path($$source_path) $$shell_path($$dest_path)
    QMAKE_EXTRA_TARGETS += copy_libstdc
}

############### Install externals ##############

win32: externals_lib_target.path = $${PREFIX}/bin
else: externals_lib_target.path = $${PREFIX}/lib
!win32-g++: externals_lib_target.files *= $${EXTERNALS_LIB_DIR}/*.$${QMAKE_EXTENSION_SHLIB}
INSTALLS += externals_lib_target

# win32-g++ needs a workaround since copy does not accept a filepath with '+' in it
# we also need to make it dependent on vc_runtime_target since this target has no
# files and does not generate any install target.
win32-g++ {
    vc_runtime_target.depends += copy_externals
    copy_externals.target = copy_externals
    source_path = $${EXTERNALS_LIB_DIR}//*.dll
    dest_path = $${externals_target.path}
    copy_externals.commands += xcopy /Y $$shell_path($$source_path) $$shell_path($$dest_path) $$escape_expand(\n\t)
    QMAKE_EXTRA_TARGETS += copy_externals
}

win32: externals_bin_target.path = $${PREFIX}/bin
else: externals_bin_target.path = $${PREFIX}/bin
!win32-g++: externals_bin_target.files *= $${EXTERNALS_BIN_DIR}/*
INSTALLS += externals_bin_target

############### Installbuilder ##############

bitrock_customize_bin = $$(BITROCK_CUSTOMIZE)
bitrock_builder_bin = $$(BITROCK_BUILDER)
!isEmpty(bitrock_customize_bin):!isEmpty(bitrock_builder_bin) {

    copy_installer_project.depends = install
    win32: copy_installer_project.commands = xcopy /Y /E /I $$shell_path($$SourceRoot/installer) $$shell_path($$DestRoot/../../installer)
    else: copy_installer_project.commands = cp -R $$shell_path($$SourceRoot/installer) $$shell_path($$DestRoot/../../)
    QMAKE_EXTRA_TARGETS += copy_installer_project

    bitrock_autoupdateproject = "$${PREFIX}/../installer/installbuilder/autoupdateproject.xml"
    bitrock_project = $${PREFIX}/../installer/installbuilder/project.xml

    create_updater.depends = copy_installer_project
    win32: create_updater.commands = $${bitrock_customize_bin} build $${bitrock_autoupdateproject} windows
    else:macx: create_updater.commands = $${bitrock_customize_bin} build $${bitrock_autoupdateproject} osx
    else:unix: create_updater.commands = $${bitrock_customize_bin} build $${bitrock_autoupdateproject} linux-x64
    QMAKE_EXTRA_TARGETS += create_updater

    create_installer.depends = create_updater
    win32: create_installer.commands = $${bitrock_builder_bin} build $${bitrock_project} windows \
                                        --setvars project.outputDirectory=$$shell_path($${PREFIX}/..) \
                                        project.installerFilename=swift-installer-$${BLACK_VERSION}-win-$${WORD_SIZE}.exe
    else:macx: create_installer.commands = $${bitrock_builder_bin} build $${bitrock_project} osx \
                                        --setvars project.outputDirectory=$$shell_path($${PREFIX}/..) \
                                        project.installerFilename=swift-installer-$${BLACK_VERSION}-osx-$${WORD_SIZE}.app
    else:unix: create_installer.commands = $${bitrock_builder_bin} build $${bitrock_project} linux-x$${WORD_SIZE} \
                                        --setvars project.outputDirectory=$$shell_path($${PREFIX}/..) \
                                        project.installerFilename=swift-installer-$${BLACK_VERSION}-linux-$${WORD_SIZE}.run
    QMAKE_EXTRA_TARGETS += create_installer
}

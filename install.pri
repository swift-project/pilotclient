CONFIG(debug, debug|release): DLL_DEBUG_SUFFIX = d

############### Readme, License etc. #####

text_files_target.path = $${PREFIX}/bin
text_files_target.files *= README.md
text_files_target.files *= LICENSE

INSTALLS += text_files_target

############### Install Qt5 ##############

win32 {
    qt5_target.path = $${PREFIX}/bin
} else {
    qt5_target.path = $${PREFIX}/lib
}

qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Core$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Gui$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Network$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5DBus$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Xml$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Multimedia$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Svg$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/Qt5Widgets$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/icudt54.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/icuin54.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/icuuc54.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/libeay32.$${QMAKE_EXTENSION_SHLIB}
qt5_target.files *= $$[QT_INSTALL_BINS]/ssleay32.$${QMAKE_EXTENSION_SHLIB}

INSTALLS += qt5_target

############### Install Qt5 platform plugins ##############

win32 {
    QT5_PLATFORM_PLUGINS *= qwindows
}

qt5_plugin_target.path = $${PREFIX}/bin/platforms

for (LIBRARY, QT5_PLATFORM_PLUGINS) {
    qt5_plugin_target.files *= $$[QT_INSTALL_PLUGINS]/platforms/$${LIBRARY}$${DLL_DEBUG_SUFFIX}.$${QMAKE_EXTENSION_SHLIB}
}

INSTALLS += qt5_plugin_target


############### Install DBus ##############

win32-g++ {
    DBUS_BINARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_BINARIES *= libdbus-1-3.dll
    DBUS_BINARIES *= dbus-daemon.exe
    dbus_target.path = $${PREFIX}/bin

    DBUS_CONFIG_SOURCE_DIR = $$[QT_INSTALL_BINS]/..
    DBUS_CONFIG_FILES *= share/dbus-1/*
    dbus_config_target.path = $${PREFIX}/share/dbus-1
}

win32-msvc2015 {
    DBUS_BINARY_SOURCE_DIR = $$[QT_INSTALL_BINS]
    DBUS_BINARIES *= expat.dll
    DBUS_BINARIES *= dbus-1-3.dll
    DBUS_BINARIES *= dbus-daemon.exe
    dbus_target.path = $${PREFIX}/bin

    DBUS_CONFIG_SOURCE_DIR = $$[QT_INSTALL_BINS]/..
    DBUS_CONFIG_FILES *= share/dbus-1/*
    dbus_config_target.path = $${PREFIX}/share/dbus-1
}

for (BINARY, DBUS_BINARIES) {
    BINARY_PATH = $${DBUS_BINARY_SOURCE_DIR}/$${BINARY}
    !exists($$BINARY_PATH): error("Cannot find $${BINARY_PATH}")
    dbus_target.files *= $${BINARY_PATH}
}

for (DBUS_CONFIG_FILE, DBUS_CONFIG_FILES) {
    DBUS_CONFIG_FILE_PATH = $${DBUS_CONFIG_SOURCE_DIR}/$${DBUS_CONFIG_FILE}
    !exists($$DBUS_CONFIG_FILE_PATH): error("Cannot find $${DBUS_CONFIG_FILE_PATH}")
    dbus_config_target.files *= $${DBUS_CONFIG_FILE_PATH}
}

INSTALLS += dbus_target dbus_config_target

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

win32: externals_target.path = $${PREFIX}/bin
else: externals_target.path = $${PREFIX}/lib
!win32-g++: externals_target.files *= $${EXTERNALSDIR}/*.$${QMAKE_EXTENSION_SHLIB}
INSTALLS += externals_target

# win32-g++ needs a workaround since copy does not accept a filepath with '+' in it
# we also need to make it dependent on vc_runtime_target since this target has no
# files and does not generate any install target.
win32-g++ {
    vc_runtime_target.depends += copy_externals
    copy_externals.target = copy_externals
    source_path = $${EXTERNALSDIR}//*.dll
    dest_path = $${externals_target.path}
    copy_externals.commands += xcopy /Y $$shell_path($$source_path) $$shell_path($$dest_path) $$escape_expand(\n\t)
    QMAKE_EXTRA_TARGETS += copy_externals
}

############### Installbuilder ##############

bitrock_customize_bin = $$(BITROCK_CUSTOMIZE)
bitrock_builder_bin = $$(BITROCK_BUILDER)
win32:!isEmpty(bitrock_customize_bin):!isEmpty(bitrock_builder_bin) {

    copy_installer_project.depends = install
    copy_installer_project.commands = xcopy /Y /E /I $$shell_path($$SourceRoot/installer) $$shell_path($$DestRoot/../../installer)
    QMAKE_EXTRA_TARGETS += copy_installer_project

    bitrock_autoupdateproject = "$${PREFIX}/../installer/installbuilder/autoupdateproject.xml"
    bitrock_project = $${PREFIX}/../installer/installbuilder/project.xml

    create_updater.depends = copy_installer_project
    create_updater.commands = $${bitrock_customize_bin} build $${bitrock_autoupdateproject} windows
    QMAKE_EXTRA_TARGETS += create_updater

    create_installer.depends = create_updater
    create_installer.commands = $${bitrock_builder_bin} build $${bitrock_project} windows --setvars project.outputDirectory=$${PREFIX}/..
    QMAKE_EXTRA_TARGETS += create_installer
}

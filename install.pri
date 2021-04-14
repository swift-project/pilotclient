############### Readme, License etc. #####

text_files_target.path = $${PREFIX}/bin
text_files_target.files *= README.md
text_files_target.files *= LICENSE

INSTALLS += text_files_target

############### Install Qt5 ##############

win32 {
    QT5_LIBRARIES *= Qt5Core$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5DBus$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Gui$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Multimedia$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Network$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5OpenGL$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Qml$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Svg$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Widgets$${DLL_DEBUG_SUFFIX}.dll
    QT5_LIBRARIES *= Qt5Xml$${DLL_DEBUG_SUFFIX}.dll

    qt5_target.path = $${PREFIX}/bin
    QT5_LIBRARY_DIR = $$[QT_INSTALL_BINS]
}
else:macx {
    # MacOS workaround using rsync. Otherwise all headers are also copied.
    qt5_target.path = $${PREFIX}/lib/QtCore.framework
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtConcurrent.framework/ $${PREFIX}/lib/QtConcurrent.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtCore.framework/ $${PREFIX}/lib/QtCore.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtDBus.framework/ $${PREFIX}/lib/QtDBus.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtGui.framework/ $${PREFIX}/lib/QtGui.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtMultimedia.framework/ $${PREFIX}/lib/QtMultimedia.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtNetwork.framework/ $${PREFIX}/lib/QtNetwork.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtOpenGL.framework/ $${PREFIX}/lib/QtOpenGL.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtQml.framework/ $${PREFIX}/lib/QtQml.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtPrintSupport.framework/ $${PREFIX}/lib/QtPrintSupport.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtSvg.framework/ $${PREFIX}/lib/QtSvg.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtWidgets.framework/ $${PREFIX}/lib/QtWidgets.framework/ &&
    qt5_target.extra += rsync -avzl --exclude \'Headers*\' --exclude \'*debug*\' $$[QT_INSTALL_LIBS]/QtXml.framework/ $${PREFIX}/lib/QtXml.framework/
}
else:unix: {
    QT5_LIBRARIES *= libQt5Core.so.5
    QT5_LIBRARIES *= libQt5DBus.so.5
    QT5_LIBRARIES *= libQt5Gui.so.5
    QT5_LIBRARIES *= libQt5Multimedia.so.5
    QT5_LIBRARIES *= libQt5MultimediaWidgets.so.5
    QT5_LIBRARIES *= libQt5Network.so.5
    QT5_LIBRARIES *= libQt5OpenGL.so.5
    QT5_LIBRARIES *= libQt5Qml.so.5
    QT5_LIBRARIES *= libQt5Svg.so.5
    QT5_LIBRARIES *= libQt5Widgets.so.5
    QT5_LIBRARIES *= libQt5XcbQpa.so.5
    QT5_LIBRARIES *= libQt5Xml.so.5
    QT5_LIBRARIES *= libQt5MultimediaGstTools.so.5

    ICU_LIBRARIES *= libicui18n.so.56
    ICU_LIBRARIES *= libicuuc.so.56
    ICU_LIBRARIES *= libicudata.so.56

    qt5_target.path = $${PREFIX}/lib
    QT5_LIBRARY_DIR = $$[QT_INSTALL_LIBS]
}

for (LIBRARY, QT5_LIBRARIES) {
    LIBRARY_PATH = $${QT5_LIBRARY_DIR}/$${LIBRARY}
    !exists($$LIBRARY_PATH): error("Cannot find $${LIBRARY_PATH}")
    qt5_target.files *= $${LIBRARY_PATH}
}


for (LIBRARY, ICU_LIBRARIES) {
    LIBRARY_PATH = $${QT5_LIBRARY_DIR}/$${LIBRARY}
    exists($$LIBRARY_PATH): qt5_target.files *= $${LIBRARY_PATH}
}

INSTALLS += qt5_target

############### Install Qt5 audio plugins ##############

win32: qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*.dylib
else:unix:qt5_audio_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/audio/*.so
qt5_audio_plugins_target.path = $${PREFIX}/bin/audio
INSTALLS += qt5_audio_plugins_target

############### Install Qt5 bearer plugins ##############

win32: qt5_bearer_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/bearer/*$${DLL_DEBUG_SUFFIX}.dll
else:macx: qt5_bearer_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/bearer/*.dylib
else:unix:qt5_bearer_plugins_target.files *= $$[QT_INSTALL_PLUGINS]/bearer/*.so
qt5_bearer_plugins_target.path = $${PREFIX}/bin/bearer
INSTALLS += qt5_bearer_plugins_target

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

############### Install VC runtime ##############

win32-msvc {
    # https://github.com/Microsoft/vswhere/wiki
    VS_WHERE = $$system_quote($$system_path($$getenv(programfiles(x86))/Microsoft Visual Studio/Installer/vswhere.exe))
    VC_REDIST_COMPONENT = Microsoft.VisualStudio.Component.VC.Redist.14.Latest
    VS_PATH = $$system($$VS_WHERE -latest -products * -requires $$VC_REDIST_COMPONENT -property installationPath)
    VC_REDIST_VERSION = $$cat($$VS_PATH/VC/Auxiliary/Build/Microsoft.VCRedistVersion.default.txt)
    VC_REDIST_PATH = $$VS_PATH/VC/Redist/MSVC/$$VC_REDIST_VERSION

    equals(WORD_SIZE,64) {
        vc_redist_target.files *= $$shell_path($$VC_REDIST_PATH/vcredist_x64.exe)
    }
    equals(WORD_SIZE,32) {
        vc_redist_target.files *= $$shell_path($$VC_REDIST_PATH/vcredist_x86.exe)
    }
    vc_redist_target.path *= $${PREFIX}/vcredist
    INSTALLS += vc_redist_target
}

win32-g++ {
    equals(WORD_SIZE,32) {
        VC_RUNTIME_LIBS *= libgcc_s_dw2-1
    }
    equals(WORD_SIZE,64) {
        VC_RUNTIME_LIBS *= libgcc_s_seh-1
    }
    VC_RUNTIME_LIBS *= libwinpthread-1
    VC_RUNTIME_LIBS *= libstdc++-6

    vc_runtime_target.path *= $${PREFIX}/bin

    for (LIBRARY, VC_RUNTIME_LIBS) {
        vc_runtime_target.files *= $$[QT_INSTALL_BINS]/$${LIBRARY}.$${QMAKE_EXTENSION_SHLIB}
    }

    INSTALLS += vc_runtime_target
}

############### Install csl2xsb ###############

win32:swiftConfig(sims.xswiftbus) {
    PYINST_BIN = $$system($$(SYSTEMROOT)\system32\where pyinstaller 2> nul)
    !isEmpty(PYINST_BIN) {
        CSL2XSB = $$shell_path($$SourceRoot/scripts/csl2xsb/CSL2XSB.py)
        csl2xsb.path = $${PREFIX}/share/CSL2XSB
        csl2xsb.extra = $$PYINST_BIN -y --distpath $$shell_path($${PREFIX}/share) --workpath $$shell_path($$(TEMP)) $$CSL2XSB
        INSTALLS += csl2xsb
    }
}

############### Install externals ##############

win32: externals_lib_target.path = $${PREFIX}/bin
else: externals_lib_target.path = $${PREFIX}/lib
externals_lib_target.files *= $${EXTERNALS_LIB_DIR}/*.$${QMAKE_EXTENSION_SHLIB}*
macx: externals_lib_target.files *= $${EXTERNALS_LIB_DIR}/qwt.framework
INSTALLS += externals_lib_target

win32: externals_bin_target.path = $${PREFIX}/bin
else: externals_bin_target.path = $${PREFIX}/bin
externals_bin_target.files *= $${EXTERNALS_BIN_DIR}/*
INSTALLS += externals_bin_target

############### Installbuilder ##############

bitrock_builder_bin = $$(BITROCK_BUILDER)
!isEmpty(bitrock_builder_bin) {

    copy_installer_project.depends = install
    win32: copy_installer_project.commands = xcopy /Y /E /I $$shell_path($$SourceRoot/installer) $$shell_path($$DestRoot/../../installer)
    else: copy_installer_project.commands = cp -R $$shell_path($$SourceRoot/installer) $$shell_path($$DestRoot/../../)
    QMAKE_EXTRA_TARGETS += copy_installer_project

    bitrock_project = $${PREFIX}/../installer/installbuilder/project.xml

    create_installer.depends = copy_installer_project
    WINDOWS64BITMODE = 0
    win32 {
        INSTALLER_PLATFORM = windows
        INSTALLER_BASENAME = swiftinstaller-windows-$${WORD_SIZE}-$$swiftConfig(version.full)
        INSTALLER_EXT = exe
        ARCHITECTURE = 32
        equals(WORD_SIZE,64) {
            WINDOWS64BITMODE = 1
            ARCHITECTURE = 64
        }
    }
    else:macx {
        INSTALLER_PLATFORM = osx
        INSTALLER_BASENAME = swiftinstaller-macos-$${WORD_SIZE}-$$swiftConfig(version.full)
        INSTALLER_EXT = app
        ARCHITECTURE = 64
    }
    else:unix {
        INSTALLER_PLATFORM = linux-x$${WORD_SIZE}
        INSTALLER_BASENAME = swiftinstaller-linux-$${WORD_SIZE}-$$swiftConfig(version.full)
        INSTALLER_EXT = run
        ARCHITECTURE = 64
    }
    swiftConfig(devBranch): INSTALLER_DEBUG = 1
    else:                   INSTALLER_DEBUG = 0

    create_installer.commands = $$shell_path($${bitrock_builder_bin}) quickbuild $${bitrock_project} $${INSTALLER_PLATFORM} \
                                    --license $$shell_path($$(HOME)/license.xml) \
                                    --setvars project.outputDirectory=$$shell_path($${PREFIX}/..) \
                                              project.installerFilename=$${INSTALLER_BASENAME}.$${INSTALLER_EXT} \
                                              project.version=$$swiftConfig(version.full) \
                                              versionFull=$$swiftConfig(version.full).$${VER_REV} \
                                              endOfLife=$$swiftConfig(endOfLife) \
                                              project.windows64bitMode=$${WINDOWS64BITMODE} \
                                              project.enableDebugger=$${INSTALLER_DEBUG} \
                                              architecture=$${ARCHITECTURE}
    QMAKE_EXTRA_TARGETS += create_installer
}

############### Bitrock Installbuilder Files ###############

# List them in IDE
OTHER_FILES += installer/installbuilder/*.xml

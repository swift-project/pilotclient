LIBS *= -L$$DestRoot/lib

unix:!macx {
    # Set the rpath-link to find dependent shared libraries when linking
    # Note: This does not add any rpath into the binaries.
    LIBS += -Wl,-rpath-link,$$DestRoot/lib -Wl,-rpath-link,$$[QT_INSTALL_LIBS]
}

simulatorfsxcommon {
    addStaticLibraryDependency(simulatorfsxcommon)
    LIBS += -lsimulatorfsxcommon
}

simulatorfscommon {
    addStaticLibraryDependency(simulatorfscommon)
    LIBS += -lsimulatorfscommon
}

simulatorplugincommon {
    addStaticLibraryDependency(simulatorplugincommon)
    LIBS +=  -lsimulatorplugincommon
}

fsuipc {
    addStaticLibraryDependency(fsuipc)
    LIBS += -lfsuipc
}

simconnect {
    DEFINES += SIMCONNECT_H_NOMANIFEST

    equals(WORD_SIZE,64) {
        INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/P3D-v4.1
        LIBS *= -L$$EXTERNALS_LIB_DIR/P3D-v4.1
        LIBS *= -lAdvapi32
        LIBS += -ldxguid -lole32
        # ole32 only needed for P3D on WIN64 systems, LNK2019: unresolved external symbol __imp_CoTaskMemFree referenced in function
        # ldxguid are DirectX guid numbers
        CONFIG(debug, debug|release): LIBS *= -lSimConnectDebug
        else:                         LIBS *= -lSimConnect
    }
    equals(WORD_SIZE,32) {
        INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
        # LIBS *= -L$$EXTERNALS_LIB_DIR/FSX-XPack
    }

    RC_FILE = $$SourceRoot/src/plugins/simulator/fsxcommon/simconnect.rc

    msvc: QMAKE_LFLAGS *= /ignore:4099
}

blackgui {
    addLibraryDependency(blackgui)
    LIBS *= -lblackgui
}

blackcore {
    addLibraryDependency(blackcore)
    LIBS *= -lblackcore -lvatlib
}

blacksound {
    LIBS *= -lblacksound
}

blackinput {
    addLibraryDependency(blackinput)
    LIBS *= -lblackinput

    macx {
        LIBS += -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
    }

    win32 {
        LIBS *= -ldxguid -lole32 -ldinput8 -lUser32
    }
}

blackmisc {
    addLibraryDependency(blackmisc)
    LIBS *= -lblackmisc

    win32 {
        LIBS *= -lShell32
    }
}

blackconfig {
    addStaticLibraryDependency(blackconfig)
    LIBS *= -lblackconfig
}

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

LIBS *= -L$$DestRoot/lib

unix:!macx {
    # Set the rpath-link to find dependent shared libraries when linking
    # Note: This does not add any rpath into the binaries.
    LIBS += -Wl,-rpath-link,$$DestRoot/lib -Wl,-rpath-link,$$[QT_INSTALL_LIBS]
}

simulatorfsxcommon {
    addLibraryDependency(simulatorfsxcommon)
    LIBS += -lsimulatorfsxcommon
}

simulatorfscommon {
    addLibraryDependency(simulatorfscommon)
    LIBS += -lsimulatorfscommon
}

simulatorplugincommon {
    addLibraryDependency(simulatorplugincommon)
    LIBS +=  -lsimulatorplugincommon
}

fsuipc {
    addStaticLibraryDependency(fsuipc)
    LIBS += -lfsuipc
}

simconnect {
    DEFINES += SIMCONNECT_H_NOMANIFEST
    equals(WORD_SIZE,64) {
        SIMCONNECT_INCLUDE += $$EXTERNALSROOT/common/include/simconnect/P3D-v4
        DEFINES += P3D_SDK_VERSION=400
    }
    equals(WORD_SIZE,32) {
        SIMCONNECT_INCLUDE += $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
    }
    INCLUDEPATH *= $$SIMCONNECT_INCLUDE
    gcc:QMAKE_CXXFLAGS_WARN_ON += -isystem $$SIMCONNECT_INCLUDE
    llvm:QMAKE_CXXFLAGS_WARN_ON *= $$clangArg(-isystem$$system_path($$SIMCONNECT_INCLUDE))
}

blackgui {
    addLibraryDependency(blackgui)
    LIBS *= -lblackgui
}

blackcore {
    addLibraryDependency(blackcore)
    LIBS *= -lblackcore
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

# to include buildconfig_gen.inc
INCLUDEPATH *= $$BuildRoot/generated

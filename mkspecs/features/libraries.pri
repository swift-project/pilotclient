LIBS *= -L$$DestRoot/lib

unix:!macx {
    # Set the rpath-link to find dependent shared libraries when linking
    # Note: This does not add any rpath into the binaries.
    LIBS += -Wl,-rpath-link,$$DestRoot/lib -Wl,-rpath-link,$$[QT_INSTALL_LIBS]
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

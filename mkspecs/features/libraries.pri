LIBS *= -L$$DestRoot/lib

unix:!macx {
    # Set the rpath-link to find dependent shared libraries when linking
    # Note: This does not add any rpath into the binaries.
    LIBS += -Wl,-rpath-link,$$DestRoot/lib -Wl,-rpath-link,$$[QT_INSTALL_LIBS]
}

blackgui {
    contains(BLACK_CONFIG, Static) {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackgui.lib
        else:           PRE_TARGETDEPS += $$DestRoot/lib/libblackgui.a
    } else {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackgui.lib
        win32-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackgui.a
        linux-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackgui.so
        macx-clang:     PRE_TARGETDEPS += $$DestRoot/lib/libblackgui.dylib
    }

    LIBS *= -lblackgui
}

blackcore {
    contains(BLACK_CONFIG, Static) {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackcore.lib
        else:           PRE_TARGETDEPS += $$DestRoot/lib/libblackcore.a
    } else {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackcore.lib
        win32-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackcore.a
        linux-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackcore.so
        macx-clang:     PRE_TARGETDEPS += $$DestRoot/lib/libblackcore.dylib
    }

    LIBS *= -lblackcore -lvatlib

    win32 {
        contains(BLACK_CONFIG, FSX) {
            LIBS *= -lSimConnect
            LIBS *= -lFSUIPC_User
        }
    }
}

blacksound {
    LIBS *= -lblacksound
}

blackinput {
    contains(BLACK_CONFIG, Static) {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackinput.lib
        else:           PRE_TARGETDEPS += $$DestRoot/lib/libblackinput.a
    } else {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackinput.lib
        win32-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackinput.a
        linux-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackinput.so
        macx-clang:     PRE_TARGETDEPS += $$DestRoot/lib/libblackinput.dylib
    }

    LIBS *= -lblackinput

    macx {
        LIBS += -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
    }

    win32 {
        LIBS *= -ldxguid -lole32 -ldinput8 -lUser32
    }
}

blackmisc {
    contains(BLACK_CONFIG, Static) {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackmisc.lib
        else:           PRE_TARGETDEPS += $$DestRoot/lib/libblackmisc.a
    } else {
        win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackmisc.lib
        win32-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackmisc.a
        linux-g++*:     PRE_TARGETDEPS += $$DestRoot/lib/libblackmisc.so
        macx-clang:     PRE_TARGETDEPS += $$DestRoot/lib/libblackmisc.dylib
    }
    LIBS *= -lblackmisc

    win32 {
        LIBS *= -lShell32
    }
}

blackconfig {
    win32:msvc:     PRE_TARGETDEPS += $$DestRoot/lib/blackconfig.lib
    else:           PRE_TARGETDEPS += $$DestRoot/lib/libblackconfig.a
    LIBS *= -lblackconfig
}

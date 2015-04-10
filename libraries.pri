include (externals.pri)

LIBS *= -L$$BuildRoot/lib

unix {
    # Set the rpath-link to find dependent shared libraries when linking
    # Note: This does not add any rpath into the binaries.
    LIBS += -Wl,-rpath-link,$$BuildRoot/lib
}

blackgui {
    contains(BLACK_CONFIG, Static) {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackgui.lib
        else:           PRE_TARGETDEPS += $$BuildRoot/lib/libblackgui.a
    } else {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackgui.lib
        win32-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackgui.a
        linux-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackgui.so
        macx-clang:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackgui.dylib
    }

    LIBS *= -lblackgui
}

blackcore {
    contains(BLACK_CONFIG, Static) {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackcore.lib
        else:           PRE_TARGETDEPS += $$BuildRoot/lib/libblackcore.a
    } else {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackcore.lib
        win32-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackcore.a
        linux-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackcore.so
        macx-clang:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackcore.dylib
    }

    LIBS *= -lblackcore -lvatlib2

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
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackinput.lib
        else:           PRE_TARGETDEPS += $$BuildRoot/lib/libblackinput.a
    } else {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackinput.lib
        win32-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackinput.a
        linux-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackinput.so
        macx-clang:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackinput.dylib
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
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackmisc.lib
        else:           PRE_TARGETDEPS += $$BuildRoot/lib/libblackmisc.a
    } else {
        win32-msvc*:    PRE_TARGETDEPS += $$BuildRoot/lib/blackmisc.lib
        win32-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackmisc.a
        linux-g++*:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackmisc.so
        macx-clang:     PRE_TARGETDEPS += $$BuildRoot/lib/libblackmisc.dylib
    }
    LIBS *= -lblackmisc
}

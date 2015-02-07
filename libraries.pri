include (externals.pri)

LIBS *= -L../../lib -L../../../lib -L../../../../lib

blackgui {
    LIBS += -lblackgui
}

blackcore {
    LIBS += -lblackcore -lvatlib2

    win32 {
        contains(BLACK_CONFIG, FSX) {
            LIBS += -lSimConnect
            LIBS += -lFSUIPC_User
        }
    }
}

blacksound {
    LIBS += -lblacksound
}

blackinput {
    LIBS += -lblackinput

    macx {
        LIBS += -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
    }

    win32 {
        LIBS += -ldxguid -lole32 -ldinput8 -lUser32
    }
}

blackmisc {
    LIBS += -lblackmisc
}

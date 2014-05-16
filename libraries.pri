include (externals.pri)

LIBS *= -L../../lib -L../../../lib -L../../../../lib

blackgui {
    LIBS += -lblackgui
}

blackcore {
    LIBS += -lblackcore -lvatlib

    win32 {
        contains(BLACK_CONFIG, FSX) {
            LIBS += -lSimConnect
        }
    }
}

blacksound {
    LIBS += -lblacksound
}

blacksim {
    LIBS += -lblacksim
}

blackmisc {
    LIBS += -lblackmisc
}

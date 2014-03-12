include (externals.pri)

LIBS *= -L../../lib

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

blackmisc {
    LIBS += -lblackmisc
}

blacksim {
    LIBS += -lblacksim
}

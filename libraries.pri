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
            LIBS += -lFSUIPC_User
        }
    }
}

blacksound {
    LIBS += -lblacksound
}

blackinput {
    LIBS += -lblackinput
}

blacksim {
    LIBS += -lblacksim
}

blackmisc {
    LIBS += -lblackmisc
}

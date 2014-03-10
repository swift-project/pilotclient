include (externals.pri)

LIBS *= -L../../lib

blackgui {
    LIBS += -lblackgui
}

blackcore {
    LIBS += -lblackcore -lvatlib
}

blacksound {
    LIBS += -lblacksound
}

blackmisc {
    LIBS += -lblackmisc
}

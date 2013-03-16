QT       += core network
QT       -= gui

TARGET = sample_config
TEMPLATE = app

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += main.cpp\

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackmisc.lib \

    LIBS += ../../lib/blackmisc.lib \
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a \

    LIBS += ../../lib/libblackmisc.a \
}

DESTDIR = ../../bin




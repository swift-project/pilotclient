QT       += core network
QT       -= gui

TARGET = com_client

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

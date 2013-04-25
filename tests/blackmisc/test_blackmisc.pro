QT       += core testlib

TARGET = test_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

HEADERS += *.h
SOURCES += *.cpp

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

QT       += core
QT       -= gui

TARGET = sample_plugin
TEMPLATE = lib
CONFIG += plugin

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

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

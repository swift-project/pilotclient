QT       += core
QT       -= gui

TARGET = sample_pluginmgr
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    PRE_TARGETDEPS +=  ../../lib/blackcore.lib
    LIBS += ../../lib/blackmisc.lib
    LIBS += ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    PRE_TARGETDEPS += ../../lib/libblackcore.a
    LIBS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackcore.a
}

DESTDIR = ../../bin

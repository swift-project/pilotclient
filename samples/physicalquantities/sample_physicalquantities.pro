QT       += core
QT       -= gui

TARGET = sample_physicalquantities
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackcore
INCLUDEPATH += . ../../src
SOURCES += main.cpp

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackcore.lib
    LIBS += ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackcore.a
    LIBS += ../../lib/libblackcore.a
}

DESTDIR = ../../bin

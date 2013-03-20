QT       += core testlib
QT       -= gui

TARGET = test_blackcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackcore
INCLUDEPATH += . ../../src/blackcore
SOURCES += main.cpp testmain.cpp testphysicalquantitiesbase.cpp
HEADERS += testmain.h testphysicalquantitiesbase.h

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackcore.lib
    LIBS += ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackcore.a
    LIBS += ../../lib/libblackcore.a
}

DESTDIR = ../../bin

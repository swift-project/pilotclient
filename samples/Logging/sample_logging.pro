QT       += core
QT       -= gui

TARGET = sample_logging
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

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




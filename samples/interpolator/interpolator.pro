QT       += core
QT       -= gui

TARGET = sample_interpolator
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += main.cpp\

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                      ../../lib/blackcore.lib

    LIBS += ../../lib/blackmisc.lib \
            ../../lib/blackcore.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                      ../../lib/libblackcore.a

    LIBS += ../../lib/libblackmisc.a \
            ../../lib/libblackcore.a
}

DESTDIR = ../../bin




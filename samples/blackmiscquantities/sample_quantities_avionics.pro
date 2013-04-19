QT       += core
QT       -= gui

TARGET = sample_physicalquantities
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

SOURCES += main.cpp \
    samplesphysicalquantities.cpp \
    samplesaviation.cpp

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

HEADERS += \
    samplesphysicalquantities.h \
    samplesaviation.h

QT       += core

TARGET = sample_vector_geo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

win32-msvc* {
    PRE_TARGETDEPS += ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

SOURCES += main.cpp \
    samplesvectormatrix.cpp \
    samplesgeo.cpp \
    samplesgeodetictoecef.cpp

HEADERS += \
    samplesvectormatrix.h \
    samplesgeo.h \
    samplesgeodetictoecef.h

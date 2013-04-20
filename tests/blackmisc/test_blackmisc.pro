QT       += core testlib

TARGET = test_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src
SOURCES += main.cpp testmain.cpp \
    testphysicalquantitiesbase.cpp \
    testaviationbase.cpp \
    testvectormatrixbase.cpp

HEADERS += testmain.h \
    testphysicalquantitiesbase.h \
    blackmisctest.h \
    testaviationbase.h \
    testvectormatrixbase.h

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

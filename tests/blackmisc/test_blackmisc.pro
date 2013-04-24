QT       += core testlib

TARGET = test_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src
SOURCES += main.cpp testmain.cpp \
    testphysicalquantities.cpp \
    testvectormatrix.cpp \
    testaviation.cpp \
    testgeo.cpp

HEADERS += testmain.h \
    blackmisctest.h \
    testphysicalquantities.h \
    testvectormatrix.h \
    testaviation.h \
    testgeo.h

win32-msvc* {
    PRE_TARGETDEPS +=  ../../lib/blackmisc.lib
    LIBS += ../../lib/blackmisc.lib
}

!win32-msvc* {
    PRE_TARGETDEPS += ../../lib/libblackmisc.a
    LIBS += ../../lib/libblackmisc.a
}

DESTDIR = ../../bin

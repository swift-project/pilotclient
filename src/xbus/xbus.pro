include (../../config.pri)
include (../../build.pri)

QT       += core gui widgets dbus network

TEMPLATE = lib

CONFIG += shared
CONFIG += blackmisc blackcore

LIBS += -lXPLM

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

#win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackmisc.lib
#win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackcore.lib


# Required by X-Plane SDK
DEFINES += XPLM200=1
win32:DEFINES += IBM
linux:DEFINES += LIN
macx:DEFINES += APL

# X-Plane plugins must follow a prescribed filename and directory structure.
# CPU architecture detection copied from externals.pri;
# TODO: refactor so detection is is done in only one place.
TARGET_EXT = .xpl
win32:TARGET = win
linux:TARGET = lin
macx:TARGET = mac
win32:contains(QMAKE_TARGET.arch, x86_64) {
    DESTDIR = ../../xbus/64
}
win32:contains(QMAKE_TARGET.arch, x86) {
    DESTDIR = ../../xbus
}
win32-g++ {
    WIN_FIND = $$(SYSTEMROOT)\system32\find
    MINGW64 = $$system($$QMAKE_CXX -Q --help=target | $$WIN_FIND \"-m64\")
    contains(MINGW64,[enabled]) {
        DESTDIR = ../../xbus/64
    }
    else {
        DESTDIR = ../../xbus
    }
}
linux-g++ {
    GCC64 = $$system($$QMAKE_CXX -Q --help=target | grep m64)
    contains(GCC64,[enabled]) {
        DESTDIR = ../../xbus/64
    }
    else {
        DESTDIR = ../../xbus
    }
}
linux-g++-64 {
    DESTDIR = ../../xbus/64
}
linux-g++-32 {
    DESTDIR = ../../xbus
}
macx {
    # a single dylib file contains both 32bit and 64bit binaries
    DESTDIR = ../../xbus
}

include (../../libraries.pri)

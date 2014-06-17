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
TARGET_EXT = .xpl
win32:TARGET = win
linux:TARGET = lin
macx:TARGET = mac
macx {
    # a single dylib file contains both 32bit and 64bit binaries
    DESTDIR = ../../xbus
}
else {
    equals(WORD_SIZE,64): DESTDIR = ../../xbus/64
    equals(WORD_SIZE,32): DESTDIR = ../../xbus
}

include (../../libraries.pri)

include (../../../config.pri)
include (../../../build.pri)

QT       += core dbus gui network

TARGET = simulator_fsx
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore

LIBS += -lSimConnect


DEPENDPATH += . ../../../src
INCLUDEPATH += . ../../../src

SOURCES += *.cpp
HEADERS += *.h

win32:!win32-g++*: PRE_TARGETDEPS += ../../../lib/blackmisc.lib
win32:!win32-g++*: PRE_TARGETDEPS += ../../../lib/blackcore.lib

DESTDIR = ../../../bin/plugins

include (../../../libraries.pri)

include (../../../../config.pri)
include (../../../../build.pri)

QT       += core dbus network

TARGET = simulator_xplane
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc

DEPENDPATH += . ../../../../src
INCLUDEPATH += . ../../../../src

SOURCES += *.cpp
HEADERS += *.h

#win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackmisc.lib

DESTDIR = ../../../../bin/plugins/simulator

include (../../../../libraries.pri)

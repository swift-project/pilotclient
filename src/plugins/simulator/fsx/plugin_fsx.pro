include (../../../../config.pri)
include (../../../../build.pri)

QT       += core dbus gui network concurrent

TARGET = simulator_fsx
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blacksim

LIBS += -lSimConnect

DEPENDPATH += . ../../../../src
INCLUDEPATH += . ../../../../src

SOURCES += *.cpp
HEADERS += *.h

win32:!win32-g++*: PRE_TARGETDEPS += ../../../../lib/blackmisc.lib \
                                     ../../../../lib/blacksim.lib \
                                     ../../../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../../../lib/libblackmisc.a \
                                     ../../../../lib/libblacksim.a \
                                     ../../../../lib/libblackcore.a

DESTDIR = ../../../../bin/plugins/simulator
include (../../../../libraries.pri)

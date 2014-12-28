include (../../../../config.pri)
include (../../../../build.pri)

QT       += core dbus gui network concurrent xml

TARGET = simulator_fsx
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blacksim

LIBS +=  -lsimulator_fscommon -lSimConnect -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

DEPENDPATH += . ../../../../src
INCLUDEPATH += . ../../../../src

LIBS += -ldxguid -lole32

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

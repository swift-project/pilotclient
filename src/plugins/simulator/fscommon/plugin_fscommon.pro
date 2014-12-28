include (../../../../config.pri)
include (../../../../build.pri)

QT       += core dbus xml

TARGET = simulator_fscommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc

LIBS += -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

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
DESTDIR = ../../lib

DESTDIR = ../../../../lib
include (../../../../libraries.pri)

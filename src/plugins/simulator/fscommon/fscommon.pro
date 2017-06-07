load(common_pre)

QT       += core dbus xml network

TARGET = simulatorfscommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc

LIBS += -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$DestRoot/lib

load(common_post)

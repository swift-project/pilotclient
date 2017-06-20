load(common_pre)

QT       += core dbus xml network

TARGET = simulatorfscommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

contains(BLACK_CONFIG, FSUIPC): DEFINES += SWIFT_USING_FSUIPC

LIBS += -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

DESTDIR = $$DestRoot/lib

load(common_post)

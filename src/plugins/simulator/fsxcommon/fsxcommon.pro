load(common_pre)

QT += core dbus xml network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc blackcore blackgui

LIBS += -lsimulatorfscommon -lSimConnect -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui

DESTDIR = $$DestRoot/lib

load(common_post)

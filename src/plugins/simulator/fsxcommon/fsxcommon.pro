load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui

CONFIG += staticlib
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$DestRoot/lib

LIBS += -lsimulatorfscommon -lfsuipc -lsimulatorplugincommon

include(../fsxp3d_include.pri)

addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

load(common_post)

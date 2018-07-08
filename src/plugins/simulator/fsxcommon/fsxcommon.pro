load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui

CONFIG += staticlib
CONFIG += blackconfig blackmisc blackcore blackgui simconnect

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$DestRoot/lib

LIBS += -lsimulatorfscommon -lfsuipc -lsimulatorplugincommon

addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

OTHER_FILES += \
        *.manifest \
        *.rc

load(common_post)

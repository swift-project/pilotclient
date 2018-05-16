load(common_pre)

QT += core dbus xml network widgets

TARGET = simulatorplugincommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackconfig blackmisc blackgui

DEPENDPATH  += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
DESTDIR = $$DestRoot/lib

load(common_post)

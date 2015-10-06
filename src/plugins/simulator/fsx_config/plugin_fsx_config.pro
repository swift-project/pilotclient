load(common_pre)

QT       += core widgets dbus

TARGET = simulator_fsx_config
TEMPLATE = lib
CONFIG += plugin shared
CONFIG += blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui
DISTFILES += simulator_fsx_config.json

DESTDIR = $$BuildRoot/bin/plugins/simulator

load(common_post)

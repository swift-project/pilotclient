load(common_pre)

QT       += core widgets dbus

TARGET = simulator_xplane_config
TEMPLATE = lib
CONFIG += plugin shared
CONFIG += blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui
DISTFILES += simulator_xplane_config.json

DESTDIR = $$DestRoot/bin/plugins/simulator

load(common_post)

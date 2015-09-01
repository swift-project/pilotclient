include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core widgets dbus

TARGET = simulator_xplane_config
TEMPLATE = lib
CONFIG += plugin shared
CONFIG += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui
DISTFILES += simulator_xplane_config.json

DESTDIR = $$BuildRoot/bin/plugins/simulator
include ($$SourceRoot/libraries.pri)

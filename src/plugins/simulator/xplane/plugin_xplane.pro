include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network

TARGET = simulator_xplane
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$BuildRoot/bin/plugins/simulator
include ($$SourceRoot/libraries.pri)

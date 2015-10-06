load(common_pre)

QT       += core dbus network

TARGET = simulator_xplane
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$DestRoot/bin/plugins/simulator

load(common_post)

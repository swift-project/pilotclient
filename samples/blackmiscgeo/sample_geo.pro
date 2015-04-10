include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network

TARGET = sample_vector_geo
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

TEMPLATE = app

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$BuildRoot/bin

include ($$SourceRoot/libraries.pri)

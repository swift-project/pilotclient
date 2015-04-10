include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network

TARGET = sample_quantities_aviation
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$BuildRoot/bin

HEADERS += *.h
SOURCES += *.cpp

include ($$SourceRoot/libraries.pri)

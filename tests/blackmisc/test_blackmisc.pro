include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core testlib dbus network

TARGET = test_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc
CONFIG   += testcase

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$BuildRoot/bin

include ($$SourceRoot/libraries.pri)

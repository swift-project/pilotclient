include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core testlib dbus network gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_blackgui
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blackgui
CONFIG   += testcase

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$BuildRoot/bin

include ($$SourceRoot/libraries.pri)

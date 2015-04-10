include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sample_hotkey
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += blackmisc blackinput blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$BuildRoot/bin

include ($$SourceRoot/libraries.pri)

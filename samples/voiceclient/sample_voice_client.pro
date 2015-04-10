include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network multimedia
QT       -= gui

TARGET = sample_voice_client
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksound blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$BuildRoot/bin

include ($$SourceRoot/libraries.pri)

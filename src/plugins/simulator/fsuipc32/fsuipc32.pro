load(common_pre)

requires(equals(WORD_SIZE,32))

QT       += core dbus xml network

TARGET = fsuipc
TEMPLATE = lib

CONFIG += staticlib

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.c

DESTDIR = $$DestRoot/lib

load(common_post)

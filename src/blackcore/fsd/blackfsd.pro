load(common_pre)

QT += core dbus network

TARGET = blackfsd
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/lib

load(common_post)

include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network

TARGET = sample_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$BuildRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES +=

include ($$SourceRoot/libraries.pri)

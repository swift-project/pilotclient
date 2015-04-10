include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += core dbus network

TARGET = sample_blackmisc_dbus
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src/blackmisc $$SourceRoot/src/blackcore
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$BuildRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += *.txt *.xml

include ($$SourceRoot/libraries.pri)

load(common_pre)

QT       += core dbus network

TARGET = sample_blackmisc_dbus
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src/blackmisc $$SourceRoot/src/blackcore
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += *.txt *.xml

load(common_post)

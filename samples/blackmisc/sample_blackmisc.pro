load(common_pre)

QT       += core dbus network

TARGET = sample_blackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES +=

load(common_post)

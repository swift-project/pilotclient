load(common_pre)

QT       += core testlib dbus network

TARGET = testblackcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore
CONFIG   += testcase
CONFIG   += no_testcase_installs

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/bin

load(common_post)

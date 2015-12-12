load(common_pre)

QT       += core testlib dbus network gui svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testblackgui
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blackgui
CONFIG   += testcase
CONFIG   += no_testcase_installs

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/bin

load(common_post)

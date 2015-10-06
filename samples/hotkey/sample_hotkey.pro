load(common_pre)

QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sample_hotkey
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += blackmisc blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp

DESTDIR = $$DestRoot/bin

load(common_post)

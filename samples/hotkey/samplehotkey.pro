load(common_pre)

QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = samplehotkey
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += blackmisc blackinput blackcore blackgui
CONFIG  -= app_bundle

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)

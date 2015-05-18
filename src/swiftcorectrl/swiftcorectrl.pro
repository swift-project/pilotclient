include (../../config.pri)
include (../../build.pri)

QT       += core dbus network xml multimedia gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftcorectrl
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackmisc blacksound blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$BuildRoot/bin
OTHER_FILES += *.qss

include (../../libraries.pri)

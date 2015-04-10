include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

# widgets required for icon
QT       += core dbus network xml multimedia gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksound blackinput blackcore

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp
OTHER_FILES += swiftcore.ico swiftcore.rc
win32: RC_FILE = swiftcore.rc

DESTDIR = $$BuildRoot/bin
include ($$SourceRoot/libraries.pri)

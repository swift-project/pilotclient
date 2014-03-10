include (../../config.pri)
include (../../build.pri)

QT       += core dbus network multimedia
QT       -= gui

TARGET = sample_voice_client
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksound blackcore

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blacksound.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblacksound.a
DESTDIR = ../../bin

include (../../libraries.pri)

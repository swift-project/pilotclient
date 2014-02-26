include (../../externals.pri)

QT       += core dbus network multimedia
QT       -= gui

TARGET = sample_voice_client
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lblackcore -lblacksound -lblackmisc
LIBS	+= -lvatlib

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blacksound.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblacksound.a
DESTDIR = ../../bin

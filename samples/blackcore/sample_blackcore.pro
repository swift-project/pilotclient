include (../../externals.pri)

QT       += core dbus network xml multimedia

TARGET = sample_blackcore
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc ../../src/blacksound ../../src/blackcore
INCLUDEPATH += . ../../src

LIBS    += -L../../lib  -lblackcore -lblacksound -lblackmisc
LIBS	+= -lvatlib

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blacksound.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblacksound.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += readme.txt

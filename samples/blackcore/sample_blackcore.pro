include (../../externals.pri)

QT       += core dbus network xml multimedia

TARGET = sample_blackcore
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc ../../src/blackcore ../../src/blacksound
INCLUDEPATH += . ../../src

LIBS    += -L../../lib -lblackcore -lblackmisc -lblacksound
LIBS	+= -lvatlib

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blacksound.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblacksound.a
DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += readme.txt

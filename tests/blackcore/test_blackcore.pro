QT       += core testlib dbus

TARGET = test_blackcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

HEADERS += *.h
SOURCES += *.cpp

LIBS += -L../../lib -lblackcore -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

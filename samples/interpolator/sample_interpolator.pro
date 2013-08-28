QT       += core dbus

TARGET = sample_interpolator
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp

LIBS    += -L../../lib -lblackcore -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

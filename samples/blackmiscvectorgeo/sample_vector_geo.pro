QT       += core dbus

TARGET = sample_vector_geo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin

SOURCES += *.cpp
HEADERS += *.h

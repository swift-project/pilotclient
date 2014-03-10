include (../../config.pri)
include (../../build.pri)

QT       += core dbus network

TARGET = sample_vector_geo
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin

SOURCES += *.cpp
HEADERS += *.h

include (../../libraries.pri)

include (../../config.pri)
include (../../build.pri)

QT       += core
QT       -= gui

TARGET = sample_plugin
TEMPLATE = lib

CONFIG += plugin
CONFIG += blackmisc blackcore


DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin

include (../../libraries.pri)

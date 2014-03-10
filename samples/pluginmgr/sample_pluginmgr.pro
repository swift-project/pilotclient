include (../../config.pri)
include (../../build.pri)

QT       += core
QT       -= gui

TARGET = sample_pluginmgr
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

include (../../libraries.pri)

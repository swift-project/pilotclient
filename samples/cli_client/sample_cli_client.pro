QT       += core
QT       -= gui

TARGET = sample_cli_client
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lblackcore -lblackmisc
LIBS	+= -L../../../vatlib -lvatlib

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../../vatlib/vatlib.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../../vatlib/libvatlib.a
                                     #TODO standardize dependency locations

DESTDIR = ../../bin

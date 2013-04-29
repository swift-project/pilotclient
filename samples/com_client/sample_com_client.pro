QT       += core network
QT       -= gui

TARGET = com_client

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




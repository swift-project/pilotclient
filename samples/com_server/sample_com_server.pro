QT       += core network
QT       -= gui

TARGET = com_server
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




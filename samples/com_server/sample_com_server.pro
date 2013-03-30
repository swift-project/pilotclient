QT       += core network
QT       -= gui

TARGET = com_server
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += main.cpp \
           server.cpp

HEADERS += server.h

LIBS    += -L../../lib -lblackmisc

win32:  PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:   PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




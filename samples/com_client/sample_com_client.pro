QT       += core network
QT       -= gui

TARGET = com_client

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += main.cpp\
           client.cpp

HEADERS += client.h

LIBS    += -L../../lib -lblackmisc

win32:  PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:   PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




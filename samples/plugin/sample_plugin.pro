QT       += core
QT       -= gui

TARGET = sample_plugin
TEMPLATE = lib
CONFIG += plugin

DEPENDPATH += . ../../src

INCLUDEPATH += . ../../src

SOURCES += *.cpp

LIBS    += -L../../lib -lblackmisc

win32:  PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:   PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




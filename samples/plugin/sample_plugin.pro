QT       += core
QT       -= gui

TARGET = sample_plugin
TEMPLATE = lib
CONFIG += plugin c++11

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

LIBS    += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin




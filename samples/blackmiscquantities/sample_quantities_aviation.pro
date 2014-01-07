QT       += core dbus network

TARGET = sample_quantities_aviation
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackmisc

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES +=

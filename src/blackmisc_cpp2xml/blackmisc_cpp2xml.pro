QT       += core dbus

TARGET = blackmisc_cpp2xml
TEMPLATE = lib

CONFIG += plugin

DEPENDPATH += . ../../src/blackmisc
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackmisc

win32:!win32-g++*:  PRE_TARGETDEPS +=   ../../lib/blackmisc.lib
else:  PRE_TARGETDEPS +=    ../../lib/libblackmisc.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

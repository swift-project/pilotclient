QT       += core dbus network

TARGET = sample_blackmisc_dbus
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle

DEPENDPATH += . ../../src/blackmisc ../../src/blackcore
INCLUDEPATH += . ../../src

LIBS += -L../../lib -lblackcore -lblackmisc
win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += *.txt *.xml

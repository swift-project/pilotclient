include (../../config.pri)
include (../../build.pri)

QT       += core dbus network

TARGET = sample_blacksim
TEMPLATE = app

CONFIG   += console c++11
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blacksim

DEPENDPATH += . ../../src/blackmisc ../../src/blacksim
INCLUDEPATH += . ../../src

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib ../../lib/blacksim.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a ../../lib/libblacksim.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES +=

include (../../libraries.pri)

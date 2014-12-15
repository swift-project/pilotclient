include (../../config.pri)
include (../../build.pri)

QT       += core dbus network

TARGET = sample_blackmisc_dbus
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blacksim

DEPENDPATH += . ../../src/blackmisc ../../src/blackcore  ../../src/blacksim
INCLUDEPATH += . ../../src

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blacksim.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblacksim.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += *.txt *.xml

include (../../libraries.pri)

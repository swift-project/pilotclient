include (../../config.pri)
include (../../build.pri)

QT       += core dbus network

TARGET = sample_cli_client
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blacksim

DEPENDPATH += . ../../src
INCLUDEPATH += . ../../src

SOURCES += *.cpp
HEADERS += *.h

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blacksim.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/blacksim.a

DESTDIR = ../../bin

include (../../libraries.pri)

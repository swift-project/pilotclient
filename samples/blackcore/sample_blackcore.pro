include (../../config.pri)
include (../../build.pri)

QT       += core dbus network xml multimedia

TARGET = sample_blackcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksound blackcore

DEPENDPATH += . ../../src/blackmisc ../../src/blacksound ../../src/blackcore
INCLUDEPATH += . ../../src

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blacksound.lib \
                                     ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblacksound.a \
                                     ../../lib/libblackcore.a

DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += readme.txt blackcore.rc blackcore.ico
RESOURCES += blackcore.qrc
win32: RC_FILE = blackcore.rc

include (../../libraries.pri)

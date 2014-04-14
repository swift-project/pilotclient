include (../../config.pri)
include (../../build.pri)

# widgets required for icon
QT       += core dbus network xml multimedia gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sample_blackcore
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blacksound blackcore blacksim

DEPENDPATH += . ../../src/blackmisc ../../src/blacksound ../../src/blackcore ../../src/blacksim
INCLUDEPATH += . ../../src

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blacksound.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blacksim.lib

else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblacksound.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblacksim.a


DESTDIR = ../../bin

HEADERS += *.h
SOURCES += *.cpp
OTHER_FILES += readme.txt blackcore.rc blackcore.ico
RESOURCES += blackcore.qrc
win32: RC_FILE = blackcore.rc

include (../../libraries.pri)

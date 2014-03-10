include (../../config.pri)
include (../../build.pri)

QT       += core dbus gui network xml multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sample_blackgui
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackmisc blacksound blackcore blackgui

DEPENDPATH += . ../../src/blackmisc ../../src/blackgui ../../src/blacksound ../../src/blackcore
INCLUDEPATH += . ../../src

DESTDIR = ../../bin

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackgui.lib \
                                     ../../lib/blacksound.lib \
                                     ../../lib/blackcore.lib

else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackgui.a \
                                     ../../lib/libblacksound.a \
                                     ../../lib/libblackcore.a

OTHER_FILES += *.qss

include (../../libraries.pri)

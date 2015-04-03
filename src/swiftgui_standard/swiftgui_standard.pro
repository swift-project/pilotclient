include (../../config.pri)
include (../../build.pri)

QT       += core dbus gui svg network xml multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = swiftgui_std
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackmisc blacksound blackinput blackcore blackgui
CONFIG  -= app_bundle

DEPENDPATH += . ../../src/blackmisc ../../src/blackgui ../../src/blacksound ../../src/blackcore  ../../src/blackinput
INCLUDEPATH += . ../../src

DESTDIR = ../../bin

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackgui.lib \
                                     ../../lib/blacksound.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blackinput.lib

else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackgui.a \
                                     ../../lib/libblacksound.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblackinput.a

OTHER_FILES += *.qss

include (../../libraries.pri)

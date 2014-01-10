include (../../externals.pri)

QT       += core dbus gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sample_blackgui
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += c++11

DEPENDPATH += . ../../src/blackmisc ../../src/blackcore ../../src/blackgui
INCLUDEPATH += . ../../src

LIBS    += -L../../lib -lblackcore -lblackmisc -lblackgui
LIBS	+= -lvatlib

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib \
                                     ../../lib/blackcore.lib \
                                     ../../lib/blackgui.lib

else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a \
                                     ../../lib/libblackcore.a \
                                     ../../lib/libblackgui.a

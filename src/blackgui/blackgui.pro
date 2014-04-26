include (../../config.pri)
include (../../build.pri)

QT       += network dbus gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = blackgui
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc blacksim blackcore

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib ../../lib/blacksim.lib ../../lib/blackcore.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a ../../lib/libblacksim.a ../../lib/libblackcore.a

HEADERS += *.h
SOURCES += *.cpp
FORMS   += *.ui
RESOURCES += blackgui.qrc

DESTDIR = ../../lib
OTHER_FILES +=

include (../../libraries.pri)

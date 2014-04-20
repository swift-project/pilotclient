include (../../config.pri)
include (../../build.pri)

QT       += network dbus gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = blackgui
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc

INCLUDEPATH += ..
DEPENDPATH += . ..

LIBS    += -L../../lib -lblackmisc

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

HEADERS += *.h
SOURCES += *.cpp
DESTDIR = ../../lib
OTHER_FILES +=

RESOURCES += blackgui.qrc

include (../../libraries.pri)

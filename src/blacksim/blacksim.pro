include (../../config.pri)
include (../../build.pri)

QT       += network dbus xml

TARGET = blacksim
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackmisc

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a

HEADERS += *.h
SOURCES += *.cpp
HEADERS += $$PWD/fscommon/*.h
SOURCES += $$PWD/fscommon/*.cpp

HEADERS += $$PWD/fsx/*.h
SOURCES += $$PWD/fsx/*.cpp

DESTDIR = ../../lib

include (../../libraries.pri)

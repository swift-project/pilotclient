include (../../config.pri)
include (../../build.pri)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml multimedia

TARGET = blackcore
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc

INCLUDEPATH += ..
DEPENDPATH += . ..

# linux-g++* { QMAKE_CXXFLAGS += -std=c++0x }
# PRECOMPILED_HEADER = stdpch.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

HEADERS += *.h
SOURCES += *.cpp

win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp

    contains(BLACK_CONFIG, FSX) {
        DEFINES += BLACK_WITH_FSX
        HEADERS += $$PWD/fsx/*.h
        SOURCES += $$PWD/fsx/*.cpp
    }
}

win32:!win32-g++*: PRE_TARGETDEPS += ../../lib/blackmisc.lib ../../lib/blacksound.lib
else:              PRE_TARGETDEPS += ../../lib/libblackmisc.a ../../lib/libblacksound.a

DESTDIR = ../../lib

OTHER_FILES += readme.txt *.xml

include (../../libraries.pri)

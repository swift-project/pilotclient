include (../../config.pri)
include (../../build.pri)

QT       += network dbus gui multimedia

TARGET = blackinput
TEMPLATE = lib
CONFIG += staticlib
CONFIG   += blackmisc

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

win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp
}

unix:!macx {
    HEADERS += $$PWD/linux/*.h
    SOURCES += $$PWD/linux/*.cpp
}

macx {
    HEADERS += $$PWD/osx/*.h
    SOURCES += $$PWD/osx/*.cpp
}

DESTDIR = ../../lib
OTHER_FILES +=
RESOURCES +=

include (../../libraries.pri)

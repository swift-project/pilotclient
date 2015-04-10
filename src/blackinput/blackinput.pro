include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += network dbus gui multimedia

TARGET = blackinput
TEMPLATE = lib
CONFIG   += blackmisc

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

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
}

unix:!macx {
    HEADERS += $$PWD/linux/*.h
    SOURCES += $$PWD/linux/*.cpp
}

macx {
    HEADERS += $$PWD/osx/*.h
    SOURCES += $$PWD/osx/*.cpp
    OBJECTIVE_SOURCES += $$PWD/osx/*.mm
}

DESTDIR = $$BuildRoot/lib
DLLDESTDIR = $$BuildRoot/bin

OTHER_FILES +=
RESOURCES +=

include ($$SourceRoot/libraries.pri)

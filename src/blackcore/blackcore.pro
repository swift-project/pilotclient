include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml multimedia

TARGET = blackcore
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc blackinput blacksound

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$BuildRoot/lib
DLLDESTDIR = $$BuildRoot/bin

OTHER_FILES += readme.txt *.xml

include ($$SourceRoot/libraries.pri)

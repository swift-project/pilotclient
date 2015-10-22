load(common_pre)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml multimedia

TARGET = blackcore
TEMPLATE = lib
CONFIG += blackmisc blackinput blacksound

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE BUILD_BLACKCORE_LIB

HEADERS += *.h
HEADERS += $$PWD/settings/*.h
HEADERS += $$PWD/data/*.h

SOURCES += *.cpp
# SOURCES += $$PWD/settings/*.cpp
SOURCES += $$PWD/data/*.cpp

LIBS *= -lvatlib2

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += readme.txt *.xml

load(common_post)

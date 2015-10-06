load(common_pre)

QT       += network dbus gui multimedia

TARGET = blacksound
TEMPLATE = lib
CONFIG += blackmisc

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

# PRECOMPILED_HEADER = stdpch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

DEFINES += LOG_IN_FILE BUILD_BLACKSOUND_LIB

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./sounds/*.wav sounds/readme.txt
COPY_FILES += $$PWD/sounds/*
RESOURCES +=

load(common_post)

QT       += network dbus gui multimedia

TARGET = blacksound
TEMPLATE = lib
CONFIG += staticlib c++11

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
DESTDIR = ../../lib
OTHER_FILES += ./sounds/*.wav sounds/readme.txt
RESOURCES +=

# requires install step
install_sounds.path = $$OUT_PWD/../../bin/sounds
install_sounds.files = sounds/*
INSTALLS += install_sounds


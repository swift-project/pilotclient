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

win32: QMAKE_POST_LINK = copy sounds/* $$OUT_PWD/../../bin/sounds
else: QMAKE_POST_LINK = cp sounds/* $$OUT_PWD/../../bin/sounds

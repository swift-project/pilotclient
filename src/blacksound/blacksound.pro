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

win32 {
# I have to replace / with \ , without xcopy the directory is not created
    PWD_OUT_WIN = \"$$shell_path($$OUT_PWD/../../bin/sounds/)\"
    PWD_WIN = \"$$shell_path($$PWD/sounds/*.*)\"
    QMAKE_POST_LINK = xcopy $$PWD_WIN $$PWD_OUT_WIN /y
} else: QMAKE_POST_LINK = cp sounds/* $$OUT_PWD/../../bin/sounds

include (../../config.pri)
include (../../build.pri)

QT       += network dbus gui multimedia

TARGET = blacksound
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
DESTDIR = ../../lib
OTHER_FILES += ./sounds/*.wav sounds/readme.txt
RESOURCES +=

win32:isEmpty(MINGW_IN_SHELL):  COPY = xcopy /yi
else:                           COPY = cp -r

win32 {
    QMAKE_POST_LINK += $$COPY $$shell_path($$PWD/sounds) \
                          $$shell_path($$OUT_PWD/../../bin/sounds)
}
else {
    QMAKE_POST_LINK += mkdir -p $$shell_path($$OUT_PWD/../../bin) && \
                          $$COPY $$shell_path($$PWD/sounds) \
                          $$shell_path($$OUT_PWD/../../bin)
}

include (../../libraries.pri)

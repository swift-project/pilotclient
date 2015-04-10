include ($$SourceRoot/config.pri)
include ($$SourceRoot/build.pri)

QT       += network dbus gui multimedia

TARGET = blacksound
TEMPLATE = lib
CONFIG += staticlib
CONFIG += blackmisc

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

include ($$SourceRoot/libraries.pri)

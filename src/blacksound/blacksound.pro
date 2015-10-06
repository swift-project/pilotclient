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
RESOURCES +=

win32:isEmpty(MINGW_IN_SHELL):  COPY = xcopy /yi
else:                           COPY = cp -r

win32 {
    QMAKE_POST_LINK += $$COPY $$shell_path($$PWD/sounds)          \
                              $$shell_path($$DestRoot/bin/sounds)
}
else {
    QMAKE_POST_LINK += mkdir -p $$shell_path($$DestRoot/bin) &&   \
                       $$COPY $$shell_path($$PWD/sounds)          \
                              $$shell_path($$DestRoot/bin)
}

load(common_post)

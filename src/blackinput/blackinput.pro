load(common_pre)

QT       += network dbus gui widgets multimedia

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

DEFINES += LOG_IN_FILE BUILD_BLACKINPUT_LIB

HEADERS += *.h
SOURCES += *.cpp

win32 {
    HEADERS += $$PWD/win/*.h
    SOURCES += $$PWD/win/*.cpp

    DEFINES += DIRECTINPUT_VERSION=0x0800

    LIBS *= -ldxguid -lole32 -ldinput8 -lUser32
}

unix:!macx {
    HEADERS += $$PWD/linux/*.h
    SOURCES += $$PWD/linux/*.cpp
}

macx {
    HEADERS += $$PWD/osx/*.h
    SOURCES += $$PWD/osx/*.cpp
    OBJECTIVE_SOURCES += $$PWD/osx/*.mm
    LIBS += -framework CoreFoundation -framework ApplicationServices -framework Foundation -framework AppKit
}

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES +=
RESOURCES +=

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)

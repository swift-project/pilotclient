load(common_pre)

# GUI is required for the matrix classes
# Network for host info etc.
QT       += network dbus xml multimedia

TARGET = blackcore
TEMPLATE = lib
CONFIG += blackconfig blackmisc blackinput blacksound

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

LIBS *= -lvatlib

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += readme.txt *.xml

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)

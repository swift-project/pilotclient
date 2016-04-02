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

OTHER_FILES += ./data/sounds/*.wav ./data/sounds/readme.txt
COPY_FILES += $$PWD/data/sounds/*
RESOURCES +=

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_sounds.path = $$PREFIX/data/sounds
package_sounds.files += data/sounds/*.wav
package_sounds.files += data/sounds/readme.txt
INSTALLS += package_sounds

load(common_post)

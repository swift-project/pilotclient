load(common_pre)

QT       += dbus multimedia

TARGET = blacksound
TEMPLATE = lib
CONFIG += blackconfig blackmisc

swiftConfig(static) {
    CONFIG += staticlib
}

INCLUDEPATH += ..
DEPENDPATH += . ..

DEFINES += LOG_IN_FILE BUILD_BLACKSOUND_LIB

HEADERS += *.h
HEADERS += $$PWD/wav/wavfile.h
HEADERS += $$PWD/dsp/*.h
HEADERS += $$PWD/codecs/*.h
HEADERS += $$PWD/sampleprovider/*.h

SOURCES += *.cpp
SOURCES += $$PWD/wav/wavfile.cpp
SOURCES += $$PWD/dsp/*.cpp
SOURCES += $$PWD/codecs/*.cpp
SOURCES += $$PWD/sampleprovider/*.cpp

LIBS *= -lopus

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./share/sounds/*.wav ./share/sounds/readme.txt
COPY_FILES += $$PWD/share/sounds/*
# RESOURCES +=

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

package_sounds.path = $$PREFIX/share/sounds
package_sounds.files += share/sounds/*.wav
package_sounds.files += share/sounds/readme.txt
INSTALLS += package_sounds

load(common_post)

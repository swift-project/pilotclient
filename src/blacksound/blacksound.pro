# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

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
HEADERS += $$files($$PWD/wav/wavfile.h)
HEADERS += $$files($$PWD/dsp/*.h)
HEADERS += $$files($$PWD/codecs/*.h)
HEADERS += $$files($$PWD/sampleprovider/*.h)

SOURCES += *.cpp
SOURCES += $$files($$PWD/wav/wavfile.cpp)
SOURCES += $$files($$PWD/dsp/*.cpp)
SOURCES += $$files($$PWD/codecs/*.cpp)
SOURCES += $$files($$PWD/sampleprovider/*.cpp)

LIBS *= -lopus

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./share/sounds/*.wav ./share/sounds/readme.txt
COPY_FILES += $$files($$PWD/share/sounds/*)
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

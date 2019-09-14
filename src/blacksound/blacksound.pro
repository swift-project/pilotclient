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

HEADERS += \
    blacksoundexport.h \
    notificationplayer.h \
    audioutilities.h \
    selcalplayer.h \
    soundgenerator.h \
    threadedtonepairplayer.h \
    tonepair.h \
    wav/wavfile.h \

SOURCES += \
    notificationplayer.cpp \
    audioutilities.cpp \
    selcalplayer.cpp \
    soundgenerator.cpp \
    threadedtonepairplayer.cpp \
    tonepair.cpp \
    wav/wavfile.cpp \

include ($$PWD/codecs/codecs.pri)
include ($$PWD/dsp/dsp.pri)
include ($$PWD/sampleprovider/sampleprovider.pri)

LIBS += \
    -lopus \

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

OTHER_FILES += ./share/sounds/*.wav ./share/sounds/readme.txt
COPY_FILES += $$PWD/share/sounds/*
RESOURCES +=

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

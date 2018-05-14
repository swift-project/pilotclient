load(common_pre)

QT += core dbus network

TARGET = simulatorfscommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackconfig blackmisc

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

LIBS *= -lsimulatorplugincommon
addStaticLibraryDependency(simulatorplugincommon)

contains(BLACK_CONFIG, FSUIPC) {
    equals(WORD_SIZE,32) {
        DEFINES += SWIFT_USING_FSUIPC32
    }
    equals(WORD_SIZE,64) {
        DEFINES += SWIFT_USING_FSUIPC64
    }
}

DESTDIR = $$DestRoot/lib

load(common_post)

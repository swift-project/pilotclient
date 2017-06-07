load(common_pre)

REQUIRES += contains(BLACK_CONFIG,P3D)

QT       += core dbus gui network xml

TARGET = simulatorp3d
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore

LIBS +=  -lsimulatorfscommon -lsimulatorfsxcommon -lSimConnect -lFSUIPC_User

# required for FSUIPC
win32:!win32-g++*: QMAKE_LFLAGS += /NODEFAULTLIB:LIBC.lib

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

LIBS += -ldxguid -lole32

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorp3d.json

DESTDIR = $$DestRoot/bin/plugins/simulator

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

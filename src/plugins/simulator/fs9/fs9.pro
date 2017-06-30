load(common_pre)

QT       += core dbus gui network xml

TARGET = simulatorfs9
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

LIBS += -lsimulatorfscommon -lfsuipc -luuid
LIBS += -ldxguid -lole32
addStaticLibraryDependency(simulatorfscommon)
addStaticLibraryDependency(fsuipc)

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorfs9.json

DESTDIR = $$DestRoot/bin/plugins/simulator

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

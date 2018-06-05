load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxconfig
TEMPLATE = lib
CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorfsxconfig.json
LIBS +=  -lsimulatorfsxcommon
DESTDIR = $$DestRoot/bin/plugins/simulator

LIBS *= -L$$EXTERNALS_LIB_DIR/FSX-XPack
LIBS *= -lsimulatorfscommon -lsimulatorfsxcommon -lfsuipc -lSimConnect -lsimulatorplugincommon
LIBS += -ldxguid -lole32
addStaticLibraryDependency(simulatorfscommon)
addStaticLibraryDependency(simulatorfsxcommon)
addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

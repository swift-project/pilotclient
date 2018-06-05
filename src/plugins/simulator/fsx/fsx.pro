load(common_pre)

REQUIRES += contains(BLACK_CONFIG,FSX)

QT += core dbus widgets

TARGET = simulatorfsx
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorfsx.json
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

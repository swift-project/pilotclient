QT += core dbus widgets

TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$DestRoot/bin/plugins/simulator

LIBS += -lsimulatorfsxcommon -lsimulatorfscommon -lfsuipc -lsimulatorplugincommon

include(fsxp3d_include.pri)

addStaticLibraryDependency(simulatorfscommon)
addStaticLibraryDependency(simulatorfsxcommon)
addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

# Ignore linker warning about missing pdb files from Simconnect
msvc: QMAKE_LFLAGS *= /ignore:4099

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_pre)

REQUIRES += contains(BLACK_CONFIG,P3D)

QT += core dbus widgets

TARGET = simulatorp3d
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorp3d.json
DESTDIR = $$DestRoot/bin/plugins/simulator

LIBS += -lsimulatorfscommon -lsimulatorfsxcommon -lfsuipc -lsimulatorplugincommon

equals(WORD_SIZE,64) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/P3D-v4.1
    LIBS *= -L$$EXTERNALS_LIB_DIR/P3D-v4.1
    LIBS *= -lAdvapi32
    CONFIG(debug, debug|release): LIBS *= -lSimConnectDebug
    else:                         LIBS *= -lSimConnect
}
equals(WORD_SIZE,32) {
    INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
    LIBS *= -L$$EXTERNALS_LIB_DIR/FSX-XPack
    LIBS *= -lSimConnect
}

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

load(common_post)

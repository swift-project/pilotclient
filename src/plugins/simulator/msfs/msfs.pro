load(common_pre)

QT += core dbus widgets network

TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui
CONFIG += simulatorfsxcommon simulatorfscommon simulatorplugincommon simconnect

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$DestRoot/bin/plugins/simulator

SOURCES += *.cpp
HEADERS += *.h

DEFINES += SIMCONNECT_H_NOMANIFEST
equals(WORD_SIZE,64) {
    SIMCONNECT_INCLUDE += $$EXTERNALSROOT/common/include/simconnect/P3D-v4
    DEFINES += P3D_SDK_VERSION=400
}
equals(WORD_SIZE,32) {
    SIMCONNECT_INCLUDE += $$EXTERNALSROOT/common/include/simconnect/FSX-XPack
}
INCLUDEPATH *= $$SIMCONNECT_INCLUDE
gcc:QMAKE_CXXFLAGS_WARN_ON += -isystem $$SIMCONNECT_INCLUDE
llvm:QMAKE_CXXFLAGS_WARN_ON *= $$clangArg(-isystem$$system_path($$SIMCONNECT_INCLUDE))

REQUIRES += swiftConfig(sims.msfs)
TARGET = simulatormsfs
DISTFILES += simulatormsfs.json

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

load(common_pre)

REQUIRES += contains(BLACK_CONFIG,FSX)

QT       += core dbus gui network xml

TARGET = simulatorfsx
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
INCLUDEPATH *= $$EXTERNALSROOT/common/include/simconnect/FSX-XPack

LIBS *= -L$$EXTERNALS_LIB_DIR/FSX-XPack
LIBS *= -lsimulatorfscommon -lsimulatorfsxcommon -lfsuipc -lSimConnect
LIBS += -ldxguid -lole32
addStaticLibraryDependency(simulatorfscommon)
addStaticLibraryDependency(simulatorfsxcommon)
addStaticLibraryDependency(fsuipc)

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorfsx.json

DESTDIR = $$DestRoot/bin/plugins/simulator

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

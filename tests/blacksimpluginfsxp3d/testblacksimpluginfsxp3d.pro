load(common_pre)

QT       += core testlib dbus network gui widgets

TARGET   = testsimpluginfsxp3d
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blackconfig blackgui
CONFIG   += testcase
CONFIG   += no_testcase_installs

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
HEADERS += *.h
SOURCES += *.cpp
DESTDIR = $$DestRoot/bin

LIBS *= -lsimulatorfsxcommon -lsimulatorfscommon -lfsuipc -lsimulatorplugincommon

# include(../../src/plugins/simulator/fsxp3d.pri)
include(../../src/plugins/simulator/fsxp3d_include.pri)

addStaticLibraryDependency(simulatorfscommon)
addStaticLibraryDependency(simulatorfsxcommon)
addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

# Ignore linker warning about missing pdb files from Simconnect
msvc: QMAKE_LFLAGS *= /ignore:4099

load(common_post)

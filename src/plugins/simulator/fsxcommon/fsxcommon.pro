load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

CONFIG += staticlib
CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui
DESTDIR = $$DestRoot/lib

LIBS += -lsimulatorfscommon -lfsuipc -lsimulatorplugincommon

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

addStaticLibraryDependency(fsuipc)
addStaticLibraryDependency(simulatorplugincommon)

load(common_post)

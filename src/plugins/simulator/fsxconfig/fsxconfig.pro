load(common_pre)

QT += core dbus widgets network

TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackconfig blackmisc blackcore blackgui
CONFIG += simulatorfsxcommon simulatorfscommon simulatorplugincommon fsuipc simconnect

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src
DESTDIR = $$DestRoot/bin/plugins/simulator

SOURCES += *.cpp
HEADERS += *.h

REQUIRES += contains(BLACK_CONFIG,FSX)
TARGET = simulatorfsxconfig
DISTFILES += simulatorfsx.json

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}


load(common_post)

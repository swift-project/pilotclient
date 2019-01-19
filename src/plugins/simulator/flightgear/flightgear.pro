load(common_pre)

QT += core dbus widgets network

TARGET = simulatorflightgear
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blackgui blackconfig simulatorplugincommon

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

unix:!macx {
    INCLUDEPATH *= /usr/include/dbus-1.0
    INCLUDEPATH *= /usr/lib/x86_64-linux-gnu/dbus-1.0/include
}

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorxplane.json
DESTDIR = $$DestRoot/bin/plugins/simulator

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

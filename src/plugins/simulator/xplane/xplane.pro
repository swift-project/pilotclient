# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus widgets network

TARGET = simulatorxplane
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blackgui blackconfig simulatorplugincommon

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

unix:!macx {
    INCLUDEPATH *= /usr/include/dbus-1.0
    exists (/usr/lib/x86_64-linux-gnu){
    INCLUDEPATH *= /usr/lib/x86_64-linux-gnu/dbus-1.0/include
    } else {
    INCLUDEPATH *= /usr/lib/dbus-1.0/include
    }
}

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorxplane.json
DESTDIR = $$DestRoot/bin/plugins/simulator

!qtc_run:isEmpty(XSWIFTBUS_COMMIT): error(Missing XSWIFTBUS_COMMIT variable)
DEFINES += XSWIFTBUS_COMMIT=\\\"$$XSWIFTBUS_COMMIT\\\"

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

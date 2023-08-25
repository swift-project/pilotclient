# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

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

REQUIRES += swiftConfig(sims.p3d)
TARGET = simulatorp3d
DISTFILES += simulatorp3d.json

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

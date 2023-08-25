# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfs9
TEMPLATE = lib

CONFIG += plugin shared
CONFIG += blackmisc blackcore blackgui blackconfig
CONFIG += simulatorfscommon simulatorplugincommon

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

LIBS += -luuid -ldxguid -lole32

SOURCES += *.cpp
HEADERS += *.h
DISTFILES += simulatorfs9.json

DESTDIR = $$DestRoot/bin/plugins/simulator

win32 {
    dlltarget.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/bin/plugins/simulator
    INSTALLS += target
}

load(common_post)

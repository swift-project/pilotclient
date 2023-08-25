# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus xml network widgets

TARGET = simulatorplugincommon
TEMPLATE = lib

CONFIG += blackconfig blackmisc blackcore blackgui

DEPENDPATH  += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

DEFINES += BUILD_SIMULATORPLUGINCOMMON_LIB

SOURCES += *.cpp
HEADERS += *.h

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

win32 {
    dlltarget.path = $$PREFIX/bin
    INSTALLS += dlltarget
} else {
    target.path = $$PREFIX/lib
    INSTALLS += target
}

load(common_post)

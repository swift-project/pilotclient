# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT       += core testlib dbus network gui widgets

TARGET = testblacksimpluginfsxp3d
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blackconfig blackgui
CONFIG   += simulatorfsxcommon simulatorfscommon simulatorplugincommon simconnect
CONFIG   += testcase
CONFIG   += no_testcase_installs

TEMPLATE = app

DEPENDPATH += \
    . \
    $$SourceRoot/src \
    $$SourceRoot/tests

INCLUDEPATH += \
    $$SourceRoot/src \
    $$SourceRoot/tests

HEADERS += *.h
SOURCES += *.cpp
DESTDIR = $$DestRoot/bin

# Ignore linker warning about missing pdb files from Simconnect
msvc: QMAKE_LFLAGS *= /ignore:4099

load(common_post)

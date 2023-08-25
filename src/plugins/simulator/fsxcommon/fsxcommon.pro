# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfsxcommon
TEMPLATE = lib

SOURCES += *.cpp
HEADERS += *.h
FORMS += *.ui

CONFIG += blackconfig blackmisc blackcore blackgui
CONFIG += simulatorfscommon simulatorplugincommon simconnect

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

DEFINES += BUILD_FSXCOMMON_LIB BUID_SIMCONNECT_LIB

OTHER_FILES += \
        *.manifest \
        *.rc

RC_FILE = $$SourceRoot/src/plugins/simulator/fsxcommon/simconnect.rc

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

dlltarget.path = $$PREFIX/bin
INSTALLS += dlltarget

load(common_post)

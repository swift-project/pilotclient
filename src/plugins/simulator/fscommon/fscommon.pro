# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network widgets

TARGET = simulatorfscommon
TEMPLATE = lib

CONFIG += blackconfig blackmisc blackcore blackgui
CONFIG += simulatorplugincommon fsuipc

contains(BLACK_CONFIG, Static) {
    CONFIG += staticlib
}

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

SOURCES += *.cpp
HEADERS += *.h

DEFINES += BUILD_FSCOMMON_LIB

swiftConfig(sims.fsuipc) {
    equals(WORD_SIZE,32) {
        DEFINES += SWIFT_USING_FSUIPC32
    }
    equals(WORD_SIZE,64) {
        DEFINES += SWIFT_USING_FSUIPC64
    }
}

DESTDIR = $$DestRoot/lib
DLLDESTDIR = $$DestRoot/bin

dlltarget.path = $$PREFIX/bin
INSTALLS += dlltarget

load(common_post)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT       += core dbus network

TARGET = sampleblackmiscsim
TEMPLATE = app

CONFIG   -= app_bundle
CONFIG   += console
CONFIG   += blackmisc blacksim blackcore

# actually not belonging in samples "blackmisc"
# but before we have not more tests we keep it here
win32 { CONFIG   += fsuipc simulatorfscommon }

DEPENDPATH += . ../../src/blackmisc
DEPENDPATH += . ../../src/blackcore
DEPENDPATH += . ../../src/plugins/fscommon
INCLUDEPATH += . ../../src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)

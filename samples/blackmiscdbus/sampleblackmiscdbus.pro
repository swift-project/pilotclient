# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network

TARGET = sampleblackmiscdbus
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore

DEPENDPATH += . $$SourceRoot/src/blackmisc $$SourceRoot/src/blackcore
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES += *.txt *.xml

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)

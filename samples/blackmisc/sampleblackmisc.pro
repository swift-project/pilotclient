# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network

TARGET = sampleblackmisc
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += blackmisc blackcore blackgui blackconfig

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

DESTDIR = $$DestRoot/bin

HEADERS += *.h
SOURCES += *.cpp

OTHER_FILES +=

target.path = $$PREFIX/bin
INSTALLS += target

load(common_post)

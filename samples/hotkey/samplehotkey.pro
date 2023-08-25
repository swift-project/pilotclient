# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT       += core gui dbus network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = samplehotkey
TEMPLATE = app

CONFIG   += blackmisc blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src
INCLUDEPATH += . $$SourceRoot/src

HEADERS += *.h
SOURCES += *.cpp

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

macx {
    # Modifies plugin path
    qtconf.path = $$PREFIX/bin/samplehotkey.app/Contents/Resources
    qtconf.files = qt.conf
    INSTALLS += qtconf
}

load(common_post)

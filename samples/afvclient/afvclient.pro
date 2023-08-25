# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += dbus network multimedia gui quick

CONFIG += c++14
CONFIG -= app_bundle
CONFIG += blackmisc blackcore blackconfig

DEPENDPATH += . $$SourceRoot/src/blackmisc
INCLUDEPATH += . $$SourceRoot/src

INCLUDEPATH += $$SourceRoot/src/blackcore/afv
INCLUDEPATH += $$SourceRoot/src/blackmisc/network

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += *.h
SOURCES += *.cpp

DEFINES += _USE_MATH_DEFINES
RESOURCES += qml/qml.qrc

DESTDIR = $$DestRoot/bin

load(common_post)

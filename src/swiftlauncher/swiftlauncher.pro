# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

QT += core dbus network widgets multimedia

TARGET = swiftlauncher
TEMPLATE = app

SOURCES += *.cpp
HEADERS += *.h
FORMS   += *.ui
CONFIG  += blackconfig blackmisc blacksound blackinput blackcore blackgui

DEPENDPATH += . $$SourceRoot/src/blackmisc \
                $$SourceRoot/src/blacksound \
                $$SourceRoot/src/blackcore \
                $$SourceRoot/src/blackgui \
                $$SourceRoot/src/blackinput

INCLUDEPATH += . $$SourceRoot/src

OTHER_FILES += *.qss *.ico *.icns
RESOURCES += swiftlauncher.qrc
RC_ICONS = swiftlauncher.ico
ICON = swiftlauncher.icns

DESTDIR = $$DestRoot/bin

target.path = $$PREFIX/bin
INSTALLS += target

macx {
    QMAKE_TARGET_BUNDLE_PREFIX = "org.swift-project"
    QMAKE_INFO_PLIST = Info.plist

    # Modifies plugin path
    qtconf.path = $$PREFIX/bin/swiftlauncher.app/Contents/Resources
    qtconf.files = qt.conf
    INSTALLS += qtconf
}

load(common_post)

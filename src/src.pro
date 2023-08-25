# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

CONFIG -= declarative_debug
CONFIG -= qml_debug

SUBDIRS += blackconfig
SUBDIRS += blackmisc

swiftConfig(libs.blacksound) {
    SUBDIRS += blacksound
}
swiftConfig(libs.blackinput) {
    SUBDIRS += blackinput
}
swiftConfig(libs.blackcore) {
    SUBDIRS += blackcore
}
swiftConfig(libs.blackgui) {
    SUBDIRS += blackgui
}
swiftConfig(apps.swiftgui) {
    SUBDIRS += swiftguistandard
}
swiftConfig(apps.swiftcore) {
    SUBDIRS += swiftcore
}
swiftConfig(apps.swiftdata) {
    SUBDIRS += swiftdata
}
swiftConfig(apps.swiftlauncher) {
    SUBDIRS += swiftlauncher
}
swiftConfig(sims.xswiftbus) {
    SUBDIRS += xswiftbus
}
SUBDIRS += plugins

load(common_post)

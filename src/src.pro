load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

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
swiftConfig(sims.xplane) {
    SUBDIRS += xswiftbus
}
SUBDIRS += plugins

load(common_post)

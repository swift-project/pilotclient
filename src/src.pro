load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += blackmisc
SUBDIRS += blacksound
SUBDIRS += blackinput
SUBDIRS += blackcore
SUBDIRS += blackgui
SUBDIRS += swiftguistandard
SUBDIRS += swiftcore
SUBDIRS += swiftdata
SUBDIRS += swiftlauncher
SUBDIRS += xbus
SUBDIRS += plugins

load(common_post)

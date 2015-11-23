load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += blackmisc

contains(BLACK_CONFIG, BlackSound) {
    SUBDIRS += blacksound
}

contains(BLACK_CONFIG, BlackInput) {
    SUBDIRS += blackinput
}

contains(BLACK_CONFIG, BlackCore) {
    SUBDIRS += blackcore
    contains(BLACK_CONFIG, BlackGui) {
        SUBDIRS += blackgui
        contains(BLACK_CONFIG, SwiftGui) {
            SUBDIRS += swiftguistandard
        }
        contains(BLACK_CONFIG, SwiftCore) {
            SUBDIRS += swiftcore
        }
        contains(BLACK_CONFIG, SwiftData) {
            SUBDIRS += swiftdata
        }
        contains(BLACK_CONFIG, SwiftLauncher) {
            SUBDIRS += swiftlauncher
        }
    }
}

contains(BLACK_CONFIG, XPlane) {
    SUBDIRS += xbus/xbus.pro
}

SUBDIRS += plugins

load(common_post)

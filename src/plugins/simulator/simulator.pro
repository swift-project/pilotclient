load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

if (contains(BLACK_CONFIG, FSX) | contains(BLACK_CONFIG, FS9)) {
    SUBDIRS += fscommon
}

contains(BLACK_CONFIG, FSX) {
    SUBDIRS += fsx
    SUBDIRS += fsxconfig
}
contains(BLACK_CONFIG, FS9) {
    SUBDIRS += fs9
}

contains(BLACK_CONFIG, XPlane) {
    SUBDIRS += xplane
    SUBDIRS += xplaneconfig
}

load(common_post)

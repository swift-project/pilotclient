load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

contains(BLACK_CONFIG,FSX|FS9|P3D) {
    SUBDIRS += fscommon
}
contains(BLACK_CONFIG,FSX|P3D) {
    SUBDIRS += fsxcommon
}
contains(BLACK_CONFIG,P3D) {
    SUBDIRS += p3d
    SUBDIRS += p3dconfig
}
contains(BLACK_CONFIG,FSX) {
    SUBDIRS += fsx
    SUBDIRS += fsxconfig
}
contains(BLACK_CONFIG,FS9) {
    SUBDIRS += fs9
}
contains(BLACK_CONFIG,XPlane) {
    SUBDIRS += xplane
    SUBDIRS += xplaneconfig
}
load(common_post)

load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += plugincommon
SUBDIRS += emulated
SUBDIRS += emulatedconfig

swiftConfig(sims.fsx)|swiftConfig(sims.fs9)|swiftConfig(sims.p3d) {
    equals(WORD_SIZE,32): SUBDIRS += fsuipc32
    equals(WORD_SIZE,64): SUBDIRS += fsuipc64
    SUBDIRS += fscommon
}
swiftConfig(sims.fsx)|swiftConfig(sims.p3d) {
    SUBDIRS += fsxcommon
}
swiftConfig(sims.p3d) {
    SUBDIRS += p3d
    SUBDIRS += p3dconfig
}
swiftConfig(sims.fsx) {
    SUBDIRS += fsx
    SUBDIRS += fsxconfig
}
swiftConfig(sims.fs9):swiftConfig(sims.fsuipc) {
    SUBDIRS += fs9
}
swiftConfig(sims.xplane) {
    SUBDIRS += xplane
    SUBDIRS += xplaneconfig
}
swiftConfig(sims.fg) {
    SUBDIRS += flightgear
    SUBDIRS += flightgearconfig
}
swiftConfig(sims.msfs) {
    SUBDIRS += msfs
}
load(common_post)

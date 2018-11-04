load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

OTHER_FILES += mkspecs/features/*.prf
OTHER_FILES += mkspecs/features/*.pri
OTHER_FILES += *.supp
OTHER_FILES += .astylerc

CheckMandatoryDependency(gl)
CheckMandatoryDependency(glu)
CheckMandatoryDependency(libpng)
CheckMandatoryDependency(zlib)

swiftConfig(doxygen) {
    SUBDIRS += docs
}
SUBDIRS += resources
SUBDIRS += src
swiftConfig(samples) {
    SUBDIRS += samples
}
swiftConfig(unitTests) {
    SUBDIRS += tests
}

include(install.pri)

load(common_post)

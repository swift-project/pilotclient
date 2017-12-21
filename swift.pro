load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

OTHER_FILES += mkspecs/features/*.prf
OTHER_FILES += mkspecs/features/*.pri
OTHER_FILES += *.supp

CheckMandatoryDependency(gl)
CheckMandatoryDependency(glu)
CheckMandatoryDependency(libpng)
CheckMandatoryDependency(zlib)

contains(BLACK_CONFIG,Doxygen) {
    SUBDIRS += docs
}
SUBDIRS += resources
SUBDIRS += src
contains(BLACK_CONFIG,Samples) {
    SUBDIRS += samples
}
contains(BLACK_CONFIG,Unittests) {
    SUBDIRS += tests
}

include(install.pri)

load(common_post)

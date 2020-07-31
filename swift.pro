load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

OTHER_FILES += mkspecs/features/*.prf
OTHER_FILES += mkspecs/features/*.pri
OTHER_FILES += *.supp
OTHER_FILES += .astylerc
OTHER_FILES += $$SWIFT_CONFIG_JSON

!qtc_run {
    CheckMandatoryDependency(gl)
    CheckMandatoryDependency(glu)
    CheckMandatoryDependency(libpng)
    CheckMandatoryDependency(zlib)
}

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

llvm|gcc:  system($$QMAKE_CXX --version)
else:msvc: system($$QMAKE_CXX)

include(install.pri)

include(src/xswiftbus/version.pri)

load(common_post)

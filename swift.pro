load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

OTHER_FILES += mkspecs/features/*.prf
OTHER_FILES += mkspecs/features/*.pri

SUBDIRS += docs
SUBDIRS += resources
SUBDIRS += src
SUBDIRS += samples
SUBDIRS += tests

include(install.pri)

load(common_post)

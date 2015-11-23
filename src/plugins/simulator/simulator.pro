load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += fscommon
SUBDIRS += fsx
SUBDIRS += fsxconfig
SUBDIRS += fs9
SUBDIRS += xplane
SUBDIRS += xplaneconfig

load(common_post)

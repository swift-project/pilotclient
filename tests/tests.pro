load(common_pre)

REQUIRES += contains(BLACK_CONFIG,Unittests)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += blackmisc
SUBDIRS += blackcore
SUBDIRS += blackgui

load(common_post)

load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += testblackmisc
SUBDIRS += testblackcore
SUBDIRS += testblackgui

testblackmisc.file = blackmisc/testblackmisc.pro
testblackcore.file = blackcore/testblackcore.pro
testblackgui.file  = blackgui/testblackgui.pro

contains(BLACK_CONFIG,FSX|P3D) {
    SUBDIRS += testsimpluginfsxp3d
    testsimpluginfsxp3d.file = blacksimpluginfsxp3d/testblacksimpluginfsxp3d.pro
}

load(common_post)

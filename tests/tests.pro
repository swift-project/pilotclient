# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += blackmisc
SUBDIRS += blackcore
SUBDIRS += blackgui

# testblackmisc.file = blackmisc/testblackmisc.pro
# testblackcore.file = blackcore/testblackcore.pro
# testblackgui.file  = blackgui/testblackgui.pro

swiftConfig(sims.fsx)|swiftConfig(sims.p3d) {
    SUBDIRS += testsimpluginfsxp3d
    testsimpluginfsxp3d.file = blacksimpluginfsxp3d/testblacksimpluginfsxp3d.pro
}

load(common_post)

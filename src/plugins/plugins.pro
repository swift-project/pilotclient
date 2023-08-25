# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: CC0-1.0

load(common_pre)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += simulator
SUBDIRS += weatherdata

load(common_post)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

include(${PROJECT_SOURCE_DIR}/cmake/swift_test.cmake)

add_swift_test(
        NAME simpluginfsx
        SOURCES simpluginfsxp3dtest.h testsimpluginfsxp3d.cpp simpluginfsxp3dtest.h
        LINK_LIBRARIES Qt::Core fsxcommon tests_test
)

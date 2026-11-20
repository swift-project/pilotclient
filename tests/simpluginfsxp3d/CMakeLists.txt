# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

include(${PROJECT_SOURCE_DIR}/cmake/swift_test.cmake)

if(SWIFT_BUILD_P3D_PLUGIN)
    set(FSXCOMMON_TEST_TARGET fsxcommon_p3d)
elseif(SWIFT_BUILD_MSFS_PLUGIN)
    set(FSXCOMMON_TEST_TARGET fsxcommon_msfs)
else()
    set(FSXCOMMON_TEST_TARGET fsxcommon_msfs2024)
endif()

add_swift_test(
        NAME simpluginfsx
        SOURCES simpluginfsxp3dtest.h testsimpluginfsxp3d.cpp simpluginfsxp3dtest.h
        LINK_LIBRARIES Qt::Core ${FSXCOMMON_TEST_TARGET} tests_test
)

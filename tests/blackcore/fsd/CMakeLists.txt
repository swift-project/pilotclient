# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

include(${PROJECT_SOURCE_DIR}/cmake/swift_test.cmake)

add_swift_test(
        NAME core_fsdclient
        SOURCES testfsdclient/testfsdclient.cpp
        LINK_LIBRARIES core misc Qt::Test tests_test
)

add_swift_test(
        NAME core_fsdmessages
        SOURCES testfsdmessages/testfsdmessages.cpp
        LINK_LIBRARIES blackconfig core tests_test Qt::Core Qt::Test
)

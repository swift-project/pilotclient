# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

include(${PROJECT_SOURCE_DIR}/cmake/swift_test.cmake)

add_swift_test(
        NAME gui_guiutility
        SOURCES testguiutility/testguiutility.cpp testguiutility/testguiutility.h
        LINK_LIBRARIES gui tests_test Qt::Core
)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_custom_target(resources)

add_custom_command(TARGET resources
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different share/ ${PROJECT_BINARY_DIR}/out/share/
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(DIRECTORY share/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share)

# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_subdirectory(blackconfig)
add_subdirectory(blackcore)
add_subdirectory(blackmisc)
add_subdirectory(blackgui)
add_subdirectory(blackinput)
add_subdirectory(blacksound)

add_subdirectory(swiftguistandard)
add_subdirectory(swiftdata)
add_subdirectory(swiftcore)
add_subdirectory(swiftlauncher)

add_subdirectory(plugins)

if(SWIFT_BUILD_XSWIFTBUS)
    add_subdirectory(xswiftbus)
endif()

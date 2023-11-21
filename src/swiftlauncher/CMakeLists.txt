# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(swiftlauncher WIN32
        main.cpp
        swiftlauncher.cpp
        swiftlauncher.h
        swiftlauncher.ui
        swiftlauncher.qrc
        )

if(WIN32)
    target_sources(swiftlauncher PRIVATE data.rc)
endif()

target_include_directories(swiftlauncher PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(swiftlauncher
        PUBLIC
        Qt::Core
        misc
        gui
        core
        )

if(APPLE)
    set_target_properties(swiftlauncher PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(swiftlauncher PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set(RESOURCE_FILES swiftlauncher.icns qt.conf)
    target_sources(swiftlauncher PRIVATE ${RESOURCE_FILES})
    set_target_properties(swiftlauncher PROPERTIES RESOURCE "${RESOURCE_FILES}")
endif()

add_dependencies(swiftlauncher resources)

if(WIN32)
        add_dependencies(swiftlauncher copy_externals_to_build_dir)
endif()

if(APPLE)
    install(TARGETS swiftlauncher BUNDLE DESTINATION bin)
else()
    install(TARGETS swiftlauncher)
endif()

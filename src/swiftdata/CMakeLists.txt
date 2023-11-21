# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(swiftdata WIN32
        main.cpp
        swiftdata.cpp
        swiftdata.h
        swiftdata.ui
        swiftdatamenus.cpp
        )

if(WIN32)
    target_sources(swiftdata PRIVATE data.rc)
endif()

target_include_directories(swiftdata PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(swiftdata
        PUBLIC
        Qt::Core
        misc
        gui
        core
        )

if(APPLE)
    set_target_properties(swiftdata PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(swiftdata PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set(RESOURCE_FILES swiftdata.icns qt.conf)
    target_sources(swiftdata PRIVATE ${RESOURCE_FILES})
    set_target_properties(swiftdata PROPERTIES RESOURCE "${RESOURCE_FILES}")
endif()

add_dependencies(swiftdata resources)

if(WIN32)
        add_dependencies(swiftdata copy_externals_to_build_dir)
endif()

if(APPLE)
    install(TARGETS swiftdata BUNDLE DESTINATION bin)
else()
    install(TARGETS swiftdata)
endif()

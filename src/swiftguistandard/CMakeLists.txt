# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(swiftguistd WIN32
        main.cpp
        swiftguistd.cpp
        swiftguistd.h
        swiftguistd.ui
        swiftguistdaircraft.cpp
        swiftguistdapplication.cpp
        swiftguistdapplication.h
        swiftguistdinit.cpp
        swiftguistdmenus.cpp
        )

if(WIN32)
    target_sources(swiftguistd PRIVATE data.rc)
endif()

target_include_directories(swiftguistd PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(swiftguistd
        PUBLIC
        Qt::Core
        misc
        gui
        core
        sound
        )

force_asserts(swiftguistd)

if(APPLE)
    target_link_libraries(swiftguistd PUBLIC input)

    set_target_properties(swiftguistd PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(swiftguistd PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    set(RESOURCE_FILES swift.icns qt.conf)
    target_sources(swiftguistd PRIVATE ${RESOURCE_FILES})
    set_target_properties(swiftguistd PROPERTIES RESOURCE "${RESOURCE_FILES}")
endif()

add_dependencies(swiftguistd resources)

if(WIN32)
        add_dependencies(swiftguistd copy_externals_to_build_dir)
endif()

if(APPLE)
    install(TARGETS swiftguistd BUNDLE DESTINATION bin)
else()
    install(TARGETS swiftguistd)
endif()

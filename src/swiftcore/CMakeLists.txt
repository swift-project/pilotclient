# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(swiftcore WIN32
        main.cpp
        swiftcore.cpp
        swiftcore.h
        swiftcore.ui
        )

if(WIN32)
    target_sources(swiftcore PRIVATE data.rc)
endif()

target_include_directories(swiftcore PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(swiftcore
        PUBLIC
        Qt::Core
        misc
        gui
        core
        )

add_dependencies(swiftcore resources)

if(WIN32)
        add_dependencies(swiftcore copy_externals_to_build_dir)
endif()

install(TARGETS swiftcore)

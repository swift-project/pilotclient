# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

add_executable(swiftguimini WIN32
        main.cpp
        miniflightplandialog.cpp
        miniflightplandialog.h
        miniguiwindow.cpp
        miniguiwindow.h
        miniMiscSettingsDialog.cpp
        miniMiscSettingsDialog.h
        minisimulatorsettingsdialog.cpp
        minisimulatorsettingsdialog.h
        ministatusbarwidget.cpp
        ministatusbarwidget.h
        swiftguiminiapplication.cpp
        swiftguiminiapplication.h
        miniloginwidget.cpp
        miniloginwidget.h
        )

target_include_directories(swiftguimini PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(swiftguimini
        PUBLIC
        Qt::Core
        Qt::Widgets
        misc
        gui
        core
        )

force_asserts(swiftguimini)

if(APPLE)
    target_link_libraries(swiftguimini PUBLIC input)
endif()
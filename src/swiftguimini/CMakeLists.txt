# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

set(SWIFTGUIMINI_TS_FILES
        i18n/swiftguimini_de_DE.ts
        i18n/swiftguimini_en_US.ts
        i18n/swiftguimini_es_ES.ts
        i18n/swiftguimini_fr_FR.ts
        i18n/swiftguimini_zh_CN.ts
        )

add_executable(swiftguimini WIN32
        main.cpp
        miniflightplandialog.cpp
        miniflightplandialog.h
        miniguiwindow.cpp
        miniguiwindow.h
        miniMiscSettingsDialog.cpp
        miniMiscSettingsDialog.h
        miniloginsettingsdialog.cpp
        miniloginsettingsdialog.h
        minisimulatorsettingsdialog.cpp
        minisimulatorsettingsdialog.h
        ministatusbarwidget.cpp
        ministatusbarwidget.h
        swiftguiminiapplication.cpp
        swiftguiminiapplication.h
        miniloginwidget.cpp
        miniloginwidget.h
        ${SWIFTGUIMINI_TS_FILES}
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

# i18n: generate .ts -> .qm translations at build time and copy them next to the
# executable (main.cpp loads them at runtime via QTranslator).
qt_add_translation(SWIFTGUIMINI_QM_FILES ${SWIFTGUIMINI_TS_FILES})

add_custom_target(swiftguimini_translations ALL DEPENDS ${SWIFTGUIMINI_QM_FILES})
add_dependencies(swiftguimini swiftguimini_translations)

add_custom_command(TARGET swiftguimini POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:swiftguimini>/i18n"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SWIFTGUIMINI_QM_FILES} "$<TARGET_FILE_DIR:swiftguimini>/i18n"
        COMMENT "Copying swiftguimini translations"
        )

install(FILES ${SWIFTGUIMINI_QM_FILES} DESTINATION bin/i18n)
# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

set(TS_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/swift_en.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/swift_de.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/swift_fr.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/swift_es.ts
    ${CMAKE_CURRENT_SOURCE_DIR}/swift_zh_CN.ts
)

# qt_create_translation scans source directories for translatable strings,
# updates the .ts files, and compiles them to .qm binary files.
qt_create_translation(QM_FILES
    ${CMAKE_SOURCE_DIR}/src/gui
    ${CMAKE_SOURCE_DIR}/src/swiftguistandard
    ${CMAKE_SOURCE_DIR}/src/swiftlauncher
    ${TS_FILES}
)

# Custom target so "cmake --build . --target translations" regenerates .qm files
add_custom_target(translations ALL DEPENDS ${QM_FILES})

# Copy .qm files to the build output share/translations directory
add_custom_command(TARGET translations POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/out/share/translations
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} ${PROJECT_BINARY_DIR}/out/share/translations/
    COMMENT "Copying .qm translation files to build output"
)

# Install .qm files alongside the application
install(FILES ${QM_FILES} DESTINATION share/translations)

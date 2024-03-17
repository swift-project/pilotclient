# SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

message(STATUS "Running windeployqt")
execute_process(COMMAND qmake6 -query QT_INSTALL_BINS OUTPUT_VARIABLE QT_INSTALL_BINS)

string(STRIP ${QT_INSTALL_BINS} QT_INSTALL_BINS)

execute_process(COMMAND ${QT_INSTALL_BINS}/windeployqt.exe
                        ${CMAKE_INSTALL_PREFIX}/bin/core.dll
                        ${CMAKE_INSTALL_PREFIX}/bin/gui.dll
                        ${CMAKE_INSTALL_PREFIX}/bin/input.dll
                        ${CMAKE_INSTALL_PREFIX}/bin/misc.dll
                        ${CMAKE_INSTALL_PREFIX}/bin/sound.dll
                        ${CMAKE_INSTALL_PREFIX}/bin/swiftcore.exe
                        ${CMAKE_INSTALL_PREFIX}/bin/swiftdata.exe
                        ${CMAKE_INSTALL_PREFIX}/bin/swiftguistd.exe
                        ${CMAKE_INSTALL_PREFIX}/bin/swiftlauncher.exe
                        --no-translations
                        --no-compiler-runtime
                        --no-system-d3d-compiler
                        --no-system-dxc-compiler
                        --no-opengl-sw
                        -opengl # not detected automatically
                        --skip-plugin-types generic,multimedia,networkinformation,qmltooling
                        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

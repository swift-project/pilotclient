message(STATUS "Running windeployqt")
execute_process(COMMAND qmake -query QT_INSTALL_BINS OUTPUT_VARIABLE QT_INSTALL_BINS)

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
                        --no-opengl-sw
                        --no-webkit2
                        --no-virtualkeyboard
                        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)

# Remove unused files
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/bin/playlistformats ${CMAKE_INSTALL_PREFIX}/bin/styles)

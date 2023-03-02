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

add_dependencies(swiftlauncher resources)

install(TARGETS swiftlauncher)

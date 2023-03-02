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

install(TARGETS swiftcore)

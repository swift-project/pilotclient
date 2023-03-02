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

add_dependencies(swiftdata resources)

install(TARGETS swiftdata)

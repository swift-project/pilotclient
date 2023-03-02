add_library(simulatorflightgearconfig SHARED
        simulatorflightgearconfig.cpp
        simulatorflightgearconfig.h
        simulatorflightgearconfigwindow.cpp
        simulatorflightgearconfigwindow.h
        simulatorflightgearconfigwindow.ui
        )

set_target_properties(simulatorflightgearconfig PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorflightgearconfig PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorflightgearconfig PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorflightgearconfig
        PUBLIC
        gui
        )

install(TARGETS simulatorflightgearconfig
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

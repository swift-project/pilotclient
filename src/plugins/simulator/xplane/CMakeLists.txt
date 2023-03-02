add_library(simulatorxplane SHARED
        simulatorxplane.cpp
        simulatorxplane.h
        simulatorxplane.json
        xplanempaircraft.cpp
        xplanempaircraft.h
        xswiftbusserviceproxy.cpp
        xswiftbusserviceproxy.h
        xswiftbustrafficproxy.cpp
        xswiftbustrafficproxy.h
        xswiftbusweatherproxy.cpp
        xswiftbusweatherproxy.h
)

include(${PROJECT_SOURCE_DIR}/cmake/xswiftbus_tools.cmake)

get_xswiftbus_commit_id()
target_compile_definitions(simulatorxplane PRIVATE XSWIFTBUS_COMMIT="${XSWIFTBUS_COMMIT_ID}")

set_target_properties(simulatorxplane PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)
set_target_properties(simulatorxplane PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/out/bin/plugins/simulator)

target_include_directories(simulatorxplane PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(simulatorxplane
        PUBLIC
        misc
        externals_dbus
        plugincommon
)

install(TARGETS simulatorxplane
    LIBRARY DESTINATION bin/plugins/simulator
    RUNTIME DESTINATION bin/plugins/simulator
)

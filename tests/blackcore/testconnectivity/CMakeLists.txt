add_executable(tests_blackcore_connectivity testconnectivity.cpp)
target_link_libraries(tests_blackcore_connectivity PUBLIC core misc tests_test Qt::Core Qt::Test)

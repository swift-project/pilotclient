add_executable(tests_blackcore_readers testreaders.cpp)
target_link_libraries(tests_blackcore_readers PUBLIC core misc tests_test Qt::Core Qt::Test)

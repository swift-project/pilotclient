add_executable(tests_blackcore_context testcontext/testcontext.cpp)
target_link_libraries(tests_blackcore_context PUBLIC core misc Qt::Test tests_test)

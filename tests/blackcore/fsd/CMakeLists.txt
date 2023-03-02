add_executable(tests_blackcore_fsdclient testfsdclient/testfsdclient.cpp)
target_link_libraries(tests_blackcore_fsdclient PUBLIC blackconfig core misc tests_test Qt::Core Qt::Test)

add_executable(tests_blackcore_fsdmessages testfsdmessages/testfsdmessages.cpp)
target_link_libraries(tests_blackcore_fsdmessages PUBLIC blackconfig core tests_test Qt::Core Qt::Test)

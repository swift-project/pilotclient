add_library(tests_test INTERFACE test.h)
target_link_libraries(tests_test INTERFACE Qt::Core Qt::Test)
target_include_directories(tests_test INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(blackcore)
add_subdirectory(blackgui)
add_subdirectory(blackmisc)
#add_subdirectory(blacksimpluginfsxp3d)

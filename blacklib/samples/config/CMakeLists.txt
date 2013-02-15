FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_config ${SRC})

TARGET_LINK_LIBRARIES(sample_config blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_config PROPERTIES PROJECT_LABEL "Samples - Config")
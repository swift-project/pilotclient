FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_logging ${SRC})

TARGET_LINK_LIBRARIES(sample_logging blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_logging PROPERTIES PROJECT_LABEL "Samples - Logging")
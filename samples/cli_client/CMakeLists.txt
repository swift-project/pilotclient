FILE(GLOB SRC *.cpp)
SET(HEADERS *.h)

ADD_EXECUTABLE(sample_cli_client ${SRC})

TARGET_LINK_LIBRARIES(sample_cli_client blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_cli_client PROPERTIES PROJECT_LABEL "Samples - CLI Client")
FILE(GLOB SRC *.cpp)

ADD_LIBRARY(sample_plugin MODULE ${SRC})

TARGET_LINK_LIBRARIES(sample_plugin blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_plugin PROPERTIES PROJECT_LABEL "Samples - Plugin")
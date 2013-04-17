FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_pluginmgr ${SRC})

TARGET_LINK_LIBRARIES(sample_pluginmgr blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_pluginmgr PROPERTIES PROJECT_LABEL "Samples - Plugin Manager")
FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_Geodetic2Ecef ${SRC})

TARGET_LINK_LIBRARIES(sample_Geodetic2Ecef blackmisc blackcore ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_Geodetic2Ecef PROPERTIES PROJECT_LABEL "Samples - Geodetic to Ecef")
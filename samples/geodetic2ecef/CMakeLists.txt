FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_geodetic2ecef ${SRC})

TARGET_LINK_LIBRARIES(sample_geodetic2ecef blackmisc blackcore ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_geodetic2ecef PROPERTIES PROJECT_LABEL "Samples - Geodetic to Ecef")
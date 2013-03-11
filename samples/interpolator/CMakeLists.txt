FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(sample_interpolator ${SRC})

TARGET_LINK_LIBRARIES(sample_interpolator blackmisc blackcore ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_interpolator PROPERTIES PROJECT_LABEL "Samples - Interpolator")
FILE(GLOB blackcore_SOURCES *.cpp)
FILE(GLOB blackcore_HEADERS *.h)
SET(blackcore_HEADERS_QOBJECT 
			fsd_client.h)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		
QT4_WRAP_CPP(blackcore_HEADERS_MOC ${blackcore_HEADERS_QOBJECT})

SOURCE_GROUP(QtGeneratedMocSrc FILES ${blackcore_HEADERS_MOC})
SOURCE_GROUP (Headers FILES ${blackcore_HEADERS})

IF(WITH_STATIC)
    ADD_LIBRARY(blackcore STATIC ${blackcore_SOURCES} ${blackcore_HEADERS_MOC})
ELSE(WITH_STATIC)
    ADD_LIBRARY(blackcore SHARED ${blackcore_SOURCES} ${blackcore_HEADERS_MOC})
ENDIF(WITH_STATIC)

TARGET_LINK_LIBRARIES(blackcore)
SET_TARGET_PROPERTIES(blackcore PROPERTIES PROJECT_LABEL "Library - BlackCore")
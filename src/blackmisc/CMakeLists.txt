FILE(GLOB blackmisc_SOURCES *.cpp)
FILE(GLOB blackmisc_HEADERS *.h)
SET(blackmisc_HEADERS_QOBJECT
		com_client.h
		com_client_buffer.h
		com_handler.h
		com_server.h)
		
QT4_WRAP_CPP(blackmisc_HEADERS_MOC ${blackmisc_HEADERS_QOBJECT})

SOURCE_GROUP(QtGeneratedMocSrc FILES ${blackmisc_HEADERS_MOC})
SOURCE_GROUP (Headers FILES ${blackmisc_HEADERS})

IF(WITH_STATIC)
    ADD_LIBRARY(blackmisc STATIC ${blackmisc_SOURCES} ${blackmisc_HEADERS_MOC})
ELSE(WITH_STATIC)
    ADD_LIBRARY(blackmisc SHARED ${blackmisc_SOURCES} ${blackmisc_HEADERS_MOC})
ENDIF(WITH_STATIC)

TARGET_LINK_LIBRARIES(blackmisc)
SET_TARGET_PROPERTIES(blackmisc PROPERTIES PROJECT_LABEL "Library - BlackMisc")
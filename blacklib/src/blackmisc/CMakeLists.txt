FILE(GLOB blackmisc_SOURCES *.cpp)
FILE(GLOB blackmisc_HEADERS ../../include/blackmisc/*.h)
SET(blackmisc_HEADERS_QOBJECT
		../../include/blackmisc/com_client.h
		../../include/blackmisc/com_client_buffer.h
		../../include/blackmisc/com_handler.h
		../../include/blackmisc/com_server.h)
		
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
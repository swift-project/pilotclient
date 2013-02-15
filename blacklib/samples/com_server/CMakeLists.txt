FILE(GLOB sample_com_server_SOURCES *.cpp)
SET(sample_com_server_HEADERS server.h)

QT4_WRAP_CPP(sample_com_server_HEADERS_MOC ${sample_com_server_HEADERS})

ADD_EXECUTABLE(sample_com_server ${sample_com_server_SOURCES} ${sample_com_server_HEADERS_MOC})

TARGET_LINK_LIBRARIES(sample_com_server blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_com_server PROPERTIES PROJECT_LABEL "Samples - Com Server")
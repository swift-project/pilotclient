FILE(GLOB sample_com_client_SOURCES *.cpp)
SET(sample_com_client_HEADERS client.h)

QT4_WRAP_CPP(sample_com_client_HEADERS_MOC ${sample_com_client_HEADERS})

ADD_EXECUTABLE(sample_com_client ${sample_com_client_SOURCES} ${sample_com_client_HEADERS_MOC})

TARGET_LINK_LIBRARIES(sample_com_client blackmisc ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(sample_com_client PROPERTIES PROJECT_LABEL "Samples - Com client")
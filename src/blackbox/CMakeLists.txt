FILE(GLOB blackbox_SOURCES *.cpp)
FILE(GLOB blackbox_HEADERS *.h)
SET(blackbox_HEADERS_QOBJECT
		blackbox.h
		dialog_connect.h
		dialog_chat.h)
		
SET(blackbox_FORMS 	blackbox.ui
					dialog_connect.ui
					dialog_chat.ui)
SET(blackbox_RESOURCES )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
		
QT4_WRAP_CPP(blackbox_HEADERS_MOC ${blackbox_HEADERS_QOBJECT})
QT4_WRAP_UI(blackbox_FORMS_HEADERS ${blackbox_FORMS})
QT4_ADD_RESOURCES(blackbox_RESOURCES_RCC ${blackbox_RESOURCES})

SOURCE_GROUP(QtGeneratedMocSrc FILES ${blackbox_HEADERS_MOC})
SOURCE_GROUP (QtResources FILES ${blackbox_RESOURCES_RCC})
SOURCE_GROUP (QtForms FILES ${blackbox_FORMS_HEADERS})

ADD_EXECUTABLE(blackbox WIN32 MACOSX_BUNDLE ${blackbox_SOURCES}
			${blackbox_HEADERS_MOC}
			${blackbox_FORMS_HEADERS}
			${blackbox_RESOURCES_RCC})

TARGET_LINK_LIBRARIES(blackbox blackmisc ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY})
SET_TARGET_PROPERTIES(blackbox PROPERTIES PROJECT_LABEL "BlackBox GUI - BlackBox")
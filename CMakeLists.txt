cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})
find_package(XPSDK REQUIRED)


if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DLIN=1)
	find_package(PNG)
	#set(XPMP_PLATFORM_SOURCES
	#	src/PlatformUtils.lin.cpp)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	add_definitions(-DIBM=1)
	#set(XPMP_PLATFORM_SOURCES
	#	src/PlatformUtils.win.cpp)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	add_definitions(-DAPL=1)
	#set(XPMP_PLATFORM_SOURCES
	#	src/PlatformUtils.mac.cpp)
endif()
if(PNG_FOUND)
	add_definitions(-DBITMAP_USE_PNG=1)
endif()

add_definitions(-DXPLM200=1 -DXPLM210=1)

add_library(xplanemp
	${XPMP_PLATFORM_SOURCES}
	src/BitmapUtils.cpp
	src/TexUtils.cpp
	src/XObjDefs.cpp
	src/XObjReadWrite.cpp
	src/XOGLUtils.cpp
	src/XPCAircraft.cpp
	src/XPMPMultiplayer.cpp
	src/XPMPMultiplayerCSL.cpp
	src/XPMPMultiplayerObj8.cpp
	src/XPMPMultiplayerObj.cpp
	src/XPMPMultiplayerVars.cpp
	src/XPMPPlaneRenderer.cpp
	src/XUtils.cpp)
target_include_directories(xplanemp
	PUBLIC 
		${XPSDK_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${PNG_INCLUDE_DIRS})
target_link_libraries(xplanemp
	PUBLIC ${XPSDK_XPLM_LIBRARIES} ${PNG_LIBRARIES})
set_target_properties(xplanemp PROPERTIES POSITION_INDEPENDENT_CODE ON)

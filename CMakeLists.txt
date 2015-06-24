cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

find_package(XPSDK REQUIRED)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_definitions(-DLIN=1)
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
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${XPSDK_XPLM_INCLUDE_DIRS}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(xplanemp
	PUBLIC ${XPSDK_XPLM_LIBRARIES})

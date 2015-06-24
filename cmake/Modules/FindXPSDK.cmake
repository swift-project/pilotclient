find_path(XPSDK_DIR 
	NAMES 
	CHeaders/XPLM/XPLMDefs.h 
	CHeaders/Wrappers/XPCDisplay.h) 
if(XPSDK_DIR)
	set(XPSDK_FOUND TRUE)
	if(CMAKE_SIZE_OF_VOID_P EQUAL 8)
		set(WSIZE "_64")
	endif()
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		set(XPSDK_ARCH "Lin")
		set(XPSDK_DEFINITIONS "-DLIN=1")
	elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(XPSDK_ARCH "Win")
		set(XPSDK_DEFINITIONS "-DIBM=1")
	elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set(XPSDK_ARCH "Mac")
		set(XPSDK_DEFINITIONS "-DAPL=1")
	endif()

	set(XPSDK_INCLUDE_DIRS ${XPSDK_DIR}/CHeaders/XPLM ${XPSDK_DIR}/CHeaders/Widgets ${XPSDK_DIR}/CHeaders/Wrappers) 
	set(XPSDK_LIBRARY_DIRS ${XPSDK_DIR}/Libraries/${XPSDK_ARCH})
	file(GLOB XPSDK_WRAPPERS_SOURCES ${XPSDK_DIR}/CHeaders/Wrappers/*.cpp)

	# linux doesn't have linking stubs for XPLM/XPWidgets!
	if(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(XPSDK_XPLM_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPLM${WSIZE}.lib)
		set(XPSDK_XPWIDGETS_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPWidgets${WSIZE}.lib)
	elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set(XPSDK_XPLM_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPLM.framework)
		set(XPSDK_XPWIDGETS_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPWidgets.framework)
	endif()

else()
	set(XPSDK_FOUND)
	set(XPSDK_NOT_FOUND_MESSAGE "Could not find the XPlane Plugin SDK - please set XPSDK_DIR to the top directory of the SDK")
	if(XPSDK_FIND_REQUIRED)
		message(SEND_ERROR "Couldn't find Required XPlane Plugin SDK")
	endif()
endif()


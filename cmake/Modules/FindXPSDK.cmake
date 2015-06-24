find_path(XPSDK_DIR 
	NAMES 
	CHeaders/XPLM/XPLMDefs.h 
	CHeaders/Wrappers/XPCDisplay.h) 
if(XPSDK_DIR)
	set(XPSDK_FOUND)
	if(CMAKE_SIZE_OF_VOID_P EQUAL 8)
		set(WSIZE "_64")
	endif()
	if(CMAKE_SYSTEM_NAME MATCHES "Linux")
		set(XPSDK_ARCH "Lin")
	elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(XPSDK_ARCH "Win")
	elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set(XPSDK_ARCH "Mac")
	endif()

	set(XPSDK_INCLUDE_DIRS ${XPSDK_DIR}/CHeaders)
	set(XPSDK_LIBRARY_DIRS ${XPSDK_DIR}/Libraries/${XPARCH})

	# linux doesn't have linking stubs for XPLM/XPWidgets!
	if(CMAKE_SYSTEM_NAME MATCHES "Windows")
		set(XPSDK_XPLM_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPLM${WSIZE}.lib)
		set(XPSDK_XPWIDGETS_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPWidgets${WSIZE}.lib)
	elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set(XPSDK_XPLM_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPLM.framework)
		set(XPSDK_XPWIDGETS_LIBRARIES ${XPSDK_LIBRARY_DIRS}/XPWidgets.framework)
	endif()

	set(XPSDK_XPLM_INCLUDE_DIRS ${XPSDK_DIR}/CHeaders/XPLM)
	set(XPSDK_XPWIDGETS_INCLUDE_DIRS ${XPSDK_DIR}/CHeaders/Widgets)
	set(XPSDK_WRAPPERS_INCLUDE_DIRS ${XPSDK_DIR}/CHeaders/Wrappers)
	file(GLOB XPSDK_WRAPPERS_SOURCES ${XPSDK_DIR}/CHeaders/Wrappers/*.cpp)
else()
	set(XPSDK_FOUND)
	set(XPSDK_NOT_FOUND_MESSAGE "Could not find the XPlane Plugin SDK - please set XPSDK_DIR to the top directory of the SDK")
	if(XPSDK_FIND_REQUIRED)
		message(SEND_ERROR "Couldn't find Required XPlane Plugin SDK")
	endif()
endif()


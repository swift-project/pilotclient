project(BlackBox)

cmake_minimum_required(VERSION 2.6.3 FATAL_ERROR)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
SET(CMAKE_MODULE_PATH "${BlackBox_SOURCE_DIR}/cmake/modules";${CMAKE_MODULE_PATH})

# Check the build path is out of the source path
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	MESSAGE(FATAL_ERROR " \
	\
	Do not run cmake inside the source path. Try: \
	\
	mkdir build \
	cd build \
	cmake ..")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

############### Build Options ###############

############### Libraries ###############
OPTION(WITH_PCH                 	"With Precompiled Headers"                      ON)

OPTION(WITH_BLACK_LIB                 "Build BlackBox libraries"                           ON )
IF(WITH_BLACK_LIB)
	OPTION(WITH_BLACK_CORE                "Build Core library"                           ON )
	OPTION(WITH_BLACK_MISC                "Build Misc library"                           ON )
	OPTION(WITH_STATIC            	"With static libraries."                        ON )
	OPTION(WITH_BLACK_SAMPLES			"With BlackLib Samples."						ON)
ENDIF(WITH_BLACK_LIB)

IF(WIN32)
	OPTION(WITH_BLACK_DAEMON              "Build BlackBox daemon"                        ON )
	OPTION(WITH_BLACK_GUI             	  "Build BlackBox GUI"                        	 ON )
ENDIF(WIN32)
OPTION(WITH_BLACK_XPLANE              "Build BlackBox as a X-Plane Plugin"        	 ON )

OPTION(WITH_SIMULATOR_DRIVER		"Build Simulator Drivers." 						ON)
IF(WITH_SIMULATOR_DRIVER)
	OPTION(WITH_STATIC_DRIVERS				"Build static simulator drivers"                    OFF )
	############### Drivers ###############
	IF(WIN32)
		OPTION(WITH_DRIVER_FSX			"Build FSX Simulator Driver"                            OFF )
		OPTION(WITH_DRIVER_FS9			"Build FS2004 Simulator Driver"							OFF )
	ENDIF(WIN32)
	OPTION(WITH_DRIVER_XPLANE				"Build X-Plane Simulator Driver"                    OFF )
ENDIF(WITH_SIMULATOR_DRIVER)
 
# By default, force release configuration
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)

ENDIF(NOT CMAKE_BUILD_TYPE)

############### CMake Macros ###############

############### The BlackBox version ###############

SET(BLACKBOX_VERSION_MAJOR "0")
SET(BLACKBOX_VERSION_MINOR "1")
SET(BLACKBOX_VERSION_PATCH "0")
SET(BLACKBOX_VERSION "${BLACKBOX_VERSION_MAJOR}.${BLACKBOX_VERSION_MINOR}.${BLACKBOX_VERSION_PATCH}")
SET(BLACKBOX_VERSION_STRING "${BLACKBOX_VERSION}")

############### Macros ###############

MACRO(MAKE_INSTALL_PATH_ABSOLUTE out in)
   IF(IS_ABSOLUTE "${in}")
      SET(${out} "${in}")
   ELSE()
      SET(${out} "\${BLACKBOX_INSTALL_DIR}/${in}")
   ENDIF()
ENDMACRO()

MACRO(ADD_COMPILER_FLAGS _FLAGS)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAGS}")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS}")
ENDMACRO()

############### Find what we need ###############

IF(APPLE)
    SET(EVENT_INPUT_DEFAULT 1)
    find_library(CORESERVICES_LIBRARY CoreServices)
    find_library(COCOA_LIBRARY Cocoa)
    list(APPEND PLATFORM_LIBS ${COCOA_LIBRARY} ${CORESERVICES_LIBRARY})
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Linux")
ENDIF()

#### Qt4 ####
SET(QT_USE_IMPORTED_TARGETS TRUE)

SET(QT_MIN_VERSION 4.8.0)
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)

# properly set up compile flags (QT_DEBUG/QT_NO_DEBUG, ...)
include(${QT_USE_FILE})

SET(QT_USE_QTNETWORK TRUE)
IF(${WIN32})
    SET(QT_USE_QTMAIN TRUE)
ENDIF(${WIN32})


# Missing in FindQt4.cmake until Dec 2011: setting QT_NO_DEBUG_OUTPUT
IF(CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_TOLOWER)
endIF()

IF(NOT CMAKE_BUILD_TYPE_TOLOWER MATCHES "debug")
   add_definitions(-DQT_NO_DEBUG_OUTPUT)
endIF()

#### SimConnect ####
IF(WITH_DRIVER_FSX)
	find_package(SimConnect REQUIRED)
ENDIF(WITH_DRIVER_FSX)

#### DirectPlay ####
IF(WITH_DRIVER_FS9)
	find_package(DirectPlay REQUIRED)
ENDIF(WITH_DRIVER_FS9)

############### Compilers flags ###############

IF(CMAKE_COMPILER_IS_GNUCXX)
	SET(WARNING_FLAGS_CXX "-Wall")
    SET(WARNING_FLAGS_C   "-Wall")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
   SET (WARNING_FLAGS_CXX "-Wall -Wno-overloaded-virtual")
   SET (WARNING_FLAGS_C   "-Wall")
ENDIF() 

IF(MSVC)
	IF(MSVC11)
		ADD_COMPILER_FLAGS("/Gy- /MP")
	# /Ox is working with VC++ 2010, but custom optimizations don't exist
		SET(RELEASE_CFLAGS "/Ox /GF /GS- ${RELEASE_CFLAGS}")
	# without inlining it's unusable, use custom optimizations again
		SET(DEBUG_CFLAGS "/Od /Ob1 /GF- ${DEBUG_CFLAGS}")
	ELSEIF(MSVC10)
		ADD_COMPILER_FLAGS("/Gy- /MP")
		# /Ox is working with VC++ 2010, but custom optimizations don't exist
		SET(RELEASE_CFLAGS "/Ox /GF /GS- ${RELEASE_CFLAGS}")
		# without inlining it's unusable, use custom optimizations again
		SET(DEBUG_CFLAGS "/Od /Ob1 /GF- ${DEBUG_CFLAGS}")
	ELSEIF(MSVC90)
		ADD_COMPILER_FLAGS("/Gy- /MP")
		# don't use a /O[012x] flag IF you want custom optimizations
		SET(RELEASE_CFLAGS "/Ob2 /Oi /Ot /Oy /GT /GF /GS- ${RELEASE_CFLAGS}")
		# without inlining it's unusable, use custom optimizations again
		SET(DEBUG_CFLAGS "/Ob1 /GF- ${DEBUG_CFLAGS}")
	ELSEIF(MSVC80)
		ADD_COMPILER_FLAGS("/Gy- /Wp64")
		# don't use a /O[012x] flag IF you want custom optimizations
		SET(RELEASE_CFLAGS "/Ox /GF /GS- ${RELEASE_CFLAGS}")
		# without inlining it's unusable, use custom optimizations again
		SET(DEBUG_CFLAGS "/Od /Ob1 ${DEBUG_CFLAGS}")
	ELSE(MSVC11)
		MESSAGE(FATAL_ERROR "Can't determine compiler version ${MSVC_VERSION}")
	ENDIF(MSVC11)

	ADD_COMPILER_FLAGS("/D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS /DWIN32 /D_WINDOWS /Zm1000 /wd4250")
	
	ADD_DEFINITIONS(
      /Zc:wchar_t-                                # Treat wchar_t as built-in type
	)
ENDIF()

############### Configure checks ###############

# redirect the output
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
IF(WIN32)
  SET(LIBRARY_OUTPUT_PATH   ${EXECUTABLE_OUTPUT_PATH})
ELSE()
  SET(LIBRARY_OUTPUT_PATH   ${CMAKE_BINARY_DIR}/lib)
ENDIF()

############### Includes ###############

INCLUDE_DIRECTORIES(${QT_INCLUDES})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)

############### Doxygen ###############

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
IF(DOXYGEN_FOUND)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	ADD_CUSTOM_TARGET(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	COMMENT "Generating API documentation with Doxygen" VERBATIM
)
ENDIF(DOXYGEN_FOUND)


############### build targets ###############

ADD_SUBDIRECTORY(src)
IF(WITH_BLACK_SAMPLES)
	ADD_SUBDIRECTORY(samples)
ENDIF(WITH_BLACK_SAMPLES)
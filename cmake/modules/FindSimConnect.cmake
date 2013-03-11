# Find SimConnect
# Find the include and libraries
# 
# SIMCONNECT_INCLUDE_DIR	-	where to find simconnect.h
# SIMCONNECT_LIBRARY		-	path to simconnect.lib
# SIMCONNECT_FOUND			-	True if SimConnect found

include(FindPackageHandleStandardArgs)

IF(SIMCONNECT_INCLUDE_DIR)
	# We have it already, dont do anything
	SET(DSIMCONNECT_FIND_QUIETLY TRUE)
ENDIF()

FIND_PATH(SIMCONNECT_INCLUDE_DIR 
	NAMES SimConnect.h
	PATHS
	"$ENV{ProgramFiles(x86)}/Microsoft Games/Microsoft Flight Simulator X SDK/SDK/Core Utilities Kit/SimConnect SDK"
	PATH_SUFFIXES inc)
	
FIND_PATH(SIMCONNECT_LIBRARY_DIR
	NAMES SimConnect.lib
	PATHS
	"$ENV{ProgramFiles(x86)}/Microsoft Games/Microsoft Flight Simulator X SDK/SDK/Core Utilities Kit/SimConnect SDK"
	PATH_SUFFIXES lib)
	
# handle the QUIETLY and REQUIRED arguments and set SIMCONNECT_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SimConnect  DEFAULT_MSG
                                  SIMCONNECT_LIBRARY_DIR SIMCONNECT_INCLUDE_DIR)
//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation. All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SIMCONNECT_H_
#define _SIMCONNECT_H_

#pragma once

#ifndef DWORD_MAX
#define DWORD_MAX 0xFFFFFFFF
#endif

#include <float.h>

typedef DWORD SIMCONNECT_OBJECT_ID;
typedef unsigned __int64 QWORD;

//----------------------------------------------------------------------------
//        Constants
//----------------------------------------------------------------------------

static const DWORD SIMCONNECT_UNUSED           = DWORD_MAX;   // special value to indicate unused event, ID
static const DWORD SIMCONNECT_OBJECT_ID_USER   = 0;           // proxy value for User vehicle ObjectID

static const float SIMCONNECT_CAMERA_IGNORE_FIELD   = FLT_MAX;  //Used to tell the Camera API to NOT modify the value in this part of the argument.

static const DWORD SIMCONNECT_CLIENTDATA_MAX_SIZE = 8192;     // maximum value for SimConnect_CreateClientData dwSize parameter


// Notification Group priority values
static const DWORD SIMCONNECT_GROUP_PRIORITY_HIGHEST              =          1;      // highest priority
static const DWORD SIMCONNECT_GROUP_PRIORITY_HIGHEST_MASKABLE     =   10000000;      // highest priority that allows events to be masked
static const DWORD SIMCONNECT_GROUP_PRIORITY_STANDARD             = 1900000000;      // standard priority
static const DWORD SIMCONNECT_GROUP_PRIORITY_DEFAULT              = 2000000000;      // default priority
static const DWORD SIMCONNECT_GROUP_PRIORITY_LOWEST               = 4000000000;      // priorities lower than this will be ignored

//Weather observations Metar strings
static const DWORD MAX_METAR_LENGTH = 2000;

// Maximum thermal size is 100 km.
static const float MAX_THERMAL_SIZE = 100000;
static const float MAX_THERMAL_RATE = 1000;

// Camera system commands
static const DWORD SIMCONNECT_CAMERA_MOVE_LEFT              = 1;
static const DWORD SIMCONNECT_CAMERA_MOVE_RIGHT             = 2;
static const DWORD SIMCONNECT_CAMERA_MOVE_UP                = 3;
static const DWORD SIMCONNECT_CAMERA_MOVE_DOWN              = 4;
static const DWORD SIMCONNECT_CAMERA_MOVE_FORWARD           = 5;
static const DWORD SIMCONNECT_CAMERA_MOVE_BACK              = 6;
static const DWORD SIMCONNECT_CAMERA_PITCH_UP               = 7;
static const DWORD SIMCONNECT_CAMERA_PITCH_DOWN             = 8;
static const DWORD SIMCONNECT_CAMERA_YAW_LEFT               = 9;
static const DWORD SIMCONNECT_CAMERA_YAW_RIGHT              = 10;
static const DWORD SIMCONNECT_CAMERA_ROLL_LEFT              = 11;
static const DWORD SIMCONNECT_CAMERA_ROLL_RIGHT             = 12;
static const DWORD SIMCONNECT_CAMERA_ZOOM_IN                = 13;
static const DWORD SIMCONNECT_CAMERA_ZOOM_OUT               = 14;
static const DWORD SIMCONNECT_CAMERA_RESET_ROTATION         = 15;

// Observer values
static const DWORD SIMCONNECT_OBSERVER_REGIME_TELLURIAN     = 0;    // Earth-based observer, limited by terrain, allowed to go underwater.
static const DWORD SIMCONNECT_OBSERVER_REGIME_TERRESTRIAL   = 1;    // Land-based observer, limited by terrain and water surfaces.
static const DWORD SIMCONNECT_OBSERVER_REGIME_GHOST         = 2;    // Ghost observer, can pass through any object or terrain feature unimpeded.
static const DWORD SIMCONNECT_OBSERVER_FOCUS_WORLD          = 0;    // Automatically shifts focal point to the world (terrain) as the observer is manipulated.
static const DWORD SIMCONNECT_OBSERVER_FOCUS_FIXED          = 1;    // Locks the focus to a fixed distance relative to the observer's position.
static const DWORD SIMCONNECT_OBSERVER_ROTATE_SELF          = 0;    // Observer will rotate about its own origin.
static const DWORD SIMCONNECT_OBSERVER_ROTATE_TARGET        = 1;    // Observer will rotate about is focal point (target).
static const DWORD SIMCONNECT_OBSERVER_AXIS_PITCH           = 0;    // The observer's pitch axis.
static const DWORD SIMCONNECT_OBSERVER_AXIS_ROLL            = 1;    // The observer's roll (bank) axis.
static const DWORD SIMCONNECT_OBSERVER_AXIS_YAW             = 2;    // The observer's yaw (heading) axis.
static const DWORD SIMCONNECT_OBSERVER_SCENE_ORIGIN_SELF    = 0;    // Observer will load scenery near its own origin.
static const DWORD SIMCONNECT_OBSERVER_SCENE_ORIGIN_TARGET  = 1;    // Observer will load scenery near its focal point (target).

// SIMCONNECT_DATA_INITPOSITION.Airspeed
static const DWORD INITPOSITION_AIRSPEED_CRUISE = -1;       // aircraft's cruise airspeed
static const DWORD INITPOSITION_AIRSPEED_KEEP = -2;         // keep current airspeed

// AddToClientDataDefinition dwSizeOrType parameter type values
static const DWORD SIMCONNECT_CLIENTDATATYPE_INT8       = -1;   //  8-bit integer number
static const DWORD SIMCONNECT_CLIENTDATATYPE_INT16      = -2;   // 16-bit integer number
static const DWORD SIMCONNECT_CLIENTDATATYPE_INT32      = -3;   // 32-bit integer number
static const DWORD SIMCONNECT_CLIENTDATATYPE_INT64      = -4;   // 64-bit integer number
static const DWORD SIMCONNECT_CLIENTDATATYPE_FLOAT32    = -5;   // 32-bit floating-point number (float)
static const DWORD SIMCONNECT_CLIENTDATATYPE_FLOAT64    = -6;   // 64-bit floating-point number (double)

// AddToClientDataDefinition dwOffset parameter special values
static const DWORD SIMCONNECT_CLIENTDATAOFFSET_AUTO    = -1;   // automatically compute offset of the ClientData variable

// Open ConfigIndex parameter special value
static const DWORD SIMCONNECT_OPEN_CONFIGINDEX_LOCAL   = -1;   // ignore SimConnect.cfg settings, and force local connection

//----------------------------------------------------------------------------
//        Enum definitions
//----------------------------------------------------------------------------

// Receive data types *ALWAYS ADD TO THE END OF THE ENUM*
enum SIMCONNECT_RECV_ID {
    SIMCONNECT_RECV_ID_NULL,
    SIMCONNECT_RECV_ID_EXCEPTION,
    SIMCONNECT_RECV_ID_OPEN,
    SIMCONNECT_RECV_ID_QUIT,
    SIMCONNECT_RECV_ID_EVENT,
    SIMCONNECT_RECV_ID_EVENT_OBJECT_ADDREMOVE,
    SIMCONNECT_RECV_ID_EVENT_FILENAME,
    SIMCONNECT_RECV_ID_EVENT_FRAME,
    SIMCONNECT_RECV_ID_SIMOBJECT_DATA,
    SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE,
    SIMCONNECT_RECV_ID_WEATHER_OBSERVATION,
    SIMCONNECT_RECV_ID_CLOUD_STATE,
    SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID,
    SIMCONNECT_RECV_ID_RESERVED_KEY,
    SIMCONNECT_RECV_ID_CUSTOM_ACTION,
    SIMCONNECT_RECV_ID_SYSTEM_STATE,
    SIMCONNECT_RECV_ID_CLIENT_DATA,
    SIMCONNECT_RECV_ID_EVENT_WEATHER_MODE,
    SIMCONNECT_RECV_ID_AIRPORT_LIST,
    SIMCONNECT_RECV_ID_VOR_LIST,
    SIMCONNECT_RECV_ID_NDB_LIST,
    SIMCONNECT_RECV_ID_WAYPOINT_LIST,
    SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SERVER_STARTED,
    SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_CLIENT_STARTED,
    SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SESSION_ENDED,
    SIMCONNECT_RECV_ID_EVENT_RACE_END,
    SIMCONNECT_RECV_ID_EVENT_RACE_LAP,
	SIMCONNECT_RECV_ID_OBSERVER_DATA,

    SIMCONNECT_RECV_ID_GROUND_INFO,
    SIMCONNECT_RECV_ID_SYNCHRONOUS_BLOCK,
    SIMCONNECT_RECV_ID_EXTERNAL_SIM_CREATE,
    SIMCONNECT_RECV_ID_EXTERNAL_SIM_DESTROY,
    SIMCONNECT_RECV_ID_EXTERNAL_SIM_SIMULATE,
    SIMCONNECT_RECV_ID_EXTERNAL_SIM_LOCATION_CHANGED,
    SIMCONNECT_RECV_ID_EXTERNAL_SIM_EVENT,
    SIMCONNECT_RECV_ID_EVENT_WEAPON,
    SIMCONNECT_RECV_ID_EVENT_COUNTERMEASURE,
    SIMCONNECT_RECV_ID_EVENT_OBJECT_DAMAGED_BY_WEAPON,
    SIMCONNECT_RECV_ID_VERSION,
    SIMCONNECT_RECV_ID_SCENERY_COMPLEXITY,
    SIMCONNECT_RECV_ID_SHADOW_FLAGS,
    SIMCONNECT_RECV_ID_TACAN_LIST,
    SIMCONNECT_RECV_ID_CAMERA_6DOF,
    SIMCONNECT_RECV_ID_CAMERA_FOV,
    SIMCONNECT_RECV_ID_CAMERA_SENSOR_MODE,
    SIMCONNECT_RECV_ID_CAMERA_WINDOW_POSITION,
    SIMCONNECT_RECV_ID_CAMERA_WINDOW_SIZE,
	SIMCONNECT_RECV_ID_MISSION_OBJECT_COUNT,
    SIMCONNECT_RECV_ID_GOAL,
    SIMCONNECT_RECV_ID_MISSION_OBJECTIVE,
    SIMCONNECT_RECV_ID_FLIGHT_SEGMENT,
    SIMCONNECT_RECV_ID_PARAMETER_RANGE,	
    SIMCONNECT_RECV_ID_FLIGHT_SEGMENT_READY_FOR_GRADING,
    SIMCONNECT_RECV_ID_GOAL_PAIR,
    SIMCONNECT_RECV_ID_EVENT_FLIGHT_ANALYSIS_DIAGRAMS,
    SIMCONNECT_RECV_ID_LANDING_TRIGGER_INFO,
    SIMCONNECT_RECV_ID_LANDING_INFO,
    SIMCONNECT_RECV_ID_SESSION_DURATION,
    SIMCONNECT_RECV_ID_ATTACHPOINT_DATA,
    SIMCONNECT_RECV_ID_PLAYBACK_STATE_CHANGED,
    SIMCONNECT_RECV_ID_RECORDER_STATE_CHANGED,
    SIMCONNECT_RECV_ID_RECORDING_INFO,
    SIMCONNECT_RECV_ID_RECORDING_BOOKMARK_INFO,
    SIMCONNECT_RECV_ID_TRAFFIC_SETTINGS,
    SIMCONNECT_RECV_ID_JOYSTICK_DEVICE_INFO,
    SIMCONNECT_RECV_ID_MOBILE_SCENERY_IN_RADIUS,
    SIMCONNECT_RECV_ID_MOBILE_SCENERY_DATA,
    SIMCONNECT_RECV_ID_EVENT_64,
};



// Data data types
enum SIMCONNECT_DATATYPE {
    SIMCONNECT_DATATYPE_INVALID,        // invalid data type

    SIMCONNECT_DATATYPE_INT32,          // 32-bit integer number
    SIMCONNECT_DATATYPE_INT64,          // 64-bit integer number
    SIMCONNECT_DATATYPE_FLOAT32,        // 32-bit floating-point number (float)
    SIMCONNECT_DATATYPE_FLOAT64,        // 64-bit floating-point number (double)

    SIMCONNECT_DATATYPE_STRING8,        // 8 character narrow string
    SIMCONNECT_DATATYPE_STRING32,       // 32 character narrow string
    SIMCONNECT_DATATYPE_STRING64,       // 64 character narrow string
    SIMCONNECT_DATATYPE_STRING128,      // 128 character narrow string
    SIMCONNECT_DATATYPE_STRING256,      // 256 character narrow string
    SIMCONNECT_DATATYPE_STRING260,      // 260 character narrow string
    SIMCONNECT_DATATYPE_STRINGV,        // variable-length narrow string

    SIMCONNECT_DATATYPE_INITPOSITION,   // see SIMCONNECT_DATA_INITPOSITION
    SIMCONNECT_DATATYPE_MARKERSTATE,    // see SIMCONNECT_DATA_MARKERSTATE
    SIMCONNECT_DATATYPE_WAYPOINT,       // see SIMCONNECT_DATA_WAYPOINT
    SIMCONNECT_DATATYPE_LATLONALT,      // see SIMCONNECT_DATA_LATLONALT
    SIMCONNECT_DATATYPE_XYZ,            // see SIMCONNECT_DATA_XYZ
    SIMCONNECT_DATATYPE_PBH,            // see SIMCONNECT_DATA_PBH
    SIMCONNECT_DATATYPE_OBSERVER,       // see SIMCONNECT_DATA_OBSERVER
    SIMCONNECT_DATATYPE_OBJECT_DAMAGED_BY_WEAPON,   // see SIMCONNECT_DATA_OBJECT_DAMAGED_BY_WEAPON
    SIMCONNECT_DATATYPE_VIDEO_STREAM_INFO,

    SIMCONNECT_DATATYPE_WSTRING8,       // 8 character wide string
    SIMCONNECT_DATATYPE_WSTRING32,      // 32 character wide string
    SIMCONNECT_DATATYPE_WSTRING64,      // 64 character wide string
    SIMCONNECT_DATATYPE_WSTRING128,     // 128 character wide string
    SIMCONNECT_DATATYPE_WSTRING256,     // 256 character wide string
    SIMCONNECT_DATATYPE_WSTRING260,     // 260 character wide string
    SIMCONNECT_DATATYPE_WSTRINGV,       // variable-length wide string

    SIMCONNECT_DATATYPE_MAX             // enum limit
};

// Exception error types
enum SIMCONNECT_EXCEPTION {
    SIMCONNECT_EXCEPTION_NONE,

    SIMCONNECT_EXCEPTION_ERROR,
    SIMCONNECT_EXCEPTION_SIZE_MISMATCH,
    SIMCONNECT_EXCEPTION_UNRECOGNIZED_ID,
    SIMCONNECT_EXCEPTION_UNOPENED,
    SIMCONNECT_EXCEPTION_VERSION_MISMATCH,
    SIMCONNECT_EXCEPTION_TOO_MANY_GROUPS,
    SIMCONNECT_EXCEPTION_NAME_UNRECOGNIZED,
    SIMCONNECT_EXCEPTION_TOO_MANY_EVENT_NAMES,
    SIMCONNECT_EXCEPTION_EVENT_ID_DUPLICATE,
    SIMCONNECT_EXCEPTION_TOO_MANY_MAPS,
    SIMCONNECT_EXCEPTION_TOO_MANY_OBJECTS,
    SIMCONNECT_EXCEPTION_TOO_MANY_REQUESTS,
    SIMCONNECT_EXCEPTION_WEATHER_INVALID_PORT,
    SIMCONNECT_EXCEPTION_WEATHER_INVALID_METAR,
    SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_GET_OBSERVATION,
    SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_CREATE_STATION,
    SIMCONNECT_EXCEPTION_WEATHER_UNABLE_TO_REMOVE_STATION,
    SIMCONNECT_EXCEPTION_INVALID_DATA_TYPE,
    SIMCONNECT_EXCEPTION_INVALID_DATA_SIZE,
    SIMCONNECT_EXCEPTION_DATA_ERROR,
    SIMCONNECT_EXCEPTION_INVALID_ARRAY,
    SIMCONNECT_EXCEPTION_CREATE_OBJECT_FAILED,
    SIMCONNECT_EXCEPTION_LOAD_FLIGHTPLAN_FAILED,
    SIMCONNECT_EXCEPTION_OPERATION_INVALID_FOR_OBJECT_TYPE,
    SIMCONNECT_EXCEPTION_ILLEGAL_OPERATION,
    SIMCONNECT_EXCEPTION_ALREADY_SUBSCRIBED,
    SIMCONNECT_EXCEPTION_INVALID_ENUM,
    SIMCONNECT_EXCEPTION_DEFINITION_ERROR,
    SIMCONNECT_EXCEPTION_DUPLICATE_ID,
    SIMCONNECT_EXCEPTION_DATUM_ID,
    SIMCONNECT_EXCEPTION_OUT_OF_BOUNDS,
    SIMCONNECT_EXCEPTION_ALREADY_CREATED,
    SIMCONNECT_EXCEPTION_OBJECT_OUTSIDE_REALITY_BUBBLE,
    SIMCONNECT_EXCEPTION_OBJECT_CONTAINER,
    SIMCONNECT_EXCEPTION_OBJECT_AI,
    SIMCONNECT_EXCEPTION_OBJECT_ATC,
    SIMCONNECT_EXCEPTION_OBJECT_SCHEDULE,
    SIMCONNECT_EXCEPTION_BLOCK_TIMEOUT,
};

// Object types
enum SIMCONNECT_SIMOBJECT_TYPE {
    SIMCONNECT_SIMOBJECT_TYPE_USER,
    SIMCONNECT_SIMOBJECT_TYPE_ALL,
    SIMCONNECT_SIMOBJECT_TYPE_AIRPLANE,
    SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT = SIMCONNECT_SIMOBJECT_TYPE_AIRPLANE,
    SIMCONNECT_SIMOBJECT_TYPE_HELICOPTER,
    SIMCONNECT_SIMOBJECT_TYPE_BOAT,
    SIMCONNECT_SIMOBJECT_TYPE_GROUND,
	SIMCONNECT_SIMOBJECT_TYPE_WEAPON,
};

// EventState values
enum SIMCONNECT_STATE {
    SIMCONNECT_STATE_OFF,
    SIMCONNECT_STATE_ON,
};

// Object Data Request Period values
enum SIMCONNECT_PERIOD {
    SIMCONNECT_PERIOD_NEVER,
    SIMCONNECT_PERIOD_ONCE,
    SIMCONNECT_PERIOD_VISUAL_FRAME,
    SIMCONNECT_PERIOD_SIM_FRAME,
    SIMCONNECT_PERIOD_SECOND,
};

// ClientData Request Period values
enum SIMCONNECT_CLIENT_DATA_PERIOD {
    SIMCONNECT_CLIENT_DATA_PERIOD_NEVER,
    SIMCONNECT_CLIENT_DATA_PERIOD_ONCE,
    SIMCONNECT_CLIENT_DATA_PERIOD_VISUAL_FRAME,
    SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET,
    SIMCONNECT_CLIENT_DATA_PERIOD_SECOND,
};

enum SIMCONNECT_MISSION_END {
    SIMCONNECT_MISSION_FAILED,
    SIMCONNECT_MISSION_CRASHED,
    SIMCONNECT_MISSION_SUCCEEDED
};

enum SIMCONNECT_TEXT_TYPE {
    SIMCONNECT_TEXT_TYPE_SCROLL_BLACK,
    SIMCONNECT_TEXT_TYPE_SCROLL_WHITE,
    SIMCONNECT_TEXT_TYPE_SCROLL_RED,
    SIMCONNECT_TEXT_TYPE_SCROLL_GREEN,
    SIMCONNECT_TEXT_TYPE_SCROLL_BLUE,
    SIMCONNECT_TEXT_TYPE_SCROLL_YELLOW,
    SIMCONNECT_TEXT_TYPE_SCROLL_MAGENTA,
    SIMCONNECT_TEXT_TYPE_SCROLL_CYAN,
    SIMCONNECT_TEXT_TYPE_PRINT_BLACK = 0x0100,
    SIMCONNECT_TEXT_TYPE_PRINT_WHITE,
    SIMCONNECT_TEXT_TYPE_PRINT_RED,
    SIMCONNECT_TEXT_TYPE_PRINT_GREEN,
    SIMCONNECT_TEXT_TYPE_PRINT_BLUE,
    SIMCONNECT_TEXT_TYPE_PRINT_YELLOW,
    SIMCONNECT_TEXT_TYPE_PRINT_MAGENTA,
    SIMCONNECT_TEXT_TYPE_PRINT_CYAN,
    SIMCONNECT_TEXT_TYPE_MENU = 0x0200,
    SIMCONNECT_TEXT_TYPE_MESSAGE_WINDOW = 0x0300,
};

enum SIMCONNECT_TEXT_RESULT {
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_1,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_2,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_3,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_4,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_5,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_6,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_7,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_8,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_9,
    SIMCONNECT_TEXT_RESULT_MENU_SELECT_10,
    SIMCONNECT_TEXT_RESULT_DISPLAYED = 0x00010000,
    SIMCONNECT_TEXT_RESULT_QUEUED,
    SIMCONNECT_TEXT_RESULT_REMOVED,
    SIMCONNECT_TEXT_RESULT_REPLACED,
    SIMCONNECT_TEXT_RESULT_TIMEOUT,
};

enum SIMCONNECT_WEATHER_MODE {
    SIMCONNECT_WEATHER_MODE_THEME,
    SIMCONNECT_WEATHER_MODE_RWW, // deprecated
    SIMCONNECT_WEATHER_MODE_CUSTOM,
    SIMCONNECT_WEATHER_MODE_GLOBAL,
};

enum SIMCONNECT_FACILITY_LIST_TYPE {
    SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT,
    SIMCONNECT_FACILITY_LIST_TYPE_WAYPOINT,
    SIMCONNECT_FACILITY_LIST_TYPE_NDB,
    SIMCONNECT_FACILITY_LIST_TYPE_VOR,
    SIMCONNECT_FACILITY_LIST_TYPE_TACAN,
    SIMCONNECT_FACILITY_LIST_TYPE_COUNT // invalid 
};

// Camera types
enum SIMCONNECT_CAMERA_TYPE {
	SIMCONNECT_CAMERA_TYPE_OBJECT_PANEL,
    SIMCONNECT_CAMERA_TYPE_OBJECT_VIRTUAL,
    SIMCONNECT_CAMERA_TYPE_OBJECT_CENTER,
    SIMCONNECT_CAMERA_TYPE_OBJECT_PILOT,
    SIMCONNECT_CAMERA_TYPE_TOWER,
    SIMCONNECT_CAMERA_TYPE_FIXED,
    SIMCONNECT_CAMERA_TYPE_WORLD_OBJECT,
    SIMCONNECT_CAMERA_TYPE_LATLONALT_ORTHOGONAL,
    SIMCONNECT_CAMERA_TYPE_OBJECT_AI_VIRTUAL,
};

typedef DWORD SIMCONNECT_EVENT_SUBSCRIPTION_FLAG;

    static const DWORD SIMCONNECT_EVENT_SUBSCRIPTION_FLAG_DEFAULT        = 0x00000000;
    static const DWORD SIMCONNECT_EVENT_SUBSCRIPTION_FLAG_BLOCK          = 0x00000001;



typedef DWORD SIMCONNECT_VOR_FLAGS;            // flags for SIMCONNECT_RECV_ID_VOR_LIST 

    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_NAV_SIGNAL  = 0x00000001;   // Has Nav signal
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_LOCALIZER   = 0x00000002;   // Has localizer
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_GLIDE_SLOPE = 0x00000004;   // Has Nav signal
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_DME         = 0x00000008;   // Station has DME



// bits for the Waypoint Flags field: may be combined
typedef DWORD SIMCONNECT_WAYPOINT_FLAGS;

    static const DWORD SIMCONNECT_WAYPOINT_NONE                    = 0x00;
    static const DWORD SIMCONNECT_WAYPOINT_SPEED_REQUESTED         = 0x04;    // requested speed at waypoint is valid
    static const DWORD SIMCONNECT_WAYPOINT_THROTTLE_REQUESTED      = 0x08;    // request a specific throttle percentage
    static const DWORD SIMCONNECT_WAYPOINT_COMPUTE_VERTICAL_SPEED  = 0x10;    // compute vertical to speed to reach waypoint altitude when crossing the waypoint
    static const DWORD SIMCONNECT_WAYPOINT_ALTITUDE_IS_AGL         = 0x20;    // AltitudeIsAGL
    static const DWORD SIMCONNECT_WAYPOINT_ON_GROUND               = 0x00100000;   // place this waypoint on the ground
    static const DWORD SIMCONNECT_WAYPOINT_REVERSE                 = 0x00200000;   // Back up to this waypoint. Only valid on first waypoint
    static const DWORD SIMCONNECT_WAYPOINT_WRAP_TO_FIRST           = 0x00400000;   // Wrap around back to first waypoint. Only valid on last waypoint.


typedef DWORD SIMCONNECT_EVENT_FLAG;

    static const DWORD SIMCONNECT_EVENT_FLAG_DEFAULT                  = 0x00000000;
    static const DWORD SIMCONNECT_EVENT_FLAG_FAST_REPEAT_TIMER        = 0x00000001;      // set event repeat timer to simulate fast repeat
    static const DWORD SIMCONNECT_EVENT_FLAG_SLOW_REPEAT_TIMER        = 0x00000002;      // set event repeat timer to simulate slow repeat
    static const DWORD SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY      = 0x00000010;      // interpret GroupID parameter as priority value


typedef DWORD SIMCONNECT_DATA_REQUEST_FLAG;

    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_DEFAULT           = 0x00000000;
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_CHANGED           = 0x00000001;      // send requested data when value(s) change
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_TAGGED            = 0x00000002;      // send requested data in tagged format
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_BLOCK             = 0x00000004;      // Block server when data is sent


typedef DWORD SIMCONNECT_DATA_SET_FLAG;

    static const DWORD SIMCONNECT_DATA_SET_FLAG_DEFAULT               = 0x00000000;
    static const DWORD SIMCONNECT_DATA_SET_FLAG_TAGGED                = 0x00000001;      // data is in tagged format
    static const DWORD SIMCONNECT_DATA_SET_FLAG_UNBLOCK               = 0x00000002;      // Unblock server after data is set


typedef DWORD SIMCONNECT_CREATE_CLIENT_DATA_FLAG;

    static const DWORD SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT     = 0x00000000;
    static const DWORD SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY   = 0x00000001;      // permit only ClientData creator to write into ClientData


typedef DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG;

    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT    = 0x00000000;
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED    = 0x00000001;      // send requested ClientData when value(s) change
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED     = 0x00000002;      // send requested ClientData in tagged format
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_BLOCK      = 0x00000004;      // Block server when data is sent


typedef DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG;

    static const DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT        = 0x00000000;
    static const DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG_TAGGED         = 0x00000001;      // data is in tagged format
    static const DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG_UNBLOCK        = 0x00000002;


typedef DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA;                  // dwData contains these flags for the "View" System Event

    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_COCKPIT_2D      = 0x00000001;      // 2D Panels in cockpit view
    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_COCKPIT_VIRTUAL = 0x00000002;      // Virtual (3D) panels in cockpit view
    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_ORTHOGONAL      = 0x00000004;      // Orthogonal (Map) view


typedef DWORD SIMCONNECT_SOUND_SYSTEM_EVENT_DATA;            // dwData contains these flags for the "Sound" System Event

    static const DWORD SIMCONNECT_SOUND_SYSTEM_EVENT_DATA_MASTER    = 0x00000001;      // Sound Master


typedef DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG;         // specifies which synchronous callbacks the external sim wants sent to it

    static const DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG_CREATE            = 0x00000001;
    static const DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG_DESTROY           = 0x00000002;
    static const DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG_SIMULATE          = 0x00000004;
    static const DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG_LOCATION_CHANGED  = 0x00000008;
    static const DWORD SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG_EVENT             = 0x00000010;


//
// Flags used in SimConnect_RequestGroundInfo API call
//

// specify format for the LatLon components
enum SIMCONNECT_GROUND_INFO_LATLON_FORMAT
{
    SIMCONNECT_GROUND_INFO_LATLON_FORMAT_RADIANS,
    SIMCONNECT_GROUND_INFO_LATLON_FORMAT_DEGREES,
    SIMCONNECT_GROUND_INFO_LATLON_FORMAT_METERS,
};

// specify format for the Alt components
enum SIMCONNECT_GROUND_INFO_ALT_FORMAT
{
    SIMCONNECT_GROUND_INFO_ALT_FORMAT_METERS,
    SIMCONNECT_GROUND_INFO_ALT_FORMAT_FEET,
};

// specify data sources to search through
typedef DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG;

    static const DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG_TERRAIN      = 0x00010000;
    static const DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG_PLATFORMS    = 0x00020000;
    static const DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG_BATHYMETRY   = 0x00040000;
    static const DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG_IGNORE_WAVES = 0x00080000;
    static const DWORD SIMCONNECT_GROUND_INFO_SOURCE_FLAG_TERRAIN_AVG  = SIMCONNECT_GROUND_INFO_SOURCE_FLAG_IGNORE_WAVES;


enum SIMCONNECT_CAMERA_SENSOR_MODE
{
    SIMCONNECT_CAMERA_SENSOR_NONE,
    SIMCONNECT_CAMERA_SENSOR_IR,    //CAMERA_SENSOR_IR_WHITE_HOT
    SIMCONNECT_CAMERA_SENSOR_IR_BLACK_HOT, //CAMERA_SENSOR_IR_BLACK_HOT
    SIMCONNECT_CAMERA_SENSOR_GDATA  //CAMERA_SENSOR_GDATA
};

enum SIMCONNECT_LICENSE_TYPE
{
    SIMCONNECT_LICENSE_ACADEMIC,
    SIMCONNECT_LICENSE_PROFESSIONAL,
    SIMCONNECT_LICENSE_PROFESSIONAL_PLUS,
    SIMCONNECT_LICENSE_UNKNOWN // should always be last
};

enum SIMCONNECT_SCENERY_COMPLEXITY
{
    SIMCONNECT_SCENERY_COMPLEXITY_VERY_SPARSE,
    SIMCONNECT_SCENERY_COMPLEXITY_SPARSE,
    SIMCONNECT_SCENERY_COMPLEXITY_NORMAL,
    SIMCONNECT_SCENERY_COMPLEXITY_DENSE,
    SIMCONNECT_SCENERY_COMPLEXITY_VERY_DENSE,
    SIMCONNECT_SCENERY_COMPLEXITY_EXTREMELY_DENSE,
};

enum SIMCONNECT_DYNAMIC_FREQUENCY
{
    SIMCONNECT_DYNAMIC_FREQUENCY_VERY_SPARSE,
    SIMCONNECT_DYNAMIC_FREQUENCY_SPARSE,
    SIMCONNECT_DYNAMIC_FREQUENCY_NORMAL,
    SIMCONNECT_DYNAMIC_FREQUENCY_DENSE,
    SIMCONNECT_DYNAMIC_FREQUENCY_VERY_DENSE,
    SIMCONNECT_DYNAMIC_FREQUENCY_EXTREMELY_DENSE,
};

typedef DWORD SIMCONNECT_SHADOW_FLAGS;

    static const DWORD SIMCONNECT_SHADOW_FLAGS_INTERIOR_CAST       = 0x00000001;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_INTERIOR_RECEIVE    = 0x00000002;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_EXTERIOR_CAST       = 0x00000004;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_EXTERIOR_RECEIVE    = 0x00000008;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_SIMOBJECTS_CAST     = 0x00000010;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_SIMOBJECTS_RECEIVE  = 0x00000020;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_TERRAIN_CAST        = 0x00000040;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_TERRAIN_RECEIVE     = 0x00000080;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_VEGETATION_CAST     = 0x00000100;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_VEGETATION_RECEIVE  = 0x00000200;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_BUILDINGS_CAST      = 0x00000400;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_BUILDINGS_RECEIVE   = 0x00000800;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_CLOUDS_CAST         = 0x00001000;
    static const DWORD SIMCONNECT_SHADOW_FLAGS_PARTICLES_CAST      = 0x00002000;


enum SIMCONNECT_GOAL_STATE
{
    SIMCONNECT_GOAL_STATE_GOAL_PENDING,
    SIMCONNECT_GOAL_STATE_GOAL_COMPLETED,
    SIMCONNECT_GOAL_STATE_GOAL_FAILED,
};

enum SIMCONNECT_GOAL_RESOLUTION
{
    SIMCONNECT_GOAL_RESOLUTION_COMPLETED,
	SIMCONNECT_GOAL_RESOLUTION_FAILED,
};



enum SIMCONNECT_MISSION_OBJECTIVE_STATUS
{
    SIMCONNECT_MISSION_OBJECTIVE_STATUS_PENDING,
	SIMCONNECT_MISSION_OBJECTIVE_STATUS_PASSED,
	SIMCONNECT_MISSION_OBJECTIVE_STATUS_FAILED,
};

enum SIMCONNECT_MISSION_OBJECT_TYPE
{
    SIMCONNECT_MISSION_OBJECT_TYPE_GOAL,
	SIMCONNECT_MISSION_OBJECT_TYPE_MISSION_OBJECTIVE,
	SIMCONNECT_MISSION_OBJECT_TYPE_FLIGHT_SEGMENT,
    SIMCONNECT_MISSION_OBJECT_TYPE_LANDING_TRIGGER
};

enum SIMCONNECT_LANDING_TYPE
{
    SIMCONNECT_LANDING_TYPE_ANY,
    SIMCONNECT_LANDING_TYPE_FULL_STOP,
    SIMCONNECT_LANDING_TYPE_TOUCHDOWN,
};

//----------------------------------------------------------------------------
//        User-defined enums
//----------------------------------------------------------------------------

typedef DWORD SIMCONNECT_NOTIFICATION_GROUP_ID;     //client-defined notification group ID
typedef DWORD SIMCONNECT_INPUT_GROUP_ID;            //client-defined input group ID
typedef DWORD SIMCONNECT_DATA_DEFINITION_ID;        //client-defined data definition ID
typedef DWORD SIMCONNECT_DATA_REQUEST_ID;           //client-defined request data ID
 
typedef DWORD SIMCONNECT_CLIENT_EVENT_ID;           //client-defined client event ID
typedef DWORD SIMCONNECT_CLIENT_DATA_ID;            //client-defined client data ID
typedef DWORD SIMCONNECT_CLIENT_DATA_DEFINITION_ID; //client-defined client data definition ID

typedef DWORD SIMCONNECT_CAMERA_COMMAND;            //camera and observer commands (mimic user input)


//----------------------------------------------------------------------------
//        Struct definitions
//----------------------------------------------------------------------------

#pragma pack(push, 1)

// SIMCONNECT_DATA_LATLONALT
struct SIMCONNECT_DATA_LATLONALT
{
    double  Latitude;
    double  Longitude;
    double  Altitude;
};

// SIMCONNECT_DATA_XYZ
struct SIMCONNECT_DATA_XYZ
{
    double  x;
    double  y;
    double  z;
};

// SIMCONNECT_DATA_PBH
struct SIMCONNECT_DATA_PBH
{
    double  Pitch;
    double  Bank;
    double  Heading;
};

struct SIMCONNECT_RECV
{
    DWORD   dwSize;         // record size
    DWORD   dwVersion;      // interface version
    DWORD   dwID;           // see SIMCONNECT_RECV_ID
};

struct SIMCONNECT_RECV_EXCEPTION : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_EXCEPTION
{
    DWORD   dwException;    // see SIMCONNECT_EXCEPTION
    static const DWORD UNKNOWN_SENDID = 0;
    DWORD   dwSendID;       // see SimConnect_GetLastSentPacketID
    static const DWORD UNKNOWN_INDEX = DWORD_MAX;
    DWORD   dwIndex;        // index of parameter that was source of error
};

struct SIMCONNECT_RECV_OPEN : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_OPEN
{
    char    szApplicationName[256];
    DWORD   dwApplicationVersionMajor;
    DWORD   dwApplicationVersionMinor;
    DWORD   dwApplicationBuildMajor;
    DWORD   dwApplicationBuildMinor;
    DWORD   dwSimConnectVersionMajor;
    DWORD   dwSimConnectVersionMinor;
    DWORD   dwSimConnectBuildMajor;
    DWORD   dwSimConnectBuildMinor;
    DWORD   dwReserved1;
    DWORD   dwReserved2;
};

struct SIMCONNECT_RECV_QUIT : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_QUIT
{
};

struct SIMCONNECT_RECV_EVENT_BASE : public SIMCONNECT_RECV       // when dwID == SIMCONNECT_RECV_ID_EVENT
{
    static const DWORD UNKNOWN_GROUP = DWORD_MAX;
    DWORD   uGroupID;
    DWORD   uEventID; 
    DWORD   dwData;       // uEventID-dependent context
};

struct SIMCONNECT_RECV_EVENT : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT
{
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_64 : public SIMCONNECT_RECV_EVENT    // when dwID == SIMCONNECT_RECV_ID_EVENT_64
{
    QWORD   qwData;       // uEventID-dependent context
};

struct SIMCONNECT_RECV_EVENT_FILENAME : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT_FILENAME
{
    char szFileName[MAX_PATH];
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT_FILENAME
{
    SIMCONNECT_SIMOBJECT_TYPE   eObjType;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_FRAME : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT_FRAME
{
    float   fFrameRate;
    float   fSimSpeed;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_MULTIPLAYER_SERVER_STARTED : public SIMCONNECT_RECV_EVENT    // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SERVER_STARTED
{
    // No event specific data, for now
};

struct SIMCONNECT_RECV_EVENT_MULTIPLAYER_CLIENT_STARTED : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_CLIENT_STARTED
{
    // No event specific data, for now
};

struct SIMCONNECT_RECV_EVENT_MULTIPLAYER_SESSION_ENDED : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SESSION_ENDED
{
    // No event specific data, for now
};

// SIMCONNECT_DATA_RACE_RESULT
struct SIMCONNECT_DATA_RACE_RESULT
{
    DWORD   dwNumberOfRacers;                         // The total number of racers
    GUID MissionGUID;                      // The name of the mission to execute, NULL if no mission
    char szPlayerName[MAX_PATH];       // The name of the player
    char szSessionType[MAX_PATH];      // The type of the multiplayer session: "LAN", "GAMESPY")
    char szAircraft[MAX_PATH];         // The aircraft type 
    char szPlayerRole[MAX_PATH];       // The player role in the mission
    double   fTotalTime;                              // Total time in seconds, 0 means DNF
    double   fPenaltyTime;                            // Total penalty time in seconds
    DWORD   dwIsDisqualified;                         // non 0 - disqualified, 0 - not disqualified
};

struct SIMCONNECT_RECV_EVENT_RACE_END : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT_RACE_END
{
    DWORD   dwRacerNumber;                            // The index of the racer the results are for
    SIMCONNECT_DATA_RACE_RESULT RacerData;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_RACE_LAP : public SIMCONNECT_RECV_EVENT_BASE       // when dwID == SIMCONNECT_RECV_ID_EVENT_RACE_LAP
{
    DWORD   dwLapIndex;                               // The index of the lap the results are for
    SIMCONNECT_DATA_RACE_RESULT RacerData;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_WEAPON : public SIMCONNECT_RECV_EVENT_BASE // when dwID == SIMCONNECT_RECV_ID_EVENT_WEAPON_FIRED or SIMCONNECT_RECV_ID_EVENT_WEAPON_DETONATED
{
    DWORD   dwRequestID;
	char szWeaponTitle[MAX_PATH];
    char szWeaponType[MAX_PATH];
    DWORD   dwObjectID;
	DWORD   dwAttackerID;
	DWORD   dwTargetID;
	SIMCONNECT_DATA_LATLONALT llaPosition;
	DWORD	dwResult;
	DWORD   dwFlags;
};

// SIMCONNECT_DATA_OBJECT_DAMAGED_BY_WEAPON
struct SIMCONNECT_DATA_OBJECT_DAMAGED_BY_WEAPON
{
    DWORD   dwWeaponID;          //Weapon ID
    DWORD   dwAttackerID;        //Attacker ID
    DWORD   dwDamagedObjectID;   //Target ID
};

struct SIMCONNECT_RECV_EVENT_OBJECT_DAMAGED_BY_WEAPON : public SIMCONNECT_RECV_EVENT_BASE // when dwID == SIMCONNECT_RECV_ID_EVENT_OBJECT_DAMAGED_BY_WEAPON
{
    DWORD   dwRequestID;
    SIMCONNECT_DATA_OBJECT_DAMAGED_BY_WEAPON DamageData;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_EVENT_COUNTERMEASURE : public SIMCONNECT_RECV_EVENT_BASE // when dwID == SIMCONNECT_RECV_ID_EVENT_COUNTERMEASURE_FIRED
{
    DWORD   dwRequestID;
    char szCountermeasureName[MAX_PATH];
    DWORD   dwObjectID;
    DWORD   dwAttackerID;
    SIMCONNECT_DATA_LATLONALT llaPosition;
    DWORD   dwFlags;
};

struct SIMCONNECT_RECV_ATTACHPOINT_DATA : public SIMCONNECT_RECV          // when dwID == SIMCONNECT_RECV_ID_ATTACHPOINT_DATA
{
    DWORD   dwRequestID;
    DWORD   dwObjectID;
    SIMCONNECT_DATA_XYZ         xyzLocalOffset;     // local offset in feet
    SIMCONNECT_DATA_PBH         pbhLocalRotation;   // local rotation in radians
    SIMCONNECT_DATA_LATLONALT   llaWorldPosition;   // latitude and longitude in radians, altitude in feet
    SIMCONNECT_DATA_PBH         pbhWorldRotation;   // world rotation in radians
};

struct SIMCONNECT_RECV_SIMOBJECT_DATA : public SIMCONNECT_RECV            // when dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA
{
    DWORD   dwRequestID;
    DWORD   dwObjectID;
    DWORD   dwDefineID;
    DWORD   dwFlags;            // SIMCONNECT_DATA_REQUEST_FLAG
    DWORD   dwentrynumber;      // if multiple objects returned, this is number <entrynumber> out of <outof>.
    DWORD   dwoutof;            // note: starts with 1, not 0.          
    DWORD   dwDefineCount;      // data count (number of datums, *not* byte count)
    DWORD   dwData;             // data begins here, dwDefineCount data items
};

struct SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE : public SIMCONNECT_RECV_SIMOBJECT_DATA           // when dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
{
};

struct SIMCONNECT_RECV_MOBILE_SCENERY_DATA : public SIMCONNECT_RECV            // when dwID == SIMCONNECT_RECV_ID_MOBILE_SCENERY_DATA
{
    DWORD dwRequestID;
    GUID             guidInstanceID;     // Instance id of the mobile scenery
    SIMCONNECT_DATA_LATLONALT   llaWorldPosition;   // latitude and longitude in degrees, altitude in feet
    SIMCONNECT_DATA_PBH         pbhWorldRotation;   // world rotation in radians
    BOOL                        isOnGround;         // Whether mobile scenery is clamped to ground
    double                      speedKnots;         //Speed of mobile scenery in knots
    float                       scale;              //Scale of mobile scenery object
    DWORD                       dwObjectID;         // Mobile Scenery Object ID
};

struct SIMCONNECT_DATA_MOBILE_SCENERY_INFO
{
    char szMobileSceneryName[MAX_PATH]; // Mobile Scenery name
    DWORD   dwObjectID;                               // Mobile Scenery Object ID
};

struct SIMCONNECT_RECV_MOBILE_SCENERY_IN_RADIUS : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_MOBILE_SCENERY_IN_RADIUS
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    // array of SIMCONNECT_DATA_MOBILE_SCENERY_INFO structures
    SIMCONNECT_DATA_MOBILE_SCENERY_INFO rgData[1];
};

struct SIMCONNECT_RECV_CLIENT_DATA : public SIMCONNECT_RECV_SIMOBJECT_DATA    // when dwID == SIMCONNECT_RECV_ID_CLIENT_DATA
{
};

struct SIMCONNECT_RECV_SYNCHRONOUS_BLOCK : public SIMCONNECT_RECV           // when dwID == SIMCONNECT_RECV_ID_SYNCHRONOUS_BLOCK
{
    DWORD   dwRequestID;
    DWORD   dwFlags;            // SIMCONNECT_DATA_REQUEST_FLAG
};

struct SIMCONNECT_RECV_CAMERA_6DOF : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CAMERA_6DOF
{
    DWORD   dwRequestID;
    float   fDeltaXMeters;
    float   fDeltaYMeters;
    float   fDeltaZMeters;
    float   fPitchDeg;
    float   fBankDeg;
    float   fHeadingDeg;
};

struct SIMCONNECT_RECV_CAMERA_FOV : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CAMERA_FOV
{
    DWORD   dwRequestID;
    float   fHorizontalFov;
    float   fVerticalFov;
};

struct SIMCONNECT_RECV_CAMERA_SENSOR_MODE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CAMERA_SENSOR_MODE
{
    DWORD   dwRequestID;
    SIMCONNECT_CAMERA_SENSOR_MODE eSensorMode;
};

struct SIMCONNECT_RECV_CAMERA_WINDOW_POSITION : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CAMERA_WINDOW_POSITION
{
    DWORD   dwRequestID;
    DWORD   dwXPosition;
    DWORD   dwYPosition;
};

struct SIMCONNECT_RECV_CAMERA_WINDOW_SIZE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CAMERA_WINDOW_SIZE
{
    DWORD   dwRequestID;
    DWORD   dwWidth;
    DWORD   dwHeight;
};

// SIMCONNECT_DATA_JOYSTICK_DEVICE_INFO
struct SIMCONNECT_DATA_JOYSTICK_DEVICE_INFO
{
    char szName[128];     // The joystick name
    DWORD dwNumber;                     // The joystick number
};

struct SIMCONNECT_RECV_JOYSTICK_DEVICE_INFO : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_JOYSTICK_DEVICE_INFO
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    // array of SIMCONNECT_DATA_JOYSTICK_DEVICE_INFO structures
    SIMCONNECT_DATA_JOYSTICK_DEVICE_INFO rgData[1];
};

struct SIMCONNECT_RECV_WEATHER_OBSERVATION : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_WEATHER_OBSERVATION
{
    DWORD   dwRequestID;
    char szMetar[1];      // Variable length string whose maximum size is MAX_METAR_LENGTH
};

static const int SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH = 64;
static const int SIMCONNECT_CLOUD_STATE_ARRAY_SIZE = SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH*SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH;

struct SIMCONNECT_RECV_CLOUD_STATE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CLOUD_STATE
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    BYTE    rgbData[1];
};

struct SIMCONNECT_RECV_ASSIGNED_OBJECT_ID : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
{
    DWORD   dwRequestID;
    DWORD   dwObjectID;
};

struct SIMCONNECT_RECV_RESERVED_KEY : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_RESERVED_KEY
{
    char    szChoiceReserved[30];
    char    szReservedKey[50];
};

struct SIMCONNECT_RECV_SYSTEM_STATE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_SYSTEM_STATE
{
    DWORD   dwRequestID;
    DWORD   dwInteger;
    float   fFloat;
    char    szString[MAX_PATH];
};

struct SIMCONNECT_RECV_VERSION : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_VERSION
{
	DWORD   dwRequestID;
    DWORD   dwApplicationVersionType;
    DWORD   dwApplicationVersionMajor;
    DWORD   dwApplicationVersionMinor;
    DWORD   dwApplicationBuildMajor;
    DWORD   dwApplicationBuildMinor;
    DWORD   dwSimConnectVersionMajor;
    DWORD   dwSimConnectVersionMinor;
    DWORD   dwSimConnectBuildMajor;
    DWORD   dwSimConnectBuildMinor;
};

struct SIMCONNECT_RECV_SCENERY_COMPLEXITY : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_SCENERY_COMPLEXITY
{
	DWORD   dwRequestID;
    DWORD   dwSceneryComplexity;
};

struct SIMCONNECT_RECV_SHADOW_FLAGS : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_SHADOW_FLAGS
{
	DWORD   dwRequestID;
    DWORD   dwShadowFlags;
};

struct SIMCONNECT_RECV_CUSTOM_ACTION : public SIMCONNECT_RECV_EVENT_BASE
{
    GUID guidInstanceId;      // Instance id of the action that executed
    DWORD dwWaitForCompletion;           // Wait for completion flag on the action
    char szPayLoad[1];      // Variable length string payload associated with the mission action.  
};

struct SIMCONNECT_RECV_SESSION_DURATION : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_SESSION_DURATION
{
    DWORD   dwRequestID;
    double  dSessionDuration;
};

struct SIMCONNECT_RECV_GOAL : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_GOAL
{
    DWORD dwRequestID;
    GUID guidInstanceID;          // Instance id of the goal
    BOOL isOptional;                         // Whether goal is optional for mission completion
    DWORD dwOrder;                           // Priority order of goal
    DWORD dwPointValue;                      // Point value of goal
    SIMCONNECT_GOAL_STATE eGoalState;        // Goal state
    DWORD dwChildGoalCount;                  // Number of child goals
    char szGoalText[MAX_PATH]; // Goal text
    char szGoalSucceededText[MAX_PATH]; // Goal's succeeded text
    char szGoalFailedText[MAX_PATH]; // Goal's failed text
};

struct SIMCONNECT_RECV_GOAL_PAIR : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_GOAL_PAIR
{
    DWORD dwRequestID;
    GUID guidChildInstanceID;    //Instance id of the child goal
    GUID guidParentInstanceID;   //Instance id of the parent mission objective or goal
};

struct SIMCONNECT_RECV_MISSION_OBJECTIVE : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_MISSION_OBJECTIVE
{
    DWORD dwRequestID;
    GUID guidInstanceID;                               // Instance id of the mission objective
	BOOL isOptional;                                              // Whether mission objective is optional for mission completion
	BOOL isObjectivePassed;                                       // Whether Mission Objective has been passed
	SIMCONNECT_MISSION_OBJECTIVE_STATUS eMissionObjectiveStatus;  // Mission objective status
	DWORD dwPassValue;                                            // Value needed to pass mission objective
	DWORD dwCurrentScore;                                         // Current score of mission objective
	DWORD dwTotalPossiblePoints;                                  // Maximum total score possible for mission objective
	DWORD dwPointValue;                                           // Points mission objective is worth when passed
	DWORD dwOrder;                                                // Priority order of mission objective
    DWORD dwChildGoalCount;                                       // Number of child goals
	char szMissionObjectiveText[MAX_PATH];          // Mission Objective text	
};

struct SIMCONNECT_RECV_MISSION_OBJECT_COUNT : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_MISSION_OBJECT_COUNT
{
    DWORD dwRequestID;
	SIMCONNECT_MISSION_OBJECT_TYPE eMissionObjectType; // Type of mission object the count is for TODO REMOVE: Whether count is goal count (otherwise is mission objective count)
	DWORD dwCount;                                     // Number of mission objects of specified type in mission
};

struct SIMCONNECT_RECV_FLIGHT_SEGMENT : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_FLIGHT_SEGMENT
{
    DWORD dwRequestID;
	GUID guidInstanceID;      // Instance id of the flight segment
	GUID guidSegmentGoalID;   // Instance id of the flight segment's goal
	DWORD dwParameterCount;              // Number of parameters that are being graded in this flight segment
    DWORD dwTotalRangeCount;             // Total number of ranges combined for all parameters that are graded with this flight segment
};

struct SIMCONNECT_RECV_PARAMETER_RANGE : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_PARAMETER_RANGE
{
    DWORD dwRequestID;
    DWORD dwExceededCount;    // Number of times this range was exceeded in either direction, checking every second
    DWORD dwMaxOverMeasured;  // The upper bound of this range, in terms of value above the measured value
    DWORD dwMinUnderMeasured; // The lower bound of this range, in terms of value under the measured value
    GUID guidFlightSegmentID; //The instance ID of the Flight Segment this range belongs to
    DWORD dwColorRedComponent; //The RGB red color value of this range
    DWORD dwColorGreenComponent; //The RGB green color value of this range
    DWORD dwColorBlueComponent; //The RGB blue color value of this range
    char szRangeName[MAX_PATH]; // Range Name
    char szParameterName[MAX_PATH]; // Parameter Name
    char szParameterUnitsString[MAX_PATH]; //Units of parameter as string
};

struct SIMCONNECT_RECV_RECORDING_INFO : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_RECORDING_INFO
{
    DWORD dwRequestID;
    double startTime;
    double endTime;
    DWORD bookmarkCount;
    char szTitle[MAX_PATH];
    char szDescription[MAX_PATH];
    char szUserContainerTitle[MAX_PATH];
    char szFilename[MAX_PATH];
};

struct SIMCONNECT_RECV_RECORDING_BOOKMARK_INFO : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_RECORDING_BOOKMARK_INFO
{
    DWORD dwRequestID;
    double timeStamp;
    DWORD bookmarkIndex;
    char szTitle[MAX_PATH];
    char szRecordingFilename[MAX_PATH];
};

struct SIMCONNECT_RECV_FLIGHT_SEGMENT_READY_FOR_GRADING : public SIMCONNECT_RECV_EVENT_BASE
{
    GUID guidInstanceID;      // Instance id of the flight segment that is ready for grading
};

struct SIMCONNECT_RECV_PLAYBACK_STATE_CHANGED : public SIMCONNECT_RECV_EVENT
{
    BOOL hasPlaybackStarted; // Playback started if true, playback ended if false
    char szRecordingFileName[MAX_PATH]; // Recording file name
};

struct SIMCONNECT_RECV_RECORDER_STATE_CHANGED : public SIMCONNECT_RECV_EVENT
{
    BOOL hasRecordingStarted; // Recording started if true, recording ended if false
};

struct SIMCONNECT_RECV_EVENT_FLIGHT_ANALYSIS_DIAGRAMS : public SIMCONNECT_RECV_EVENT // when dwID == SIMCONNECT_RECV_ID_EVENT_FLIGHT_ANALYSIS_DIAGRAMS
{
    bool hasGlideSlope;      // Whether or not the flight analysis diagrams had a glide slope
};


struct SIMCONNECT_RECV_LANDING_TRIGGER_INFO : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_LANDING_TRIGGER_INFO
{
    DWORD           dwRequestID;
    GUID landingTriggerInstanceID;
    int             landingsCount;
};

struct SIMCONNECT_RECV_LANDING_INFO : public SIMCONNECT_RECV //when dwID == SIMCONNECT_RECV_ID_LANDING_INFO
{
    DWORD                   dwRequestID;
    GUID         LandingTriggerInstanceID;
    SIMCONNECT_LANDING_TYPE LandingType;
    double                  Latitude;
    double                  Longitude;
    double                  Altitude;
};


struct SIMCONNECT_RECV_EVENT_WEATHER_MODE : public SIMCONNECT_RECV_EVENT
{
    // No event specific data - the new weather mode is in the base structure dwData member.
};

// SIMCONNECT_RECV_FACILITIES_LIST
struct SIMCONNECT_RECV_FACILITIES_LIST : public SIMCONNECT_RECV
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    DWORD   dwEntryNumber;  // when the array of items is too big for one send, which send this is (0..dwOutOf-1)
    DWORD   dwOutOf;        // total number of transmissions the list is chopped into
};

// SIMCONNECT_DATA_FACILITY_AIRPORT
struct SIMCONNECT_DATA_FACILITY_AIRPORT
{
    char Icao[9];     // ICAO of the object
    double  Latitude;               // degrees
    double  Longitude;              // degrees
    double  Altitude;               // meters   
};

// SIMCONNECT_RECV_AIRPORT_LIST
struct SIMCONNECT_RECV_AIRPORT_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_DATA_FACILITY_AIRPORT rgData[1];
};


// SIMCONNECT_DATA_FACILITY_WAYPOINT
struct SIMCONNECT_DATA_FACILITY_WAYPOINT : public SIMCONNECT_DATA_FACILITY_AIRPORT
{
    float   fMagVar;                // Magvar in degrees
};

// SIMCONNECT_RECV_WAYPOINT_LIST
struct SIMCONNECT_RECV_WAYPOINT_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_DATA_FACILITY_WAYPOINT rgData[1];
};

// SIMCONNECT_DATA_FACILITY_NDB
struct SIMCONNECT_DATA_FACILITY_NDB : public SIMCONNECT_DATA_FACILITY_WAYPOINT
{
    DWORD   fFrequency;             // frequency in Hz
};

// SIMCONNECT_RECV_NDB_LIST
struct SIMCONNECT_RECV_NDB_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_DATA_FACILITY_NDB rgData[1];
};

// SIMCONNECT_DATA_FACILITY_TACAN
struct SIMCONNECT_DATA_FACILITY_TACAN : public SIMCONNECT_DATA_FACILITY_WAYPOINT
{
    DWORD   uChannel;             // frequency in Hz
    BOOL    bXYBandIsY;           //X or Y band
};

// SIMCONNECT_RECV_TACAN_LIST
struct SIMCONNECT_RECV_TACAN_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_DATA_FACILITY_TACAN rgData[1];
};


// SIMCONNECT_DATA_FACILITY_VOR
struct SIMCONNECT_DATA_FACILITY_VOR : public SIMCONNECT_DATA_FACILITY_NDB
{
    DWORD   Flags;                  // SIMCONNECT_VOR_FLAGS
    float   fLocalizer;             // Localizer in degrees
    double  GlideLat;               // Glide Slope Location (deg, deg, meters)
    double  GlideLon;
    double  GlideAlt;
    float   fGlideSlopeAngle;       // Glide Slope in degrees
};

// SIMCONNECT_RECV_VOR_LIST
struct SIMCONNECT_RECV_VOR_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_DATA_FACILITY_VOR rgData[1];
};


// SIMCONNECT_DATATYPE_INITPOSITION
struct SIMCONNECT_DATA_INITPOSITION
{
    double  Latitude;   // degrees
    double  Longitude;  // degrees
    double  Altitude;   // feet   
    double  Pitch;      // degrees
    double  Bank;       // degrees
    double  Heading;    // degrees
    DWORD   OnGround;   // 1=force to be on the ground
    DWORD   Airspeed;   // knots
};


// SIMCONNECT_DATATYPE_MARKERSTATE
struct SIMCONNECT_DATA_MARKERSTATE
{
    char    szMarkerName[64];
    DWORD   dwMarkerState;
};

// SIMCONNECT_DATATYPE_WAYPOINT
struct SIMCONNECT_DATA_WAYPOINT
{
    double          Latitude;   // degrees
    double          Longitude;  // degrees
    double          Altitude;   // feet   
    unsigned long   Flags;
    double          ktsSpeed;   // knots
    double          percentThrottle;
};

// SIMCONNECT_DATA_OBSERVER
struct SIMCONNECT_DATA_OBSERVER
{
    SIMCONNECT_DATA_LATLONALT   Position;
    SIMCONNECT_DATA_PBH         Rotation;
    DWORD                       Regime;
    BOOL                        RotateOnTarget;
    BOOL                        FocusFixed;
    float                       FocalLength;
    float                       FieldOfViewH;
    float                       FieldOfViewV;
    float                       LinearStep;
    float                       AngularStep;
};

// SIMCONNECT_RECV_OBSERVER_DATA
struct SIMCONNECT_RECV_OBSERVER_DATA : public SIMCONNECT_RECV
{
	DWORD						dwRequestID;
	char    szObserverName[128];
	SIMCONNECT_DATA_OBSERVER      ObserverData;
};

// Structure returned as array in SIMCONNECT_RECV_GROUND_INFO response to SimConnect_RequestGroundInfo API call
struct SIMCONNECT_DATA_GROUND_INFO
{
    BOOL bIsValid;              // true = this data item is valid 
    double fLat;                // Latitude of this point (units based on SIMCONNECT_RECV_GROUND_INFO.eFlags)
    double fLon;                // Longitude of this point (units based on SIMCONNECT_RECV_GROUND_INFO.eFlags)
    double fAlt;                // Altitude of this point (units based on SIMCONNECT_RECV_GROUND_INFO.eFlags)
    double fNormalI;            // I Component of normal at point
    double fNormalJ;            // J Component of normal at point
    double fNormalK;            // K Component of normal at point
    DWORD eSurfaceType;         // Surface Type value
    DWORD eSurfaceCondition;    // Surface Condition value
    BOOL bIsPlatform;           // true = Point represents a platform
    BOOL bIsPlatformMoving;     // true = Point represents a moving plaform
};


// Response structure for SimConnect_RequestGroundInfo API Call
struct SIMCONNECT_RECV_GROUND_INFO : public SIMCONNECT_RECV
{
    DWORD   dwRequestID;    // Request ID for this reply
    DWORD   dwGridWidth;    // Grid Width asked for
    DWORD   dwGridHeight;   // Grid Hieght asked for
    DWORD   dwArraySize;    // Number of points in rgData (Grid Width * Grid Height)
    DWORD   dwFlags;
    BOOL    bNotAllValid;   // true if any point in rgData returned no data
    // array of SIMCONNECT_DATA_GROUND_INFO structures
    SIMCONNECT_DATA_GROUND_INFO rgData[1];
};

// base data for external sim callbacks
struct SIMCONNECT_RECV_EXTERNAL_SIM_BASE : public SIMCONNECT_RECV
{
    GUID guidExternalSimID;
    DWORD dwObjectID;
};

struct SIMCONNECT_RECV_EXTERNAL_SIM_CREATE : public SIMCONNECT_RECV_EXTERNAL_SIM_BASE
{
	DWORD dwExternalSimVarCount;				// number of simvars for this external sim
	DWORD dwExternalSimVarBase;					// first index of simvars for this external sim
    char szExternalSimData[1];		// Variable length string payload containing data to pass along to the external sim
};

struct SIMCONNECT_RECV_EXTERNAL_SIM_DESTROY : public SIMCONNECT_RECV_EXTERNAL_SIM_BASE
{
};

struct SIMCONNECT_RECV_EXTERNAL_SIM_SIMULATE : public SIMCONNECT_RECV_EXTERNAL_SIM_BASE
{
    double fDeltaTime;
    BOOL bShouldSimulate;
    DWORD dwDefineID;
    DWORD dwDefineCount;      // data count (number of datums, *not* byte count)
    DWORD dwData;             // data begins here, dwDefineCount data items
};

struct SIMCONNECT_RECV_EXTERNAL_SIM_LOCATION_CHANGED : public SIMCONNECT_RECV_EXTERNAL_SIM_BASE
{
    double fLatRadians;
    double fLonRadians; 
    double fAltMeters; 
    BOOL bPlaceOnGround;
    BOOL bZeroSpeed;
};

struct SIMCONNECT_RECV_EXTERNAL_SIM_EVENT : public SIMCONNECT_RECV_EXTERNAL_SIM_BASE
{
    DWORD uEventID;
    DWORD dwData;
};

// SIMCONNECT_RECV_TRAFFIC_SETTINGS
struct SIMCONNECT_RECV_TRAFFIC_SETTINGS : public SIMCONNECT_RECV
{
    DWORD dwRequestID;
    UINT uAirlineDensity;
    UINT uGADensity;
    UINT uRoadTrafficDensity;
    UINT uShipsAndFerriesDensity;
    UINT uLeisureBoatDensity;
    SIMCONNECT_DYNAMIC_FREQUENCY eAirportVehicleDensity;
    BOOL bIFROnly;
};

// SIMCONNECT_DATA_VIDEO_STREAM_INFO
struct SIMCONNECT_DATA_VIDEO_STREAM_INFO
{
    char szSourceAddress[48];
    char szDestinationAddress[48];
    UINT  uPort;
    UINT  uWidth;
    UINT  uHeight;
    UINT  uFramerate;
    UINT  uBitRate;
    UINT  uFormat;
};

#pragma pack(pop)

//----------------------------------------------------------------------------
//        End of Struct definitions
//----------------------------------------------------------------------------


#if !defined(SIMCONNECTAPI)
#define SIMCONNECTAPI extern "C" HRESULT __stdcall
#endif


typedef void (CALLBACK *DispatchProc)(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);


SIMCONNECTAPI SimConnect_RetrieveString(SIMCONNECT_RECV * pData, DWORD cbData, void * pStringV, char ** pszString, DWORD * pcbString);
SIMCONNECTAPI SimConnect_GetLastSentPacketID(HANDLE hSimConnect, DWORD * pdwError);
SIMCONNECTAPI SimConnect_CallDispatch(HANDLE hSimConnect, DispatchProc pfcnDispatch, void * pContext);
SIMCONNECTAPI SimConnect_GetNextDispatch(HANDLE hSimConnect, SIMCONNECT_RECV ** ppData, DWORD * pcbData);
SIMCONNECTAPI SimConnect_RequestResponseTimes(HANDLE hSimConnect, DWORD nCount, float * fElapsedSeconds);
SIMCONNECTAPI SimConnect_InsertString(BYTE * pDest, DWORD cbDest, BYTE ** ppEnd, DWORD * pcbStringV, const char * pSource);
SIMCONNECTAPI SimConnect_Open(HANDLE * phSimConnect, LPCSTR szName, HWND hWnd, DWORD UserEventWin32, HANDLE hEventHandle, DWORD ConfigIndex);
SIMCONNECTAPI SimConnect_Close(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_MapClientEventToSimEvent(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char * EventName = "");
SIMCONNECTAPI SimConnect_TransmitClientEvent(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_CLIENT_EVENT_ID EventID, DWORD dwData, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_EVENT_FLAG Flags);
SIMCONNECTAPI SimConnect_SetSystemEventState(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, SIMCONNECT_STATE dwState);
SIMCONNECTAPI SimConnect_AddClientEventToNotificationGroup(HANDLE hSimConnect, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_CLIENT_EVENT_ID EventID, BOOL bMaskable = FALSE);
SIMCONNECTAPI SimConnect_RemoveClientEvent(HANDLE hSimConnect, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_CLIENT_EVENT_ID EventID);
SIMCONNECTAPI SimConnect_SetNotificationGroupPriority(HANDLE hSimConnect, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, DWORD uPriority);
SIMCONNECTAPI SimConnect_ClearNotificationGroup(HANDLE hSimConnect, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID);
SIMCONNECTAPI SimConnect_RequestNotificationGroup(HANDLE hSimConnect, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, DWORD dwReserved = 0, DWORD Flags = 0);
SIMCONNECTAPI SimConnect_AddToDataDefinition(HANDLE hSimConnect, SIMCONNECT_DATA_DEFINITION_ID DefineID, const char * DatumName, const char * UnitsName, SIMCONNECT_DATATYPE DatumType = SIMCONNECT_DATATYPE_FLOAT64, float fEpsilon = 0, DWORD DatumID = SIMCONNECT_UNUSED);
SIMCONNECTAPI SimConnect_ClearDataDefinition(HANDLE hSimConnect, SIMCONNECT_DATA_DEFINITION_ID DefineID);
SIMCONNECTAPI SimConnect_RequestDataOnSimObject(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_DATA_DEFINITION_ID DefineID, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_PERIOD Period, SIMCONNECT_DATA_REQUEST_FLAG Flags = 0, DWORD origin = 0, DWORD interval = 0, DWORD limit = 0);
SIMCONNECTAPI SimConnect_RequestDataOnSimObjectType(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_DATA_DEFINITION_ID DefineID, DWORD dwRadiusMeters, SIMCONNECT_SIMOBJECT_TYPE type);
SIMCONNECTAPI SimConnect_SetDataOnSimObject(HANDLE hSimConnect, SIMCONNECT_DATA_DEFINITION_ID DefineID, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_SET_FLAG Flags, DWORD ArrayCount, DWORD cbUnitSize, void * pDataSet);
SIMCONNECTAPI SimConnect_MapInputEventToClientEvent(HANDLE hSimConnect, SIMCONNECT_INPUT_GROUP_ID GroupID, const char * szInputDefinition, SIMCONNECT_CLIENT_EVENT_ID DownEventID, DWORD DownValue = 0, SIMCONNECT_CLIENT_EVENT_ID UpEventID = (SIMCONNECT_CLIENT_EVENT_ID)SIMCONNECT_UNUSED, DWORD UpValue = 0, BOOL bMaskable = FALSE);
SIMCONNECTAPI SimConnect_SetInputGroupPriority(HANDLE hSimConnect, SIMCONNECT_INPUT_GROUP_ID GroupID, DWORD uPriority);
SIMCONNECTAPI SimConnect_RemoveInputEvent(HANDLE hSimConnect, SIMCONNECT_INPUT_GROUP_ID GroupID, const char * szInputDefinition);
SIMCONNECTAPI SimConnect_ClearInputGroup(HANDLE hSimConnect, SIMCONNECT_INPUT_GROUP_ID GroupID);
SIMCONNECTAPI SimConnect_SetInputGroupState(HANDLE hSimConnect, SIMCONNECT_INPUT_GROUP_ID GroupID, DWORD dwState);
SIMCONNECTAPI SimConnect_RequestReservedKey(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char * szKeyChoice1 = "", const char * szKeyChoice2 = "", const char * szKeyChoice3 = "");
SIMCONNECTAPI SimConnect_SubscribeToSystemEvent(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char * SystemEventName);
SIMCONNECTAPI SimConnect_UnsubscribeFromSystemEvent(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID);
SIMCONNECTAPI SimConnect_WeatherRequestInterpolatedObservation(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, float lat, float lon, float alt);
SIMCONNECTAPI SimConnect_WeatherRequestObservationAtStation(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szICAO);
SIMCONNECTAPI SimConnect_WeatherRequestObservationAtNearestStation(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, float lat, float lon);
SIMCONNECTAPI SimConnect_WeatherCreateStation(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szICAO, const char * szName, float lat, float lon, float alt);
SIMCONNECTAPI SimConnect_WeatherRemoveStation(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szICAO);
SIMCONNECTAPI SimConnect_WeatherSetObservation(HANDLE hSimConnect, DWORD Seconds, const char * szMETAR);
SIMCONNECTAPI SimConnect_WeatherSetModeServer(HANDLE hSimConnect, DWORD dwPort, DWORD dwSeconds);
SIMCONNECTAPI SimConnect_WeatherSetModeTheme(HANDLE hSimConnect, const char * szThemeName);
SIMCONNECTAPI SimConnect_WeatherSetModeGlobal(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_WeatherSetModeCustom(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_WeatherSetDynamicUpdateRate(HANDLE hSimConnect, DWORD dwRate);
SIMCONNECTAPI SimConnect_WeatherRequestCloudState(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, float minLat, float minLon, float minAlt, float maxLat, float maxLon, float maxAlt, DWORD dwFlags = 0);
SIMCONNECTAPI SimConnect_WeatherCreateThermal(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, float lat, float lon, float alt, float radius, float height, float coreRate = 3.0f, float coreTurbulence = 0.05f, float sinkRate = 3.0f, float sinkTurbulence = 0.2f, float coreSize = 0.4f, float coreTransitionSize = 0.1f, float sinkLayerSize = 0.4f, float sinkTransitionSize = 0.1f);
SIMCONNECTAPI SimConnect_WeatherRemoveThermal(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID);
SIMCONNECTAPI SimConnect_AICreateParkedATCAircraft(HANDLE hSimConnect, const char * szContainerTitle, const char * szTailNumber, const char * szAirportID, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AICreateEnrouteATCAircraft(HANDLE hSimConnect, const char * szContainerTitle, const char * szTailNumber, int iFlightNumber, const char * szFlightPlanPath, double dFlightPlanPosition, BOOL bTouchAndGo, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AICreateNonATCAircraft(HANDLE hSimConnect, const char * szContainerTitle, const char * szTailNumber, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AICreateSimulatedObject(HANDLE hSimConnect, const char * szContainerTitle, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AIReleaseControl(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AIRemoveObject(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AISetAircraftFlightPlan(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, const char * szFlightPlanPath, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_ExecuteMissionAction(HANDLE hSimConnect, const GUID guidInstanceId);
SIMCONNECTAPI SimConnect_CompleteCustomMissionAction(HANDLE hSimConnect, const GUID guidInstanceId);
SIMCONNECTAPI SimConnect_CameraSetRelative6DOF(HANDLE hSimConnect, float fDeltaX, float fDeltaY, float fDeltaZ, float fPitchDeg, float fBankDeg, float fHeadingDeg);
SIMCONNECTAPI SimConnect_MenuAddItem(HANDLE hSimConnect, const char * szMenuItem, SIMCONNECT_CLIENT_EVENT_ID MenuEventID, DWORD dwData);
SIMCONNECTAPI SimConnect_MenuDeleteItem(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID MenuEventID);
SIMCONNECTAPI SimConnect_MenuAddSubItem(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID MenuEventID, const char * szMenuItem, SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID, DWORD dwData);
SIMCONNECTAPI SimConnect_MenuDeleteSubItem(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID MenuEventID, const SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID);
SIMCONNECTAPI SimConnect_RequestSystemState(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szState);
SIMCONNECTAPI SimConnect_SetSystemState(HANDLE hSimConnect, const char * szState, DWORD dwInteger, float fFloat, const char * szString);
SIMCONNECTAPI SimConnect_MapClientDataNameToID(HANDLE hSimConnect, const char * szClientDataName, SIMCONNECT_CLIENT_DATA_ID ClientDataID);
SIMCONNECTAPI SimConnect_CreateClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, DWORD dwSize, SIMCONNECT_CREATE_CLIENT_DATA_FLAG Flags);
SIMCONNECTAPI SimConnect_AddToClientDataDefinition(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, DWORD dwOffset, DWORD dwSizeOrType, float fEpsilon = 0, DWORD DatumID = SIMCONNECT_UNUSED);
SIMCONNECTAPI SimConnect_ClearClientDataDefinition(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID);
SIMCONNECTAPI SimConnect_RequestClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, SIMCONNECT_CLIENT_DATA_PERIOD Period = SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG Flags = 0, DWORD origin = 0, DWORD interval = 0, DWORD limit = 0);
SIMCONNECTAPI SimConnect_SetClientData(HANDLE hSimConnect, SIMCONNECT_CLIENT_DATA_ID ClientDataID, SIMCONNECT_CLIENT_DATA_DEFINITION_ID DefineID, SIMCONNECT_CLIENT_DATA_SET_FLAG Flags, DWORD dwReserved, DWORD cbUnitSize, void * pDataSet);
SIMCONNECTAPI SimConnect_FlightLoad(HANDLE hSimConnect, const char * szFileName);
SIMCONNECTAPI SimConnect_FlightSave(HANDLE hSimConnect, const char * szFileName, const char * szTitle, const char * szDescription, DWORD Flags);
SIMCONNECTAPI SimConnect_FlightPlanLoad(HANDLE hSimConnect, const char * szFileName);
SIMCONNECTAPI SimConnect_Text(HANDLE hSimConnect, SIMCONNECT_TEXT_TYPE type, float fTimeSeconds, SIMCONNECT_CLIENT_EVENT_ID EventID, DWORD cbUnitSize, void * pDataSet);
SIMCONNECTAPI SimConnect_SubscribeToFacilities(HANDLE hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE type, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_UnsubscribeToFacilities(HANDLE hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE type);
SIMCONNECTAPI SimConnect_RequestFacilitiesList(HANDLE hSimConnect, SIMCONNECT_FACILITY_LIST_TYPE type, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_ChangeVehicle(HANDLE hSimConnect, const char * vehicleName);
SIMCONNECTAPI SimConnect_SetCameraRenderSettings(HANDLE hSimConnect, const GUID guidCamera, DWORD dwFlags);
SIMCONNECTAPI SimConnect_SendCameraCommand(HANDLE hSimConnect, const GUID guidCamera, SIMCONNECT_CAMERA_COMMAND eCommand);
SIMCONNECTAPI SimConnect_CreateObserver(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_OBSERVER ObserverData);
SIMCONNECTAPI SimConnect_RequestObserverData(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szName);
SIMCONNECTAPI SimConnect_MoveObserver(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_XYZ Translation);
SIMCONNECTAPI SimConnect_RotateObserver(HANDLE hSimConnect, const char * szName, DWORD dwAxis, double dAngleDegrees);
SIMCONNECTAPI SimConnect_SetObserverPosition(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_LATLONALT Position);
SIMCONNECTAPI SimConnect_SetObserverRotation(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_PBH RotationAngles);
SIMCONNECTAPI SimConnect_SetObserverLookAt(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_LATLONALT TargetPosition);
SIMCONNECTAPI SimConnect_SetObserverFieldOfView(HANDLE hSimConnect, const char * szName, float fHorizontal, float fVertical);
SIMCONNECTAPI SimConnect_SetObserverStepSize(HANDLE hSimConnect, const char * szName, float fLinearStep, float fAngularStep);
SIMCONNECTAPI SimConnect_SetObserverFocalLength(HANDLE hSimConnect, const char * szName, float fFocalLength);
SIMCONNECTAPI SimConnect_SetObserverFocusFixed(HANDLE hSimConnect, const char * szName, BOOL bFocusFixed);
SIMCONNECTAPI SimConnect_SetObserverRegime(HANDLE hSimConnect, const char * szName, DWORD eRegime);
SIMCONNECTAPI SimConnect_SetObserverZoomLevels(HANDLE hSimConnect, const char * szName, DWORD dwNumLevels);
SIMCONNECTAPI SimConnect_RequestGroundInfo(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, double minLat, double minLon, double minAlt, double maxLat, double maxLon, double maxAlt, DWORD dwGridWidth, DWORD dwGridHeight, SIMCONNECT_GROUND_INFO_LATLON_FORMAT eLatLonFormat, SIMCONNECT_GROUND_INFO_ALT_FORMAT eAltFormat, SIMCONNECT_GROUND_INFO_SOURCE_FLAG eSourceFlags);
SIMCONNECTAPI SimConnect_RequestGroundInfoOnSimObject(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, DWORD ObjectID, double offLat, double offsetLon, double offsetAlt, DWORD dwGridWidth, DWORD dwGridHeight, SIMCONNECT_GROUND_INFO_LATLON_FORMAT eLatLonFormat, SIMCONNECT_GROUND_INFO_ALT_FORMAT eAltFormat, SIMCONNECT_GROUND_INFO_SOURCE_FLAG eSourceFlags, SIMCONNECT_PERIOD Period, SIMCONNECT_DATA_REQUEST_FLAG Flags = 0, DWORD origin = 0, DWORD interval = 0, DWORD limit = 0);
SIMCONNECTAPI SimConnect_RegisterExternalSim(HANDLE hSimConnect, const GUID guidExternalSimID, SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG CallbackMask, SIMCONNECT_DATA_DEFINITION_ID DefineID);
SIMCONNECTAPI SimConnect_UnregisterExternalSim(HANDLE hSimConnect, const GUID guidExternalSimID);
SIMCONNECTAPI SimConnect_RegisterExternalSecondarySim(HANDLE hSimConnect, const GUID guidExternalSecondarySimID, SIMCONNECT_EXTERNAL_SIM_CALLBACK_FLAG CallbackMask, SIMCONNECT_DATA_DEFINITION_ID DefineID);
SIMCONNECTAPI SimConnect_UnregisterExternalSecondarySim(HANDLE hSimConnect, const GUID guidExternalSecondarySimID);
SIMCONNECTAPI SimConnect_AttachExternalSecondarySimToSimObject(HANDLE hSimConnect, DWORD dwObjectID, const GUID guidExternalSecondarySimID, const char * szExternalSimParams, DWORD dwExternalSimVarCount);
SIMCONNECTAPI SimConnect_DetachExternalSecondarySimFromSimObject(HANDLE hSimConnect, DWORD dwObjectID, const GUID guidExternalSecondarySimID);
SIMCONNECTAPI SimConnect_AICreateObjectWithExternalSim(HANDLE hSimConnect, const char * szContainerTitle, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID guidExternalSimID, const char * szExternalSimParams, DWORD dwExternalSimVarCount);
SIMCONNECTAPI SimConnect_ChangeVehicleWithExternalSim(HANDLE hSimConnect, const char * szContainerTitle, const GUID guidExternalSimID, const char * szExternalSimParams, DWORD dwExternalSimVarCount);
SIMCONNECTAPI SimConnect_RequestSynchronousBlock(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_PERIOD Period, SIMCONNECT_DATA_REQUEST_FLAG Flags = 0, DWORD origin = 0, DWORD interval = 0, DWORD limit = 0);
SIMCONNECTAPI SimConnect_SynchronousUnblock(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_SetSynchronousTimeout(HANDLE hSimConnect, float fTimeSeconds);
SIMCONNECTAPI SimConnect_SubscribeToSystemEventEx(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID EventID, const char * SystemEventName, SIMCONNECT_EVENT_SUBSCRIPTION_FLAG Flags);
SIMCONNECTAPI SimConnect_AttachObjectToSimObject(HANDLE hSimConnect, DWORD dwObjectID, SIMCONNECT_DATA_XYZ vecOff1, SIMCONNECT_DATA_PBH rotOff1, const char * szContainerTitle, SIMCONNECT_DATA_XYZ vecOff2, SIMCONNECT_DATA_PBH rotOff2, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_AttachSimObjectToSimObject(HANDLE hSimConnect, DWORD dwObjectID1, SIMCONNECT_DATA_XYZ vecOff1, SIMCONNECT_DATA_PBH rotOff1, DWORD dwObjectID2, SIMCONNECT_DATA_XYZ vecOff2, SIMCONNECT_DATA_PBH rotOff2);
SIMCONNECTAPI SimConnect_ReleaseSimObjectFromSimObject(HANDLE hSimConnect, DWORD dwObjectID1, DWORD dwObjectID2);
SIMCONNECTAPI SimConnect_AISetGroundClamp(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, BOOL bGroundClamp = TRUE);
SIMCONNECTAPI SimConnect_SetCameraColorizationMode(HANDLE hSimConnect, const char * szName, int eColorizationMode);
SIMCONNECTAPI SimConnect_SetCameraSensorMode(HANDLE hSimConnect, const char * szName, SIMCONNECT_CAMERA_SENSOR_MODE eSensorMode);
SIMCONNECTAPI SimConnect_CreateEffect(HANDLE hSimConnect, const char * effectName, SIMCONNECT_DATA_LATLONALT TargetPosition, SIMCONNECT_DATA_XYZ offset, BOOL attachToSimObject, DWORD dwObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID, int effectDuration = -1);
SIMCONNECTAPI SimConnect_RemoveEffect(HANDLE hSimConnect, const DWORD effectId);
SIMCONNECTAPI SimConnect_AttachWeaponToObject(HANDLE hSimConnect, const char * szContainerTitle, DWORD dwObjectID, int hardpointIndex, int numOfRounds);
SIMCONNECTAPI SimConnect_ClearWeapons(HANDLE hSimConnect, DWORD dwObjectID);
SIMCONNECTAPI SimConnect_CreateCameraDefinition(HANDLE hSimConnect, const GUID guidCamera, SIMCONNECT_CAMERA_TYPE ViewType, const char * szTitle, SIMCONNECT_DATA_XYZ xyzBias, SIMCONNECT_DATA_PBH pbhBias);
SIMCONNECTAPI SimConnect_CreateCameraInstance(HANDLE hSimConnect, const GUID guidCamera, const char * szName, SIMCONNECT_OBJECT_ID dwObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_DeleteCameraInstance(HANDLE hSimConnect, const GUID guidCamera, UINT32 instanceId);
SIMCONNECTAPI SimConnect_SetCameraHorizontalFov(HANDLE hSimConnect, const char * szName, float hFov);
SIMCONNECTAPI SimConnect_SetCameraVerticalFov(HANDLE hSimConnect, const char * szName, float vFov);
SIMCONNECTAPI SimConnect_SetCameraFov(HANDLE hSimConnect, const char * szName, float hFov, float vFov);
SIMCONNECTAPI SimConnect_CameraSetRelative6DofByName(HANDLE hSimConnect, const char * szName, float fDeltaX, float fDeltaY, float fDeltaZ, float fPitchDeg, float fBankDeg, float fHeadingDeg);
SIMCONNECTAPI SimConnect_SetMainCameraSensorMode(HANDLE hSimConnect, SIMCONNECT_CAMERA_SENSOR_MODE eSensorMode);
SIMCONNECTAPI SimConnect_SetMainCameraHorizontalFov(HANDLE hSimConnect, float hFov);
SIMCONNECTAPI SimConnect_SetMainCameraVerticalFov(HANDLE hSimConnect, float vFov);
SIMCONNECTAPI SimConnect_SetMainCameraFov(HANDLE hSimConnect, float hFov, float vFov);
SIMCONNECTAPI SimConnect_CameraZoomIn(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_CameraZoomOut(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_MainCameraZoomIn(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_MainCameraZoomOut(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_OpenView(HANDLE hSimConnect, const char * szName, const char * szTitle = NULL);
SIMCONNECTAPI SimConnect_CloseView(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_UndockView(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_DockView(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_SetCameraWindowPosition(HANDLE hSimConnect, const char * szName, UINT uX, UINT uY);
SIMCONNECTAPI SimConnect_SetCameraWindowSize(HANDLE hSimConnect, const char * szName, UINT uWidth, UINT uHeight);
SIMCONNECTAPI SimConnect_ChangeView(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_AddPostProcess(HANDLE hSimConnect, const char * szCameraName, const char * szPostProcessName);
SIMCONNECTAPI SimConnect_AddPostProcessMainCamera(HANDLE hSimConnect, const char * szPostProcessName);
SIMCONNECTAPI SimConnect_RemovePostProcess(HANDLE hSimConnect, const char * szCameraName, const char * szPostProcessName);
SIMCONNECTAPI SimConnect_RemovePostProcessMainCamera(HANDLE hSimConnect, const char * szPostProcessName);
SIMCONNECTAPI SimConnect_CameraSmoothRelative6DOF(HANDLE hSimConnect, float fDeltaX, float fDeltaY, float fDeltaZ, float fPitchDeg, float fBankDeg, float fHeadingDeg, float fSmoothTime = 0);
SIMCONNECTAPI SimConnect_CameraSmoothRelative6DofByName(HANDLE hSimConnect, const char * szName, float fDeltaX, float fDeltaY, float fDeltaZ, float fPitchDeg, float fBankDeg, float fHeadingDeg, float fSmoothTime = 0);
SIMCONNECTAPI SimConnect_CameraPanToView(HANDLE hSimConnect, const char * szCameraName, const char * szTargetName, float fSmoothTime = 0);
SIMCONNECTAPI SimConnect_MainCameraPanToView(HANDLE hSimConnect, const char * szTargetName, float fSmoothTime = 0);
SIMCONNECTAPI SimConnect_AICreateSimulatedObjectEx(HANDLE hSimConnect, const char * szContainerTitle, SIMCONNECT_DATA_INITPOSITION InitPos, SIMCONNECT_DATA_REQUEST_ID RequestID, BOOL isDisabled = FALSE, int OwnerID = -1);
SIMCONNECTAPI SimConnect_AIReleaseControlEx(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_DATA_REQUEST_ID RequestID, BOOL destroyAI = FALSE);
SIMCONNECTAPI SimConnect_ReportWeaponDamage(HANDLE hSimConnect, SIMCONNECT_DATA_OBJECT_DAMAGED_BY_WEAPON WeaponDamageData);
SIMCONNECTAPI SimConnect_RequestVersion(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestSceneryComplexity(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestShadowFlags(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_ObserverTrackEntityOn(HANDLE hSimConnect, const char * szName, DWORD dwObjectID);
SIMCONNECTAPI SimConnect_ObserverTrackEntityOff(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_ObserverAttachToEntityOn(HANDLE hSimConnect, const char * szName, DWORD dwObjectID, SIMCONNECT_DATA_XYZ Offset);
SIMCONNECTAPI SimConnect_ObserverAttachToEntityOff(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_SetObserverLookAtEx(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_LATLONALT TargetPosition);
SIMCONNECTAPI SimConnect_RequestCameraFov(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestCameraRelative6DOF(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestCameraRelative6DofByName(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestCameraSensorMode(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestCameraWindowPosition(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestCameraWindowSize(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestMainCameraFov(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestMainCameraSensorMode(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestGoalDataByGUID(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID guidInstanceID);
SIMCONNECTAPI SimConnect_RequestGoalDataByIndex(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, int goalIndex);
SIMCONNECTAPI SimConnect_ResolveGoal(HANDLE hSimConnect, const GUID guidInstanceID, SIMCONNECT_GOAL_RESOLUTION goalResolution);
SIMCONNECTAPI SimConnect_RequestGoalCount(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestMissionObjectiveCount(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestMissionObjectiveDataByGUID(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID guidInstanceID);
SIMCONNECTAPI SimConnect_RequestMissionObjectiveDataByIndex(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, int missionObjectiveIndex);
SIMCONNECTAPI SimConnect_RequestFlightSegmentCount(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestFlightSegmentDataByGUID(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID guidInstanceID);
SIMCONNECTAPI SimConnect_RequestFlightSegmentDataByIndex(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, int flightSegmentIndex);
SIMCONNECTAPI SimConnect_RequestFlightSegmentRangeData(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID guidInstanceID, int rangeIndex);
SIMCONNECTAPI SimConnect_RequestChildGoalDataByIndex(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID parentGuidInstanceID, int goalIndex);
SIMCONNECTAPI SimConnect_GenerateFlightAnalysisDiagrams(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_RequestLandingTriggerCount(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestLandingTriggerLandingInfoCount(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, int landingTriggerIndex);
SIMCONNECTAPI SimConnect_RequestLandingTriggerLandingInfoByIndex(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const GUID landingTriggerInstanceID, int landingIndex);
SIMCONNECTAPI SimConnect_RequestSessionDuration(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestAttachPointData(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, DWORD dwObjectID, const char * szAttachPointName, BOOL bRequestWorldCoordinates = FALSE);
SIMCONNECTAPI SimConnect_PlaybackRecording(HANDLE hSimConnect, const char * szFileName, int bookmarkIndex, double endTimeInSeconds);
SIMCONNECTAPI SimConnect_StartRecorder(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_StopRecorderAndSaveRecording(HANDLE hSimConnect, const char * szTitle, const char * szDescription, BOOL promptUser);
SIMCONNECTAPI SimConnect_RequestRecordingInfo(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szFileName);
SIMCONNECTAPI SimConnect_RequestBookmarkInfo(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, const char * szFileName, int bookmarkIndex);
SIMCONNECTAPI SimConnect_ObserverTrackLocationOn(HANDLE hSimConnect, const char * szName, SIMCONNECT_DATA_LATLONALT TargetPosition);
SIMCONNECTAPI SimConnect_ObserverTrackLocationOff(HANDLE hSimConnect, const char * szName);
SIMCONNECTAPI SimConnect_SetObserverSceneryOrigin(HANDLE hSimConnect, const char * szName, DWORD eOrigin);
SIMCONNECTAPI SimConnect_RequestTrafficSettings(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_SetTrafficSettings(HANDLE hSimConnect, UINT uAirlineDensity, UINT uGADensity, UINT uRoadTrafficDensity, UINT uShipsAndFerriesDensity, UINT uLeisureBoatDensity, SIMCONNECT_DYNAMIC_FREQUENCY eAirportVehicleDensity, BOOL bIFROnly);
SIMCONNECTAPI SimConnect_RequestJoystickDeviceInfo(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID);
SIMCONNECTAPI SimConnect_RequestMobileSceneryInRadius(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, DWORD dwRadiusMeters);
SIMCONNECTAPI SimConnect_RequestMobileSceneryDataByID(HANDLE hSimConnect, SIMCONNECT_DATA_REQUEST_ID RequestID, SIMCONNECT_OBJECT_ID ObjectID);
SIMCONNECTAPI SimConnect_CaptureImage(HANDLE hSimConnect, const char * szFileName, const char * szFilePath, UINT uImageFormat, const char * szViewName, BOOL bFileNameContainsExtension = FALSE);
SIMCONNECTAPI SimConnect_BeginVideoStream(HANDLE hSimConnect, const char * szViewName, SIMCONNECT_DATA_VIDEO_STREAM_INFO StreamInfo);
SIMCONNECTAPI SimConnect_EndVideoStream(HANDLE hSimConnect, const char * szViewName, SIMCONNECT_DATA_VIDEO_STREAM_INFO StreamInfo);
SIMCONNECTAPI SimConnect_InsertStringW(BYTE * pDest, DWORD cbDest, BYTE ** ppEnd, DWORD * pcbStringV, const wchar_t * pSource);
SIMCONNECTAPI SimConnect_RetrieveStringW(SIMCONNECT_RECV * pData, DWORD cbData, void * pStringV, wchar_t ** pszString, DWORD * pcbString);
SIMCONNECTAPI SimConnect_TransmitClientEvent64(HANDLE hSimConnect, SIMCONNECT_OBJECT_ID ObjectID, SIMCONNECT_CLIENT_EVENT_ID EventID, QWORD qwData, SIMCONNECT_NOTIFICATION_GROUP_ID GroupID, SIMCONNECT_EVENT_FLAG Flags);
SIMCONNECTAPI SimConnect_MenuAddItem64(HANDLE hSimConnect, const char * szMenuItem, SIMCONNECT_CLIENT_EVENT_ID MenuEventID, QWORD qwData);
SIMCONNECTAPI SimConnect_MenuAddSubItem64(HANDLE hSimConnect, SIMCONNECT_CLIENT_EVENT_ID MenuEventID, const char * szMenuItem, SIMCONNECT_CLIENT_EVENT_ID SubMenuEventID, QWORD qwData);


#endif // _SIMCONNECT_H_


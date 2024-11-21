//-----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation. All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SIMCONNECT_H_
#define _SIMCONNECT_H_

#pragma once

#ifdef _MSFS_WASM
	#ifndef SIMCONNECT_WASM_MODULE
		#define SIMCONNECT_WASM_MODULE "env"
	#endif
#endif

#ifndef DWORD_MAX
#define DWORD_MAX 0xFFFFFFFF
#endif

#include <float.h>

typedef DWORD SIMCONNECT_OBJECT_ID;

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

//these came from substituteMacros
#define SIMCONNECT_REFSTRUCT struct
#define SIMCONNECT_STRUCT struct
#define SIMCONNECT_STRING(name, size) char name[size]
#define SIMCONNECT_GUID GUID
#define SIMCONNECT_STRINGV(name) char name[1]
#define SIMCONNECT_DATAV(name, id, count) DWORD name
#define SIMCONNECT_FIXEDTYPE_DATAV(type, name, count, cliMarshalAs, cliType) type name[1]
#define SIMCONNECT_GUID GUID
#define SIMCONNECT_ENUM enum
#define SIMCONNECT_ENUM_FLAGS typedef DWORD
#define SIMCONNECT_USER_ENUM typedef DWORD


// Receive data types
SIMCONNECT_ENUM SIMCONNECT_RECV_ID {
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
#ifdef ENABLE_SIMCONNECT_EXPERIMENTAL
    SIMCONNECT_RECV_ID_PICK,
#endif //ENABLE_SIMCONNECT_EXPERIMENTAL
};



// Data data types
SIMCONNECT_ENUM SIMCONNECT_DATATYPE {
    SIMCONNECT_DATATYPE_INVALID,        // invalid data type
    SIMCONNECT_DATATYPE_INT32,          // 32-bit integer number
    SIMCONNECT_DATATYPE_INT64,          // 64-bit integer number
    SIMCONNECT_DATATYPE_FLOAT32,        // 32-bit floating-point number (float)
    SIMCONNECT_DATATYPE_FLOAT64,        // 64-bit floating-point number (double)
    SIMCONNECT_DATATYPE_STRING8,        // 8-byte string
    SIMCONNECT_DATATYPE_STRING32,       // 32-byte string
    SIMCONNECT_DATATYPE_STRING64,       // 64-byte string
    SIMCONNECT_DATATYPE_STRING128,      // 128-byte string
    SIMCONNECT_DATATYPE_STRING256,      // 256-byte string
    SIMCONNECT_DATATYPE_STRING260,      // 260-byte string
    SIMCONNECT_DATATYPE_STRINGV,        // variable-length string

    SIMCONNECT_DATATYPE_INITPOSITION,   // see SIMCONNECT_DATA_INITPOSITION
    SIMCONNECT_DATATYPE_MARKERSTATE,    // see SIMCONNECT_DATA_MARKERSTATE
    SIMCONNECT_DATATYPE_WAYPOINT,       // see SIMCONNECT_DATA_WAYPOINT
    SIMCONNECT_DATATYPE_LATLONALT,      // see SIMCONNECT_DATA_LATLONALT
    SIMCONNECT_DATATYPE_XYZ,            // see SIMCONNECT_DATA_XYZ

    SIMCONNECT_DATATYPE_MAX             // enum limit
};

// Exception error types
SIMCONNECT_ENUM SIMCONNECT_EXCEPTION {
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
};

// Object types
SIMCONNECT_ENUM SIMCONNECT_SIMOBJECT_TYPE {
    SIMCONNECT_SIMOBJECT_TYPE_USER,
    SIMCONNECT_SIMOBJECT_TYPE_ALL,
    SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT,
    SIMCONNECT_SIMOBJECT_TYPE_HELICOPTER,
    SIMCONNECT_SIMOBJECT_TYPE_BOAT,
    SIMCONNECT_SIMOBJECT_TYPE_GROUND,
};

// EventState values
SIMCONNECT_ENUM SIMCONNECT_STATE {
    SIMCONNECT_STATE_OFF,
    SIMCONNECT_STATE_ON,
};

// Object Data Request Period values
SIMCONNECT_ENUM SIMCONNECT_PERIOD {
    SIMCONNECT_PERIOD_NEVER,
    SIMCONNECT_PERIOD_ONCE,
    SIMCONNECT_PERIOD_VISUAL_FRAME,
    SIMCONNECT_PERIOD_SIM_FRAME,
    SIMCONNECT_PERIOD_SECOND,
};


SIMCONNECT_ENUM SIMCONNECT_MISSION_END {
    SIMCONNECT_MISSION_FAILED,
    SIMCONNECT_MISSION_CRASHED,
    SIMCONNECT_MISSION_SUCCEEDED
};

// ClientData Request Period values
SIMCONNECT_ENUM SIMCONNECT_CLIENT_DATA_PERIOD {
    SIMCONNECT_CLIENT_DATA_PERIOD_NEVER,
    SIMCONNECT_CLIENT_DATA_PERIOD_ONCE,
    SIMCONNECT_CLIENT_DATA_PERIOD_VISUAL_FRAME,
    SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET,
    SIMCONNECT_CLIENT_DATA_PERIOD_SECOND,
};

SIMCONNECT_ENUM SIMCONNECT_TEXT_TYPE {
    SIMCONNECT_TEXT_TYPE_SCROLL_BLACK,
    SIMCONNECT_TEXT_TYPE_SCROLL_WHITE,
    SIMCONNECT_TEXT_TYPE_SCROLL_RED,
    SIMCONNECT_TEXT_TYPE_SCROLL_GREEN,
    SIMCONNECT_TEXT_TYPE_SCROLL_BLUE,
    SIMCONNECT_TEXT_TYPE_SCROLL_YELLOW,
    SIMCONNECT_TEXT_TYPE_SCROLL_MAGENTA,
    SIMCONNECT_TEXT_TYPE_SCROLL_CYAN,
    SIMCONNECT_TEXT_TYPE_PRINT_BLACK=0x0100,
    SIMCONNECT_TEXT_TYPE_PRINT_WHITE,
    SIMCONNECT_TEXT_TYPE_PRINT_RED,
    SIMCONNECT_TEXT_TYPE_PRINT_GREEN,
    SIMCONNECT_TEXT_TYPE_PRINT_BLUE,
    SIMCONNECT_TEXT_TYPE_PRINT_YELLOW,
    SIMCONNECT_TEXT_TYPE_PRINT_MAGENTA,
    SIMCONNECT_TEXT_TYPE_PRINT_CYAN,
    SIMCONNECT_TEXT_TYPE_MENU=0x0200,
};

SIMCONNECT_ENUM SIMCONNECT_TEXT_RESULT {
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

SIMCONNECT_ENUM SIMCONNECT_WEATHER_MODE {
    SIMCONNECT_WEATHER_MODE_THEME,
    SIMCONNECT_WEATHER_MODE_RWW,
    SIMCONNECT_WEATHER_MODE_CUSTOM,
    SIMCONNECT_WEATHER_MODE_GLOBAL,
};

SIMCONNECT_ENUM SIMCONNECT_FACILITY_LIST_TYPE {
    SIMCONNECT_FACILITY_LIST_TYPE_AIRPORT,
    SIMCONNECT_FACILITY_LIST_TYPE_WAYPOINT,
    SIMCONNECT_FACILITY_LIST_TYPE_NDB,
    SIMCONNECT_FACILITY_LIST_TYPE_VOR,
    SIMCONNECT_FACILITY_LIST_TYPE_COUNT // invalid 
};


SIMCONNECT_ENUM_FLAGS SIMCONNECT_VOR_FLAGS;            // flags for SIMCONNECT_RECV_ID_VOR_LIST 
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_NAV_SIGNAL  = 0x00000001;   // Has Nav signal
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_LOCALIZER   = 0x00000002;   // Has localizer
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_GLIDE_SLOPE = 0x00000004;   // Has Nav signal
    static const DWORD SIMCONNECT_RECV_ID_VOR_LIST_HAS_DME         = 0x00000008;   // Station has DME



// bits for the Waypoint Flags field: may be combined
SIMCONNECT_ENUM_FLAGS SIMCONNECT_WAYPOINT_FLAGS;
    static const DWORD SIMCONNECT_WAYPOINT_NONE                    = 0x00;
    static const DWORD SIMCONNECT_WAYPOINT_SPEED_REQUESTED         = 0x04;    // requested speed at waypoint is valid
    static const DWORD SIMCONNECT_WAYPOINT_THROTTLE_REQUESTED      = 0x08;    // request a specific throttle percentage
    static const DWORD SIMCONNECT_WAYPOINT_COMPUTE_VERTICAL_SPEED  = 0x10;    // compute vertical to speed to reach waypoint altitude when crossing the waypoint
    static const DWORD SIMCONNECT_WAYPOINT_ALTITUDE_IS_AGL         = 0x20;    // AltitudeIsAGL
    static const DWORD SIMCONNECT_WAYPOINT_ON_GROUND               = 0x00100000;   // place this waypoint on the ground
    static const DWORD SIMCONNECT_WAYPOINT_REVERSE                 = 0x00200000;   // Back up to this waypoint. Only valid on first waypoint
    static const DWORD SIMCONNECT_WAYPOINT_WRAP_TO_FIRST           = 0x00400000;   // Wrap around back to first waypoint. Only valid on last waypoint.

SIMCONNECT_ENUM_FLAGS SIMCONNECT_EVENT_FLAG;
    static const DWORD SIMCONNECT_EVENT_FLAG_DEFAULT                  = 0x00000000;
    static const DWORD SIMCONNECT_EVENT_FLAG_FAST_REPEAT_TIMER        = 0x00000001;      // set event repeat timer to simulate fast repeat
    static const DWORD SIMCONNECT_EVENT_FLAG_SLOW_REPEAT_TIMER        = 0x00000002;      // set event repeat timer to simulate slow repeat
    static const DWORD SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY      = 0x00000010;      // interpret GroupID parameter as priority value

SIMCONNECT_ENUM_FLAGS SIMCONNECT_DATA_REQUEST_FLAG;
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_DEFAULT           = 0x00000000;
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_CHANGED           = 0x00000001;      // send requested data when value(s) change
    static const DWORD SIMCONNECT_DATA_REQUEST_FLAG_TAGGED            = 0x00000002;      // send requested data in tagged format

SIMCONNECT_ENUM_FLAGS SIMCONNECT_DATA_SET_FLAG;
    static const DWORD SIMCONNECT_DATA_SET_FLAG_DEFAULT               = 0x00000000;
    static const DWORD SIMCONNECT_DATA_SET_FLAG_TAGGED                = 0x00000001;      // data is in tagged format

SIMCONNECT_ENUM_FLAGS SIMCONNECT_CREATE_CLIENT_DATA_FLAG;
    static const DWORD SIMCONNECT_CREATE_CLIENT_DATA_FLAG_DEFAULT     = 0x00000000;
    static const DWORD SIMCONNECT_CREATE_CLIENT_DATA_FLAG_READ_ONLY   = 0x00000001;      // permit only ClientData creator to write into ClientData


SIMCONNECT_ENUM_FLAGS SIMCONNECT_CLIENT_DATA_REQUEST_FLAG;
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_DEFAULT    = 0x00000000;
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED    = 0x00000001;      // send requested ClientData when value(s) change
    static const DWORD SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED     = 0x00000002;      // send requested ClientData in tagged format

SIMCONNECT_ENUM_FLAGS SIMCONNECT_CLIENT_DATA_SET_FLAG;
    static const DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG_DEFAULT        = 0x00000000;
    static const DWORD SIMCONNECT_CLIENT_DATA_SET_FLAG_TAGGED         = 0x00000001;      // data is in tagged format


SIMCONNECT_ENUM_FLAGS SIMCONNECT_VIEW_SYSTEM_EVENT_DATA;                // dwData contains these flags for the "View" System Event
    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_COCKPIT_2D      = 0x00000001;      // 2D Panels in cockpit view
    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_COCKPIT_VIRTUAL = 0x00000002;      // Virtual (3D) panels in cockpit view
    static const DWORD SIMCONNECT_VIEW_SYSTEM_EVENT_DATA_ORTHOGONAL      = 0x00000004;      // Orthogonal (Map) view

SIMCONNECT_ENUM_FLAGS SIMCONNECT_SOUND_SYSTEM_EVENT_DATA;            // dwData contains these flags for the "Sound" System Event
    static const DWORD SIMCONNECT_SOUND_SYSTEM_EVENT_DATA_MASTER    = 0x00000001;      // Sound Master


#ifdef ENABLE_SIMCONNECT_EXPERIMENTAL

SIMCONNECT_ENUM_FLAGS SIMCONNECT_PICK_FLAGS
{
    SIMCONNECT_PICK_GROUND           = 0x01,    // pick ground/ pick result item is ground location
    SIMCONNECT_PICK_AI               = 0x02,    // pick AI    / pick result item is AI, (dwSimObjectID is valid)
    SIMCONNECT_PICK_SCENERY          = 0x04,    // pick scenery/ pick result item is scenery object (hSceneryObject is valid)
    SIMCONNECT_PICK_ALL              = SIMCONNECT_PICK_SCENERY | SIMCONNECT_PICK_AI | SIMCONNECT_PICK_GROUND, // pick all / (not valid on pick result item)
    SIMCONNECT_PICK_COORDSASPIXELS   = 0x08,
};

#endif //ENABLE_SIMCONNECT_EXPERIMENTAL

//----------------------------------------------------------------------------
//        User-defined enums
//----------------------------------------------------------------------------

SIMCONNECT_USER_ENUM SIMCONNECT_NOTIFICATION_GROUP_ID;     //client-defined notification group ID
SIMCONNECT_USER_ENUM SIMCONNECT_INPUT_GROUP_ID;            //client-defined input group ID
SIMCONNECT_USER_ENUM SIMCONNECT_DATA_DEFINITION_ID;        //client-defined data definition ID
SIMCONNECT_USER_ENUM SIMCONNECT_DATA_REQUEST_ID;           //client-defined request data ID
 
SIMCONNECT_USER_ENUM SIMCONNECT_CLIENT_EVENT_ID;           //client-defined client event ID
SIMCONNECT_USER_ENUM SIMCONNECT_CLIENT_DATA_ID;            //client-defined client data ID
SIMCONNECT_USER_ENUM SIMCONNECT_CLIENT_DATA_DEFINITION_ID; //client-defined client data definition ID


//----------------------------------------------------------------------------
//        Struct definitions
//----------------------------------------------------------------------------

#pragma pack(push, 1)

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV
{
    DWORD   dwSize;         // record size
    DWORD   dwVersion;      // interface version
    DWORD   dwID;           // see SIMCONNECT_RECV_ID
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EXCEPTION : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_EXCEPTION
{
    DWORD   dwException;    // see SIMCONNECT_EXCEPTION
    static const DWORD UNKNOWN_SENDID = 0;
    DWORD   dwSendID;       // see SimConnect_GetLastSentPacketID
    static const DWORD UNKNOWN_INDEX = DWORD_MAX;
    DWORD   dwIndex;        // index of parameter that was source of error
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_OPEN : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_OPEN
{
    SIMCONNECT_STRING(    szApplicationName, 256);
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

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_QUIT : public SIMCONNECT_RECV   // when dwID == SIMCONNECT_RECV_ID_QUIT
{
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT : public SIMCONNECT_RECV       // when dwID == SIMCONNECT_RECV_ID_EVENT
{
    static const DWORD UNKNOWN_GROUP = DWORD_MAX;
    DWORD   uGroupID;
    DWORD   uEventID; 
    DWORD   dwData;       // uEventID-dependent context
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_FILENAME : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_FILENAME
{
    SIMCONNECT_STRING(    szFileName, MAX_PATH);   // uEventID-dependent context
    DWORD   dwFlags;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_OBJECT_ADDREMOVE : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_FILENAME
{
    SIMCONNECT_SIMOBJECT_TYPE   eObjType;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_FRAME : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_FRAME
{
    float   fFrameRate;
    float   fSimSpeed;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_MULTIPLAYER_SERVER_STARTED : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SERVER_STARTED
{
    // No event specific data, for now
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_MULTIPLAYER_CLIENT_STARTED : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_CLIENT_STARTED
{
    // No event specific data, for now
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_MULTIPLAYER_SESSION_ENDED : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_MULTIPLAYER_SESSION_ENDED
{
    // No event specific data, for now
};

// SIMCONNECT_DATA_RACE_RESULT
SIMCONNECT_STRUCT SIMCONNECT_DATA_RACE_RESULT
{
    DWORD   dwNumberOfRacers;                         // The total number of racers
    SIMCONNECT_GUID MissionGUID;                      // The name of the mission to execute, NULL if no mission
    SIMCONNECT_STRING( szPlayerName, MAX_PATH);       // The name of the player
    SIMCONNECT_STRING( szSessionType, MAX_PATH);      // The type of the multiplayer session: "LAN", "GAMESPY")
    SIMCONNECT_STRING( szAircraft, MAX_PATH);         // The aircraft type 
    SIMCONNECT_STRING( szPlayerRole, MAX_PATH);       // The player role in the mission
    double   fTotalTime;                              // Total time in seconds, 0 means DNF
    double   fPenaltyTime;                            // Total penalty time in seconds
    DWORD   dwIsDisqualified;                         // non 0 - disqualified, 0 - not disqualified
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_RACE_END : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_RACE_END
{
    DWORD   dwRacerNumber;                            // The index of the racer the results are for
    SIMCONNECT_DATA_RACE_RESULT RacerData;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_RACE_LAP : public SIMCONNECT_RECV_EVENT       // when dwID == SIMCONNECT_RECV_ID_EVENT_RACE_LAP
{
    DWORD   dwLapIndex;                               // The index of the lap the results are for
    SIMCONNECT_DATA_RACE_RESULT RacerData;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_SIMOBJECT_DATA : public SIMCONNECT_RECV           // when dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA
{
    DWORD   dwRequestID;
    DWORD   dwObjectID;
    DWORD   dwDefineID;
    DWORD   dwFlags;            // SIMCONNECT_DATA_REQUEST_FLAG
    DWORD   dwentrynumber;      // if multiple objects returned, this is number <entrynumber> out of <outof>.
    DWORD   dwoutof;            // note: starts with 1, not 0.          
    DWORD   dwDefineCount;      // data count (number of datums, *not* byte count)
    SIMCONNECT_DATAV(   dwData, dwDefineID, );             // data begins here, dwDefineCount data items
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE : public SIMCONNECT_RECV_SIMOBJECT_DATA           // when dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
{
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_CLIENT_DATA : public SIMCONNECT_RECV_SIMOBJECT_DATA    // when dwID == SIMCONNECT_RECV_ID_CLIENT_DATA
{
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_WEATHER_OBSERVATION : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_WEATHER_OBSERVATION
{
    DWORD   dwRequestID;
    SIMCONNECT_STRINGV( szMetar);      // Variable length string whose maximum size is MAX_METAR_LENGTH
};

static const int SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH = 64;
static const int SIMCONNECT_CLOUD_STATE_ARRAY_SIZE = SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH*SIMCONNECT_CLOUD_STATE_ARRAY_WIDTH;

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_CLOUD_STATE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_CLOUD_STATE
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    SIMCONNECT_FIXEDTYPE_DATAV(BYTE,    rgbData, dwArraySize, U1 /*member of UnmanagedType enum*/ , System::Byte /*cli type*/);
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_ASSIGNED_OBJECT_ID : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
{
    DWORD   dwRequestID;
    DWORD   dwObjectID;
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_RESERVED_KEY : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_RESERVED_KEY
{
    SIMCONNECT_STRING(    szChoiceReserved, 30);
    SIMCONNECT_STRING(    szReservedKey, 50);
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_SYSTEM_STATE : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_SYSTEM_STATE
{
    DWORD   dwRequestID;
    DWORD   dwInteger;
    float   fFloat;
    SIMCONNECT_STRING(    szString, MAX_PATH);
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_CUSTOM_ACTION : public SIMCONNECT_RECV_EVENT
{
    SIMCONNECT_GUID guidInstanceId;      // Instance id of the action that executed
    DWORD dwWaitForCompletion;           // Wait for completion flag on the action
    SIMCONNECT_STRINGV( szPayLoad);      // Variable length string payload associated with the mission action.  
};

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_EVENT_WEATHER_MODE : public SIMCONNECT_RECV_EVENT
{
    // No event specific data - the new weather mode is in the base structure dwData member.
};

// SIMCONNECT_RECV_FACILITIES_LIST
SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_FACILITIES_LIST : public SIMCONNECT_RECV
{
    DWORD   dwRequestID;
    DWORD   dwArraySize;
    DWORD   dwEntryNumber;  // when the array of items is too big for one send, which send this is (0..dwOutOf-1)
    DWORD   dwOutOf;        // total number of transmissions the list is chopped into
};

// SIMCONNECT_DATA_FACILITY_AIRPORT
SIMCONNECT_REFSTRUCT SIMCONNECT_DATA_FACILITY_AIRPORT
{
    SIMCONNECT_STRING(Icao, 9);     // ICAO of the object
    double  Latitude;               // degrees
    double  Longitude;              // degrees
    double  Altitude;               // meters   
};

// SIMCONNECT_RECV_AIRPORT_LIST
SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_AIRPORT_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_FIXEDTYPE_DATAV(SIMCONNECT_DATA_FACILITY_AIRPORT, rgData, dwArraySize, U1 /*member of UnmanagedType enum*/, SIMCONNECT_DATA_FACILITY_AIRPORT /*cli type*/);
};


// SIMCONNECT_DATA_FACILITY_WAYPOINT
SIMCONNECT_REFSTRUCT SIMCONNECT_DATA_FACILITY_WAYPOINT : public SIMCONNECT_DATA_FACILITY_AIRPORT
{
    float   fMagVar;                // Magvar in degrees
};

// SIMCONNECT_RECV_WAYPOINT_LIST
SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_WAYPOINT_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_FIXEDTYPE_DATAV(SIMCONNECT_DATA_FACILITY_WAYPOINT, rgData, dwArraySize, U1 /*member of UnmanagedType enum*/, SIMCONNECT_DATA_FACILITY_WAYPOINT /*cli type*/);
};

// SIMCONNECT_DATA_FACILITY_NDB
SIMCONNECT_REFSTRUCT SIMCONNECT_DATA_FACILITY_NDB : public SIMCONNECT_DATA_FACILITY_WAYPOINT
{
    DWORD   fFrequency;             // frequency in Hz
};

// SIMCONNECT_RECV_NDB_LIST
SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_NDB_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_FIXEDTYPE_DATAV(SIMCONNECT_DATA_FACILITY_NDB, rgData, dwArraySize, U1 /*member of UnmanagedType enum*/, SIMCONNECT_DATA_FACILITY_NDB /*cli type*/);
};

// SIMCONNECT_DATA_FACILITY_VOR
SIMCONNECT_REFSTRUCT SIMCONNECT_DATA_FACILITY_VOR : public SIMCONNECT_DATA_FACILITY_NDB
{
    DWORD   Flags;                  // SIMCONNECT_VOR_FLAGS
    float   fLocalizer;             // Localizer in degrees
    double  GlideLat;               // Glide Slope Location (deg, deg, meters)
    double  GlideLon;
    double  GlideAlt;
    float   fGlideSlopeAngle;       // Glide Slope in degrees
};

// SIMCONNECT_RECV_VOR_LIST
SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_VOR_LIST : public SIMCONNECT_RECV_FACILITIES_LIST
{
    SIMCONNECT_FIXEDTYPE_DATAV(SIMCONNECT_DATA_FACILITY_VOR, rgData, dwArraySize, U1 /*member of UnmanagedType enum*/, SIMCONNECT_DATA_FACILITY_VOR /*cli type*/);
};

#ifdef ENABLE_SIMCONNECT_EXPERIMENTAL

SIMCONNECT_REFSTRUCT SIMCONNECT_RECV_PICK : public SIMCONNECT_RECV // when dwID == SIMCONNECT_RECV_ID_RESERVED_KEY
{
    HANDLE  hContext;
    DWORD   dwFlags;  //
    double  Latitude;   // degrees
    double  Longitude;  // degrees
    double  Altitude;   // feet   
    int     xPos; //reserved
    int     yPos; //reserved;
    DWORD   dwSimObjectID;
    HANDLE  hSceneryObject;
    DWORD   dwentrynumber;      // if multiple objects returned, this is number <entrynumber> out of <outof>.
    DWORD   dwoutof;            // note: starts with 1, not 0.          
};

#endif //ENABLE_SIMCONNECT_EXPERIMENTAL


// SIMCONNECT_DATATYPE_INITPOSITION
SIMCONNECT_STRUCT SIMCONNECT_DATA_INITPOSITION
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
SIMCONNECT_STRUCT SIMCONNECT_DATA_MARKERSTATE
{
    SIMCONNECT_STRING(    szMarkerName, 64);
    DWORD   dwMarkerState;
};

// SIMCONNECT_DATATYPE_WAYPOINT
SIMCONNECT_STRUCT SIMCONNECT_DATA_WAYPOINT
{
    double          Latitude;   // degrees
    double          Longitude;  // degrees
    double          Altitude;   // feet   
    unsigned long   Flags;
    double          ktsSpeed;   // knots
    double          percentThrottle;
};

// SIMCONNECT_DATA_LATLONALT
SIMCONNECT_STRUCT SIMCONNECT_DATA_LATLONALT
{
    double  Latitude;
    double  Longitude;
    double  Altitude;
};

// SIMCONNECT_DATA_XYZ
SIMCONNECT_STRUCT SIMCONNECT_DATA_XYZ
{
    double  x;
    double  y;
    double  z;
};

struct SIMCONNECT_DATA_PBH
{
    double  Pitch;    //!< pitch
    double  Bank;     //!< bank
    double  Heading;  //!< heading
};

#pragma pack(pop)

//----------------------------------------------------------------------------
//        End of Struct definitions
//----------------------------------------------------------------------------

typedef void (CALLBACK *DispatchProc)(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);

#if !defined(SIMCONNECTAPI)
	#ifdef _MSFS_WASM
		#ifdef __INTELLISENSE__
			#define MODULE_EXPORT
			#define SIMCONNECTAPI extern "C" HRESULT
		#else
			#define MODULE_EXPORT __attribute__( ( visibility( "default" ) ) )
			#define SIMCONNECTAPI extern "C" __attribute__((import_module(SIMCONNECT_WASM_MODULE))) HRESULT
		#endif
	#else
		#define MODULE_EXPORT
		#define SIMCONNECTAPI extern "C" HRESULT __stdcall
	#endif
#endif

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
SIMCONNECTAPI SimConnect_Close(HANDLE hSimConnect);
SIMCONNECTAPI SimConnect_RetrieveString(SIMCONNECT_RECV * pData, DWORD cbData, void * pStringV, char ** pszString, DWORD * pcbString);
SIMCONNECTAPI SimConnect_GetLastSentPacketID(HANDLE hSimConnect, DWORD * pdwError);
SIMCONNECTAPI SimConnect_Open(HANDLE * phSimConnect, LPCSTR szName, HWND hWnd, DWORD UserEventWin32, HANDLE hEventHandle, DWORD ConfigIndex);
SIMCONNECTAPI SimConnect_CallDispatch(HANDLE hSimConnect, DispatchProc pfcnDispatch, void * pContext);
SIMCONNECTAPI SimConnect_GetNextDispatch(HANDLE hSimConnect, SIMCONNECT_RECV ** ppData, DWORD * pcbData);
SIMCONNECTAPI SimConnect_RequestResponseTimes(HANDLE hSimConnect, DWORD nCount, float * fElapsedSeconds);
SIMCONNECTAPI SimConnect_InsertString(char * pDest, DWORD cbDest, void ** ppEnd, DWORD * pcbStringV, const char * pSource);
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

#endif // _SIMCONNECT_H_

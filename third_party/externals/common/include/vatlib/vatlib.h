#ifndef VATLIB_H_
#define VATLIB_H_

/*!
 \file vatlib.h
 \author Roland Winklmeier
 \date 2014
 \brief VATSIM library

 vatlib is the library required to interface with typical
 VATSIM servers. It provides methods to communicate via
 network and voice protocols.
*/

#if defined(_WIN32) && !defined(VATLIB_STATIC)
/* You should define VATLIB_EXPORTS *only* when building the DLL. */
#  ifdef VATLIB_EXPORTS
#    define VATLIB_API __declspec(dllexport)
#  else
#    define VATLIB_API __declspec(dllimport)
#  endif
#else
/*! Calling convention to export symbols */
#  define VATLIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>
#include <stdbool.h>

/*!*************************************************************************
    General Section
 ***************************************************************************/

/*!
 Versions 0.1.0 through 1.0.0 are in the range 0 to 100.
 From version 1.0.1 on it's xxyyzz, where x=major, y=minor, z=release.
 Reason:
 Leading zeros changes the number to octal.
*/
#define VAT_LIBVATLIB_VERSION 908

/*!
 Retrieve the release number of the currently running vatlib build.
 \return Version as integer value, e.g. 907
*/
VATLIB_API int Vat_GetVersion();

/*!
 Retrieve a textual description of the current vatlib build.
 \return Version as string, e.g. "v0.9.7"
*/
VATLIB_API const char *Vat_GetVersionText();

/*!
 Get vatlib build information.
 \return Returns a multiline string similar to the following example:
         "Built from revision d9e6750 on Dec 19 2017 17:41:03"
         "Built with GNU-5.3.0 32bit"
         "Built in Release mode"
*/
VATLIB_API const char *Vat_GetBuildInfo();

/*!
 Log severity levels. They are used within the log handlers to specify
 the severity of the log message.

 \sa VatlibLogHandler_t, Vat_SetVoiceLogHandler, Vat_SetVoiceLogSeverityLevel,
 Vat_SetNetworkLogHandler, Vat_SetNetworkLogSeverityLevel
*/
typedef enum
{
    vatSeverityNone,       /*!< Severity None. Nothing is logged. */
    vatSeverityError,      /*!< Severity Error. Only errors are logged. */
    vatSeverityWarning,    /*!< Severity Warning. Warning and higher is logged. */
    vatSeverityInfo,       /*!< Severity Info. Info and higher is is logged. */
    vatSeverityDebug       /*!< Severity Debug. Debug and higher is logged. */
}
VatSeverityLevel;

/*!
 Log handler callback. A function of this signature is called by vatlib when a message is logged.

 \param level The log message severity.
 \param context Description of the executed function in which the log message was generated
 \param message The message logged by vatlib.
 \see VatSeverityLevel
 */
typedef void (* VatlibLogHandler_t)(
    VatSeverityLevel level,
    const char *context,
    const char *message
    );

/*! Connection status */
typedef enum
{
    vatStatusConnecting,    /*!< Connecting to server */
    vatStatusConnected,     /*!< Connection successful (log-in may be in process). */
    vatStatusDisconnecting, /*!< Disconnecting from server. */
    vatStatusDisconnected,  /*!< Disconnected from server. */
}
VatConnectionStatus;

/*!*************************************************************************
    Network Section
 ***************************************************************************/

/*!
 VatFsdClient is an opaque reference to a client session. A fsd client is a single user's connection
 to the protocol. A session may be connected multiple times, but its login information
 can only be specified when disconnected. Each session has callbacks for incoming data
 associated with it; these are good for the life of a session but must be set up
 separately for each session. For this reason, you probably want to keep your session
 around a long time.
*/
#ifdef __cplusplus
class VatFsdClient;
#else
typedef void *VatFsdClient;
#endif

/*!
 A test method to pass valid FSD message lines as if they have been received from the
 FSD servers. The message is fully processed and all callbacks are called.
 \param fsdClient
 \param message The FSD message line
*/
VATLIB_API void Vat_SendFsdMessage(
    VatFsdClient *fsdClient,
    const char *message);

/*! Server type */
typedef enum
{
    vatServerLegacyFsd,   /**< Legacy FSD. */
    vatServerVatsim       /**< VATSIM server. */
}
VatServerType;

/*! Client capability flags */
typedef enum
{
    /*! None. */
    vatCapsNone             = (1 << 0),

    /*! Can accept ATIS responses. */
    vatCapsAtcInfo          = (1 << 1),

    /*! Can send/receive secondary visibility center points (ATC/Server only). */
    vatCapsSecondaryPos     = (1 << 2),

    /*!
     * Can send/receive modern model packets.
     *
     * This should be the standard for any new pilot client. Also all older VATSIM clients
     * starting from SB3 do support this capability.
     * Aircraft info contains
     * \li Aircraft ICAO identifier
     * \li Airline ICAO identifier (optional)
     * \li Airline livery (optional)
     */
    vatCapsAircraftInfo     = (1 << 3),

    /*! Can send/receive inter-facility coordination packets (ATC only). */
    vatCapsOngoingCoord     = (1 << 4),

    /*!
     * Can send/receive Interim position updates (pilot only)
     * \deprecated Used only by Squawkbox with high precision errors. Use
     * vatCapsFastPos instead.
     */
    vatCapsInterminPos      = (1 << 5),

    /*! Can send/receive fast position updates (pilot only). */
    vatCapsFastPos          = (1 << 6),

    /*! Stealth mode */
    vatCapsStealth          = (1 << 7),

    /*! Aircraft Config */
    vatCapsAircraftConfig   = (1 << 8)
}
VatCapabilities;

/****************************************************************
 * Protocol constants
 ****************************************************************/

/*! VATSIM-standard unicom */
#define VAT_FREQUENCY_UNICOM 122.8

/*! Emergency channel */
#define VAT_FREQUENCY_GUARD 121.5

/*! ATC party-line */
#define VAT_FREQUENCY_ATC 149.999


/*! Server error codes */
typedef enum
{
    vatServerErrorNone,             /*!< No error */
    vatServerErrorCsInUs,           /*!< Callsign in use */
    vatServerErrorCallsignInvalid,  /*!< Callsign invalid */
    vatServerErrorRegistered,       /*!< Already registered */
    vatServerErrorSyntax,           /*!< Syntax error */
    vatServerErrorSrcInvalid,       /*!< Invalid source in packet */
    vatServerErrorCidInvalid,       /*!< Invalid CID/password */
    vatServerErrorNoSuchCs,         /*!< No such callsign */
    vatServerErrorNoFP,             /*!< No flightplan */
    vatServerErrorNoWeather,        /*!< No such weather profile */
    vatServerErrorRevision,         /*!< Invalid protocol revision */
    vatServerErrorLevel,            /*!< Requested level too high */
    vatServerErrorServFull,         /*!< No more clients */
    vatServerErrorCsSuspended,      /*!< CID/PID was suspended */
    vatServerErrorInvalidCtrl,      /*!< Not valid control */
    vatServerErrorInvPos,           /*!< Invalid position for rating */
    vatServerErrorUnAuth,           /*!< Not authorized software */
    vatServerWrongType,             /*!< Wrong server type */
    vatErrorUnknown                 /*!< Unknown error */
}
VatServerError;

/*! Client type */
typedef enum
{
    vatClientTypeUnknown,   /*!< Unknown type */
    vatClientTypePilot,     /*!< Pilot client type */
    vatClientTypeAtc        /*!< ATC client type */
}
VatClientType;

/*! Transponder modes */
typedef enum
{
    vatTransponderModeStandby,  /*!< Transponder is off, or in standby. */
    vatTransponderModeCharlie,  /*!< Transponder is on mode C, not identing. */
    vatTransponderModeIdent     /*!< Transponder is on mode C and identing. */
}
VatTransponderMode;

/*! ATC ratings */
typedef enum
{
    vatAtcRatingUnknown,        /*!< Unknown */
    vatAtcRatingObserver,       /*!< OBS */
    vatAtcRatingStudent,        /*!< S1 */
    vatAtcRatingStudent2,       /*!< S2 */
    vatAtcRatingStudent3,       /*!< S3 */
    vatAtcRatingController1,    /*!< C1 */
    vatAtcRatingController2,    /*!< C2 */
    vatAtcRatingController3,    /*!< C3 */
    vatAtcRatingInstructor1,    /*!< I1 */
    vatAtcRatingInstructor2,    /*!< I2 */
    vatAtcRatingInstructor3,    /*!< I3 */
    vatAtcRatingSupervisor,     /*!< SUP */
    vatAtcRatingAdministrator   /*!< ADM */
}
VatAtcRating;

/*! Pilot ratings */
typedef enum
{
    vatPilotRatingUnknown,      /*!< Unknown rating */
    vatPilotRatingStudent,      /*!< P1 */
    vatPilotRatingVFR,          /*!< P2 */
    vatPilotRatingIFR,          /*!< P3 */
    vatPilotRatingInstructor,   /*!< Instructor */
    vatPilotRatingSupervisor    /*!< SUP */
}
VatPilotRating;

/*! ATC facility type */
typedef enum
{
    vatFacilityTypeUnknown, /*!< Unknown facility type */
    vatFacilityTypeFSS,     /*!< FSS */
    vatFacilityTypeDEL,     /*!< Delivery */
    vatFacilityTypeGND,     /*!< Ground */
    vatFacilityTypeTWR,     /*!< Tower */
    vatFacilityTypeAPP,     /*!< Approach */
    vatFacilityTypeCTR      /*!< Center */
}
VatFacilityType;

/*! Flight simulator type */
typedef enum
{
    vatSimTypeUnknown,      /*!< Unknown simulator type */
    vatSimTypeMSFS95,       /*!< MS Flight Simulator 95 */
    vatSimTypeMSFS98,       /*!< MS Flight Simulator 98 */
    vatSimTypeMSCFS,        /*!< MS Combat Flight Simulator */
    vatSimTypeMSFS2000,     /*!< MS Flight Simulator 2000 */
    vatSimTypeMSCFS2,       /*!< MS Combat Flight Simulator 2 */
    vatSimTypeMSFS2002,     /*!< MS Flight Simulator 2002 */
    vatSimTypeMSCFS3,       /*!< MS Combat Flight Simulator 3 */
    vatSimTypeMSFS2004,     /*!< MS Flight Simulator 2004 */
    vatSimTypeMSFSX,        /*!< MS Flight Simulator X */
    vatSimTypeXPLANE8,      /*!< X-Plane 8 */
    vatSimTypeXPLANE9,      /*!< X-Plane 9 */
    vatSimTypeXPLANE10,     /*!< X-Plane 10 */
    vatSimTypeXPLANE11,     /*!< X-Plane 11 */
    vatSimTypeP3Dv1,        /*!< Prepar3D V1 */
    vatSimTypeP3Dv2,        /*!< Prepar3D V2 */
    vatSimTypeP3Dv3,        /*!< Prepar3D V3 */
    vatSimTypeP3Dv4,        /*!< Prepar3D V4 */
    vatSimTypeFlightGear    /*!< Flight Gear */
}
VatSimType;

/*! Flight rules */
typedef enum
{
    vatFlightTypeIFR,   /*!< IFR flight rules. */
    vatFlightTypeVFR,   /*!< Visual flight rules. */
    vatFlightTypeSVFR,  /*!< Special visual flight rules. */
    vatFlightTypeDVFR   /*!< Defense visual Flight Rules. */
}
VatFlightType;

/*! Client query types */
typedef enum
{
    vatClientQueryFP,     /*!< Flight plan (pilots only, reply is a flight plan message. */
    vatClientQueryFreq,   /*!< Frequency (pilots only). */
    vatClientQueryInfo,   /*!< User Info (must be supervisor). */
    vatClientQueryAtis,   /*!< ATIS (controllers only, reply is a text message. */
    vatClientQueryServer, /*!< What server is this client on. */
    vatClientQueryName,   /*!< Real name. */
    vatClientQueryAtc,    /*!< Is this client working ATC or just an observer (ATC only). */
    vatClientQueryCaps,   /*!< What capabilities does this client have. */
    vatClientQueryIP,     /*!< What IP address am I sending from? */
}
VatClientQueryType;

/*! Aircraft engine type */
typedef enum
{
    vatEngineTypePiston,    /*!< Piston */
    vatEngineTypeJet,       /*!< Jet */
    vatEngineTypeNone,      /*!< None */
    vatEngineTypeHelo       /*!< Helo */
}
VatEngineType;

/*! Operations for a land line */
typedef enum
{
    vatLandlineCmdRequest,  /*!< Request landline with other client. */
    vatLandlineCmdApprove,  /*!< Approve a request for a landline connection. */
    vatLandlineCmdReject,   /*!< Reject a request for a landline. */
    vatLandlineCmdEnd       /*!< Terminate an in-progress landline. */
}
VatLandlineCmd;

/*! Types of landlines */
typedef enum
{
    vatLandlineTypeIntercom,    /*!< Intercom, a basic two-way telephone call. */
    vatLandlineTypeOverride,    /*!< Override. Receiver doesn't have to key mic. */
    vatLandlineTypeMonitor      /*!< Monitor - one way send back to the initiator. */
}
VatLandlineType;

/*! Tracking commands */
typedef enum
{
    vatTrackingCmdStartTrack,   /*!< Start tracking */
    vatTrackingCmdDropTrack,    /*!< Drop tracking */
    vatTrackingCmdIHave,        /*!< I'm tracking */
    vatTrackingCmdWhoHas,       /*!< Who is tracking */
    vatTrackingCmdPointout,     /*!< Point out a target on the radar screen */
    vatTrackingCmdDepartureList /*!< Add target to departure list */
}
VatTrackingCmd;

/*! Pilot position structure */
typedef struct
{
    double latitude;        /*!< Latitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    double longitude;       /*!< Longitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    int altitudeTrue;       /*!< True altitude in feet above MSL. */
    int altitudePressure;   /*!< Pressure altitude in feet above MSL. */
    int groundSpeed;        /*!< Ground speed in knots. */
    double heading;         /*!< Heading in degrees, clockwise from true north, 0-359. */
    double bank;            /*!< Bank in degrees, positive = roll right. */
    double pitch;           /*!< Pitch in degrees, positive = pitch up. */
    bool onGround;          /*!< True if aircraft is on ground, false otherwise. */
    int transponderCode;    /*!< Transponder code. Valid values are between 0000-7777. */
    VatTransponderMode transponderMode; /*!< Current transponder mode. Either standby, charlie or ident. */
    VatPilotRating rating;  /*!< Pilot rating. This is not yet used in VATSIM.
                                 VATSIM expects every pilot client to send vatPilotRatingStudent only. */
}
VatPilotPosition;

/*! Interim pilot position structure */
typedef struct
{
    double latitude;    /*!< Latitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    double longitude;   /*!< Longitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    int altitudeTrue;   /*!< True altitude in feet above MSL. */
    int groundSpeed;    /*!< Ground speed in knots. */
    double heading;     /*!< Heading in degrees, clockwise from true north, 0-359. */
    double bank;        /*!< Bank in degrees, positive = roll right. */
    double pitch;       /*!< Pitch in degrees, positive = pitch up. */
    bool onGround;      /*!< True if aircraft is on ground, false otherwise. */
}
VatInterimPilotPosition;

/*! ATC position structure */
typedef struct
{
    int frequency;              /*!< ATC frequency in khz, e.g. 128200 */
    VatFacilityType facility;   /*!< Facility type */
    int visibleRange;           /*!< Visible range in nm */
    VatAtcRating rating;        /*!< ATC rating */
    double latitude;            /*!< Latitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    double longitude;           /*!< Longitude in decimal degrees. Precision shall be minimum 5 fractional digits. */
    int elevation;              /*!< Elevation AGL in feet. */
}
VatAtcPosition;

/*! Pilot Connection information */
typedef struct
{
    const char *callsign;       /*!< Pilots callsign. */
    const char *name;           /*!< Pilots real name. */
    VatSimType simType;         /*!< Simulator type. */
    VatPilotRating rating;      /*!< Pilot rating. This is not yet used in VATSIM. VATSIM expects every pilot
                                     client to send vatPilotRatingStudent only. This must also be consistent with what is
                                     sent in \sa VatPilotPosition. */
}
VatPilotConnection;

/*! ATC Connection information */
typedef struct
{
    const char *callsign;   /*!< Controllers callsign */
    const char *name;       /*!< Controllers real name */
    VatAtcRating rating;    /*!< Controller rating. This must also be consistent with what is
                                 sent in \sa VatAtcPosition. */
}
VatAtcConnection;

/*! A single temperature layer */
typedef struct
{
    int ceiling;    /*!< Top of temperature layer in feet MSL */
    int temp;       /*!< Temperature in degrees celsius */
}
VatTempLayer;

/*! A single wind layer */
typedef struct
{
    int ceiling;    /*!< Wind ceiling in ft MSL. */
    int floor;      /*!< Wind floor in ft MSL. */
    int direction;  /*!< Wind direction in degrees clockwise from true north. */
    int speed;      /*!< Wind speed in knots. */
    int gusting;    /*!< 1 = true, 0 = false. */
    int turbulence; /*!< 0 - 255, 0 = none, 255 = extreme. */
}
VatWindLayer;

/*! A single cloud layer */
typedef struct
{
    int ceiling;    /*!< Cloud ceiling in ft MSL */
    int floor;      /*!< Cloud floor in ft MSL. */
    int coverage;   /*!< Cloud coverage. 0 - 8 octets, 0 = clear, 8 = overcast. */
    int icing;      /*!< Icing 1 = true, 0 = false. */
    int turbulence; /*!< Turbulence 0 - 255, 0 = none, 255 = extreme. */
}
VatCloudLayer;

/*! A thunderstorm layer */
typedef struct
{
    int ceiling;    /*!< Thunderstorm ceiling in ft MSL */
    int floor;      /*!< Thunderstorm floor in ft MSL. */
    int coverage;   /*!< Thunderstorm coverage. 0 - 8 octets, 0 = clear, 8 = overcast. */
    int deviation;  /*!< Icing 1 = true, 0 = false. */
    int turbulence; /*!< Turbulence 0 - 255, 0 = none, 255 = extreme. */
}
VatThunderStormLayer;

/*!
 A flight plan structure sent or received from the server.
 \note All pointers must be valid and cannot be NULL. Pass empty strings for empty input.
*/
typedef struct
{
    VatFlightType flightType;       /*!< Flight type, IFR, VFR, etc. */
    const char *aircraftType;       /*!< Aircraft ICAO type e.g. T/B734/F. */
    int trueCruisingSpeed;          /*!< True cruising speed in knots. */
    const char *departAirport;      /*!< ICAO code of departure airport. */
    int departTime;                 /*!< Departure time in UTC, e.g. 2030 = 8:30 pm GMT. */
    int departTimeActual;           /*!< Actual departure time in UTC, e.g. 2030 = 8:30 pm GMT. */
    const char *cruiseAltitude;     /*!< Planned cruising altitude. FL230 or 15000. */
    const char *destAirport;        /*!< ICAO code of destination airport. */
    int enrouteHrs;                 /*!< Enroute time - hours. */
    int enrouteMins;                /*!< Enroute time - minutes. */
    int fuelHrs;                    /*!< Available fuel - hours. */
    int fuelMins;                   /*!< Available fuel - minutes. */
    const char *alternateAirport;   /*!< ICAO code of alternate airport. */
    const char *remarks;            /*!< Flight plan remarks. */
    const char *route;              /*!< Flight route in capital letters. Separate with dots. */
}
VatFlightPlan;

/*!
 Controller Atis structure
 \note All pointers must be valid and cannot be NULL. Pass empty strings for empty input.
*/
typedef struct
{
    const char *voiceRoom;      /*!< Controller voice room in the form url/room. */
    const char **textLines;     /*!< Controller messages lines. Maximum 4. */
    unsigned int textLineCount; /*!< Number of message lines. */
    const char *zuluLogoff;     /*!< Controllers planned logoff time, e.g 20:00 UTC. */
}
VatControllerAtis;

/*!
 Aircraft information structure
 \note All pointers must be valid and cannot be NULL. Pass empty strings for empty input.
*/
typedef struct
{
    const char *aircraftType;   /*!< Aircraft ICAO identifier. */
    const char *airline;        /*!< Airline callsign. */
    const char *livery;         /*!< Livery identifier. */
}
VatAircraftInfo;

/*!
 Vatlib uses installed callback functions as parameters for some function calls,
 e.g. to set up notification handlers. The typedefs defined in this group describe the function parameters used to set
 up or clear the callback functions and should also be referenced to define the
 callback function to handle such events in the user's code.

 Each callback function has always two mandatory parameters. VatFsdClient is the pointer to the client
 which called the callback. This can be used to identify the session object.
 The second mandatory parameter ref lets you any user defined data. As in C++ all callbacks have to be static
 functions, user defined data is typically set to the C++ object pointer. This allows you to forward the call
 from the static method to the listening C++ class.
*/

/****************************************************************
 * Callbacks for all clients
 ****************************************************************/

/*!
 \defgroup all_client_callbacks Callbacks for all clients
 \{
*/

/*!
 Connection state change callback.
 \param fsdClient Client connection handler
 \param oldStatus Old connection status
 \param newStatus New connection status
 \param ref User defined data
*/
typedef void (* VatStateChangeHandler_f)(
    VatFsdClient *fsdClient,
    VatConnectionStatus oldStatus,
    VatConnectionStatus newStatus,
    void *ref);

/*!
 Server error callback.
 Vatlib calls this callback when the server reported an error. This can be due to
 wrong login details, a malformed packet etc.
 \param fsdClient Client connection handler.
 \param errorCode Exact code identifying the error.
 \param parameter String identifying the exact parameter which caused the error.
 \param description Some additional and helpful description what went wrong.
 \param ref User defined data.
 \see VatServerError
*/
typedef void (* VatServerErrorHandler_f)(
    VatFsdClient *fsdClient,
    VatServerError errorCode,
    const char *parameter,
    const char *description,
    void *ref);

/*!
 Text message callback.
 Vatlib calls this callback when a private text message has been received.
 \param fsdClient Session pointer
 \param sender Callsign of the message sender
 \param receiver Callsign of the message receiver
 \param ref User defined data
*/
typedef void (* VatTextMessageHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *receiver,
    const char *message,
    void *ref);

/*!
 Radio message callback.
 Vatlib calls this callback when a radio text message has been received. Vatlib does not
 know which frequency the user is listening to therefore this callback is called for every
 radio message. It is the responsibility of the client to filter relevant messages.
 Each radio message can be transmitted on more then one frequency.
 \param fsdClient Session pointer
 \param sender Callsign of the message sender
 \param freqCount The number of frequencies on which the text message was transmitted
 \param freqList Array of frequencies on which the text message was transmitted
 \param receiver Callsign of the message receiver
 \param ref User defined data
*/
typedef void (* VatRadioMessageHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    unsigned int freqCount,
    int *freqList,
    const char *message,
    void *ref);

/*!
 Delete Pilot callback.
 Vatlib calls this callback when a pilot has left the network.
 \param fsdClient Client connection handler.
 \param sender Callsign of the pilot leaving the network.
 \param ref User defined data.
*/
typedef void (* VatDeletePilotHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    void *ref);

/*!
 Delete ATC station callback.
 Vatlib calls this callback when an ATC station has left the network.
 \param fsdClient Client connection handler.
 \param sender Callsign of the ATC station leaving the network.
 \param ref User defined data.
*/
typedef void (* VatDeleteAtcHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    void *ref);

/*!
 Pilot position callback.
 Vatlib calls this callback when a pilot's position has been reported.
 \param fsdClient Client connection handler.
 \param sender Callsign of the pilot reporting its position.
 \param position Pilot position struct
 \param ref User defined data.
*/
typedef void (* VatPilotPositionHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatPilotPosition *position,
    void *ref);

/*!
 Interim pilot position callback.
 Vatlib calls this callback when a pilot's interim position has been reported.
 \param fsdClient Client connection handler.
 \param sender Callsign of the pilot reporting its position.
 \param position Interim pilot position struct
 \param ref User defined data.
 \see VatInterimPilotPosition
*/
typedef void (* VatInterimPilotPositionHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatInterimPilotPosition *position,
    void *ref);

/*!
 ATC position callback.
 Vatlib calls this callback when an ATC position has been reported.
 \param fsdClient Client connection handler.
 \param sender Callsign of the ATC station reporting its position.
 \param position ATC position struct
 \param ref User defined data.
*/
typedef void (* VatAtcPositionHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatAtcPosition *position,
    void *ref);

/*!
 Kill callback.
 Vatlib calls this callback to inform you have been kicked from the network. There
 is no further action requred. Both - vatlib and server - will close the connection
 immediatly.
 \param fsdClient Client connection handler.
 \param reason Message from the supervisor, why you have been kicked. This can also be empty.
 \param ref User defined data.
*/
typedef void (* VatKillHandler_f)(
    VatFsdClient *fsdClient,
    const char *reason,
    void *ref);

/*!
 Pong callback.
 Vatlib calls this callback when your ping has been answered with a pong.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param elapsedTime Elapsed time, after ping had been sent in milliseconds.
 \param ref User defined data.
*/
typedef void (* VatPongHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    double elapsedTime,
    void *ref);

/*!
 Flightplan callback.
 Vatlib calls this callback when a flight plan was received from the server.
 \param fsdClient Client connection handler.
 \param sender Callsign of the pilot for which the flightplan is reported
 \param flightPlan Received flight plan
 \param ref User defined data.
*/
typedef void (* VatFlightPlanHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatFlightPlan *flightPlan,
    void *ref);

/*!
 Metar response callback.
 Vatlib calls this callback when metar data has been received from the server as previously
 requested. It does not tell you which ICAO station the metar belongs to, so you better don't
 send several requests in parallel, but better wait until one request is answered.
 \param fsdClient Client connection handler.
 \param metar METAR string
 \param ref User defined data.
 \see Vat_RequestMetar
*/
typedef void (* VatMetarResponseHandler_f)(
    VatFsdClient *fsdClient,
    const char *metar,
    void *ref);

/*!
 Client query callback.
 Vatlib calls this callback when another client is requesting some information from you.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param type Type of information the remote client is asking for.
 \param ref User defined data.
*/
typedef void (* VatClientQueryHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    VatClientQueryType type,
    const char *data,
    void *ref);

/*!
 Client query response callback.
 Vatlib calls this callback when the info request has been answered by another client to you.
 Function signature when an info reponse is received. For most info responses, only data is passed
 back, but for an ATC query about another ATC, data2 is also used.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param type Type of information the remote client is asking for.
 \param data Reponse data. The content depends heavily on the request type.
 \param data2 Some responses use a second data field.
 \todo Complete the descriptions for the different sub types
 \param ref User defined data.
*/
typedef void (* VatClientQueryResponseHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    VatClientQueryType type,
    const char *data,
    const char *data2,
    void *ref);

/*!
 Client capabilities callback
 Vatlib calls this callback when another client has sent his client capabilities.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param capabilityFlags A flag containing each supported capability.
 \param ref User defined data.
 \see VatCapabilities
*/
typedef void (* VatClientCapabilitiesReplyHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    int capabilityFlags,
    void *ref);

/*!
 Voice room callback.
 Vatlib calls this callback when an ATC client has sent his voice room.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param voiceRoom Voiceroom string in the form of "<host>/room", e.g. "voice.somewhere.com/zzzz_twr".
 \param ref User defined data.
*/
typedef void (* VatVoiceRoomHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *voiceRoom,
    void *ref);

/*!
 Controller ATIS callback.
 Vatlib calls this callback when an ATC client has sent the controller ATIS. Be aware this
 is not the ATIS of an airport, but some detailed information about the controller itself
 (e.g. until when he is going to be online, voiceroom) and some information he wants to pass
 onto pilots (e.g. links to charts or initial calls advices).
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param atis The ATC controllers atis.
 \param ref User defined data.
 \see VatControllerAtis
*/
typedef void (* VatControllerAtisHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatControllerAtis *atis,
    void *ref);

/*!
 Temperature data callback
 Vatlib calls this callback when temperature layers for an ICAO station have been received
 as previously requested. Be aware that this callback does not say which ICAO station the
 data belongs to. So you better do not run multiple requests in parallel.
 \param fsdClient Client connection handler.
 \param tempLayers Array of 4 temperature layers
 \param pressure The barometric pressure in the form 2992.
 \param ref User defined data.
 \see VatTempLayer
*/
typedef void (* VatTemperatureDataHandler_f)(
    VatFsdClient *fsdClient,
    const VatTempLayer tempLayers[4],
    int pressure,
    void *ref);

/*!
 Wind data callback.
 Vatlib calls this callback when wind layers for an ICAO station have been received
 as previously requested. Be aware that this callback does not say which ICAO station the
 data belongs to. So you better do not run multiple requests for different stations in parallel.
 \param fsdClient Client connection handler.
 \param windLayers Array of 4 wind layers
 \param pressure The barometric pressure in the form 2992.
 \param ref User defined data.
 \see VatWindLayer
*/
typedef void (* VatWindDataHandler_f)(
    VatFsdClient *fsdClient,
    const VatWindLayer windLayers[4],
    void *ref);

/*!
 Cloud data callback.
 Vatlib calls this callback when cloud layers for an ICAO station have been received
 as previously requested. Be aware that this callback does not say which ICAO station the
 data belongs to. So you better do not run multiple requests for different stations in parallel.
 \param fsdClient Client connection handler.
 \param cloudLayers Array of 2 cloud layers
 \param thunderStormLayer One extra layer in case of thunderstorms. If no thunderstorm is around,
 this layer will have invalid values.

 \param visibility Visibility in statute miles.
 \param ref User defined data.
 \see VatCloudLayer, VatThunderStormLayer
*/
typedef void (* VatCloudDataHandler_f)(
    VatFsdClient *fsdClient,
    const VatCloudLayer cloudLayers[2],
    VatThunderStormLayer thunderStormLayer,
    float visibility,
    void *ref);

/*!
 Aircraft configuration callback.
 Vatlib calls this callback when aircraft configuration has been received from another pilot
 client. Aircraft configuration is a JSON encoded object and does contain among other things:
 \li aircraft lights (on/off)
 \li aircraft engines (on/off)
 \li gear down/up
 \li aircraft on ground (true/false)
 \li flaps between 0 - 100 %.
 \li ...

 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param aircraftConfig JSON encoded aircraft config object.
 \param ref User defined data.
*/
typedef void (* VatAircraftConfigHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *aircraftConfig,
    void *ref);

/*! \} */ /* group all_client_callbacks */

/*!*************************************************************************
    Callbacks for pilot clients
 ***************************************************************************/

/*!
 \defgroup pilot_client_callbacks Callbacks for pilot clients
 \{
*/

/*!
 Aircraft info request callback.
 Vatlib calls this callback when a aircraft info request is received. A aircraft
 info request needs to be responded by sending the modern plane info.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param ref User defined data.
 \see Vat_SendAircraftInfo
*/
typedef void (* VatAircraftInfoRequestHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    void *ref);

/*!
 Aircraft info response callback.
 Vatlib calls this callback when a aircraft info response is received. A aircraft
 info reponse is the answer to an aircraft info request. It provides you in the best case
 information about the other clients aircraft type, airline and livery. Other clients
 might have disabled to send this information, so it could be that you receive only
 some.
 This is the modern version. Older clients used to send a different packet as reponse.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param aircraftInfo Aircraft info struct.
 \param ref User defined data.
 \see VatAircraftInfo, VatLegacyAircraftInfoHandler_f
*/
typedef void (* VatAircraftInfoResponseHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const VatAircraftInfo *aircraftInfo,
    void *ref);

/*!
 Legacy aircraft info callback.
 Vatlib calls this callback when a aircraft info response is received. A aircraft
 info reponse is the answer to an aircraft info request. It provides you information
 about the engine type and the used CSL model.
 This is the legacy version of the reponse. Modern clients use to send a different packet as reponse.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param aircraftInfo Aircraft info struct.
 \param ref User defined data.
 \see VatAircraftInfoResponseHandler_f
*/
typedef void (* VatLegacyAircraftInfoHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    VatEngineType engineType,
    const char *cslModel,
    void *ref);

/*!
 Custom pilot packet callback.
 Vatlib calls this callback when a custom pilot client communication packet
 has been received, which was unknown to vatlib. It therefore forwards the packet type
 plus all tokens to the client. This allows clients to implement custom packages and
 handle them themselves.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param subType String identifying the type of custom packet.
 \param data Array of const char pointers. Each pointer points to a packet token string.
 \param dataSize The size of the const char* array.
 \param ref User defined data.
*/
typedef void (* VatCustomPilotPacketHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *subType,
    const char **data,
    int dataSize,
    void *ref);

/*!
 Functions of type VatFsdMessageHandler_f can be implemented by any client.
 \param fsdClient Client connection handler.
 \param message Raw FSD message
 \param ref User defined data.
*/
typedef void (* VatFsdMessageHandler_f)(
    VatFsdClient *fsdClient,
    const char *message,
    void *ref);

/*! \} */ /* group pilot_client_callbacks */

/****************************************************************
 * Callbacks for ATC clients
 ****************************************************************/

/*!
 \defgroup atc_client_callbacks Callbacks for ATC clients
 \{
*/

/*!
 Handoff request callback.
 Vatlib calls this callback when another ATC client is requesting to handoff a target
 to you. You can either accept the handover or reject it.
 \param fsdClient Client connection handler.
 \param target Targets callsign
 \param ref User defined data.
 \see Vat_AcceptHandoff, Vat_CancelHandoff
*/
typedef void (* VatHandoffRequestHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 Handoff accepted callback.
 Vatlib calls this callback when another ATC client has accepted the handoff of target
 from you.
 \param fsdClient Client connection handler.
 \param target Targets callsign
 \param ref User defined data.
*/
typedef void (* VatHandoffAcceptedHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 Handoff cancelled callback.
 Vatlib calls this callback when another ATC client has cancelled the handoff of target
 from you. You are still tracking it.
 \param fsdClient Client connection handler.
 \param target Targets callsign
 \param ref User defined data.
*/
typedef void (* VatHandoffCancelledHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 Metar request callback.
 Vatlib calls this callback when metar is requested from you for a ICAO station.
 This is never going to happen with VATSIM servers, since you always request METAR
 from the servers and not the other way round. But just in case someone needs it.
 \param fsdClient Client connection handler.
 \param station Targets callsign
 \param ref User defined data.
 \see Vat_RequestMetar
*/
typedef void (* VatMetarRequestHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *station,
    void *ref);

/*!
 Custom ATC packet callback.
 Vatlib calls this callback when a custom ATC client communication packet
 has been received, which was unknown to vatlib. It therefore forwards the packet type
 plus all tokens to the client. This allows clients to implement custom packages and
 handle them themselves.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client
 \param subType String identifying the type of custom packet.
 \param data Array of const char pointers. Each pointer points to a packet token string.
 \param dataSize The size of the const char* array.
 \param ref User defined data.
*/
typedef void (* VatCustomAtcPacketHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *subType,
    const char **data,
    int dataSize,
    void *ref);

/*!
 Targets Scratchpad callback.
 Use this callback to update your local scratchpad.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param target Callsign of the target.
 \param scratchpad New content of the scratchpad
 \param ref User defined data.
 \see VatAircraftInfoResponseHandler_f
*/
typedef void (* VatTargetsScratchpadHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    const char *scratchpad,
    void *ref);

/*!
 Controller break command callback.
 Vatlib calls this callback when an ATC controller requested a break or stopped the break.
 If break mode is active, the ATC client shall display the ATC controller different
 and no handover shall be initiated.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param wantsBreak True if the controller requested a break and false if the break is over
 \param ref User defined data.
*/
typedef void (* VatControllerBreakCommandHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    bool wantsBreak,
    void *ref);

/*!
 Push to departure list callback.
 Vatlib calls this callback when an ATC controller wants to push a target into our
 depature list. The receiving client shall add the target into the departure list,
 if it is not already there.
 \todo Callback Setter and send method
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param target Callsign of the target.
 \param ref User defined data.
*/
typedef void (* VatPushToDepartureListHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 Point out callback.
 Vatlib calls this callback when an ATC controller wants to point out a target onto our
 radar screen. The receiving client shall highlight target on the radar screen.
 \todo Callback Setter and send method
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param target Callsign of the target.
 \param ref User defined data.
*/
typedef void (* VatPointOutHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 I have target callback.
 Vatlib calls this callback when an ATC controller answered that he is currently
 tracking the target. This is typically the reponse to the request who is tracking
 the target.
 \todo Callback Setter and send method
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param target Callsign of the target.
 \param ref User defined data.
*/
typedef void (* VatIHaveTargetHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    void *ref);

/*!
 Push flight strip callback.
 Vatlib calls this callback when an ATC controller wants to push a flight strip
 into our flight strip bay. Receiving ATC clients shall add the flight strip as
 requested and with all the information received from the packet.
 \todo Callback Setter and send method
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param target Callsign of the target.
 \param type Type of the flight strip
 \param annotations Array of strings with flight strip annotations
 \param ref User defined data.
*/
typedef void (* VatPushFlightStripHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *target,
    int type,
    const char **annotations,
    void *ref);

/*!
 Help command callback.
 Vatlib calls this callback when an ATC controller requested help or stopped it.
 If help mode is active, the ATC client shall display the ATC controller in a different
 way.
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param wantsHelp True if the controller requested help and false if the request is over.
 \param message Additional details about the help request.
 \param ref User defined data.
*/
typedef void (* VatHelpCommandHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    bool wantsHelp,
    const char *message,
    void *ref);

/*! \} */ /* group atc_client_callbacks */

/****************************************************************
 * Callbacks for ??? Clients
 ****************************************************************/

/*!
 IPC callback.
 \todo complete the description
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param ipcCommand
 \param ipcSlot
 \param ipcValue
 \param ref User defined data.
*/
typedef void (* VatIPCHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    const char *ipcCommand,
    int ipcSlot,
    int ipcValue,
    void *ref);

/*!
 Functions of type VatHandleLandlineCommandHandler_f are implemented by ??? clients.
 \todo complete the description
 \param fsdClient Client connection handler.
 \param sender Callsign of the sending client.
 \param command
 \param type
 \param ip
 \param port
 \param ref User defined data.
*/
typedef void (* VatLandlineCommandHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    VatLandlineCmd command,
    VatLandlineType type,
    const char *ip,
    int port,
    void *ref);

/*!
    Function signature when a nex-gen client is ID'ing us
 */
typedef void (* VatSharedStateIDHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    void *ref);

/*!
    Function signature when a nex-gen client is replying to our ID
 */
typedef void (* VatSharedStateDIHandler_f)(
    VatFsdClient *fsdClient,
    const char *sender,
    void *ref);

/****************************************************************
 * Network Callback Installers
 ****************************************************************/

/*!
 These functions allow you to associate a callback with a session. Install
 a NULL handler to delete a callback. You pass in a void "reference ptr"
 that is returned to you in your callback (in the ref field). This lets
 you find local variables attached to this particular instance of a session.
*/

/*!
 Installs the NetworkLogHandler callback. Whenever vatlib is logging something
 from the network side, this callback will be called.
 It does not have any user defined field yet.
 \param maximumSeverityLevel The maximal severity you want to receive.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatlibLogHandler_t
*/
VATLIB_API void Vat_SetNetworkLogHandler(
    VatSeverityLevel maximumSeverityLevel,
    VatlibLogHandler_t handler);

/*!
 Sets the maximal severity to a new value.
 \param maximumSeverityLevel Maximum severity level
*/
VATLIB_API void Vat_SetNetworkLogSeverityLevel(
    VatSeverityLevel maximumSeverityLevel);

/*!
 Installs the StateChangeHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatStateChangeHandler_f
*/
VATLIB_API void Vat_SetStateChangeHandler(
    VatFsdClient *fsdClient,
    VatStateChangeHandler_f handler,
    void *ref);

/*!
 Installs the TextMessageHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatTextMessageHandler_f
*/
VATLIB_API void Vat_SetTextMessageHandler(
    VatFsdClient *fsdClient,
    VatTextMessageHandler_f handler,
    void *ref);

/*!
 Installs the RadioMessageHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatRadioMessageHandler_f
*/
VATLIB_API void Vat_SetRadioMessageHandler(
    VatFsdClient *fsdClient,
    VatRadioMessageHandler_f handler,
    void *ref);

/*!
 Installs the PilotPositionHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatPilotPositionHandler_f
*/
VATLIB_API void Vat_SetPilotPositionHandler(
    VatFsdClient *fsdClient,
    VatPilotPositionHandler_f handler,
    void *ref);

/*!
 Installs the SetInterimPilotPositionHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatInterimPilotPositionHandler_f
*/
VATLIB_API void Vat_SetInterimPilotPositionHandler(
    VatFsdClient *fsdClient,
    VatInterimPilotPositionHandler_f handler,
    void *ref);

/*!
 Installs the AtcPositionHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatAtcPositionHandler_f
*/
VATLIB_API void Vat_SetAtcPositionHandler(
    VatFsdClient *fsdClient,
    VatAtcPositionHandler_f handler,
    void *ref);

/*!
 Installs the DeletePilotHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatDeletePilotHandler_f
*/
VATLIB_API void Vat_SetDeletePilotHandler(
    VatFsdClient *fsdClient,
    VatDeletePilotHandler_f handler,
    void *ref);

/*!
 Installs the DeleteAtcHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatDeleteAtcHandler_f
*/
VATLIB_API void Vat_SetDeleteAtcHandler(
    VatFsdClient *fsdClient,
    VatDeleteAtcHandler_f handler,
    void *ref);

/*!
 Installs the KillHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatKillHandler_f
*/
VATLIB_API void Vat_SetKillHandler(
    VatFsdClient *fsdClient,
    VatKillHandler_f handler,
    void *ref);

/*!
 Installs the PongHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatPongHandler_f
*/
VATLIB_API void Vat_SetPongHandler(
    VatFsdClient *fsdClient,
    VatPongHandler_f handler,
    void *ref);

/*!
 Installs the FlightPlanHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatFlightPlanHandler_f
*/
VATLIB_API void Vat_SetFlightPlanHandler(
    VatFsdClient *fsdClient,
    VatFlightPlanHandler_f handler,
    void *ref);

/*!
 Installs the HandoffRequestHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatHandoffRequestHandler_f
*/
VATLIB_API void Vat_SetHandoffRequestHandler(
    VatFsdClient *fsdClient,
    VatHandoffRequestHandler_f handler,
    void *ref);

/*!
 Installs the HandoffAcceptedHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatHandoffAcceptedHandler_f
*/
VATLIB_API void Vat_SetHandoffAcceptedHandler(
    VatFsdClient *fsdClient,
    VatHandoffAcceptedHandler_f handler,
    void *ref);

/*!
 Installs the HandoffCancelledHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatHandoffCancelledHandler_f
*/
VATLIB_API void Vat_SetHandoffCancelledHandler(
    VatFsdClient *fsdClient,
    VatHandoffCancelledHandler_f handler,
    void *ref);

/*!
 Installs the MetarRequestHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatMetarRequestHandler_f
*/
VATLIB_API void Vat_SetMetarRequestHandler(
    VatFsdClient *fsdClient,
    VatMetarRequestHandler_f handler,
    void *ref);

/*!
 Installs the MetarResponseHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatMetarResponseHandler_f
*/
VATLIB_API void Vat_SetMetarResponseHandler(
    VatFsdClient *fsdClient,
    VatMetarResponseHandler_f handler,
    void *ref);

/*!
 Installs the InfoRequestHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatInfoRequestHandler_f
*/
VATLIB_API void Vat_SetClientQueryHandler(
    VatFsdClient *fsdClient,
    VatClientQueryHandler_f handler,
    void *ref);

/*!
 Installs the ClientQueryResponseHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatInfoResponseHandler_f
*/
VATLIB_API void Vat_SetClientQueryResponseHandler(
    VatFsdClient *fsdClient,
    VatClientQueryResponseHandler_f handler,
    void *ref);

/*!
 Installs the lientCapabilitiesReply callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatClientCapabilitiesReplyHandler_f
*/
VATLIB_API void Vat_SetClientCapabilitiesReplyHandler(
    VatFsdClient *fsdClient,
    VatClientCapabilitiesReplyHandler_f handler,
    void *ref);

/*!
 Installs the VoiceRoomHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatVoiceRoomHandler_f
*/
VATLIB_API void Vat_SetVoiceRoomHandler(
    VatFsdClient *fsdClient,
    VatVoiceRoomHandler_f handler,
    void *ref);

/*!
 Installs the ControllerAtisHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatControllerAtisHandler_f
*/
VATLIB_API void Vat_SetControllerAtisHandler(
    VatFsdClient *fsdClient,
    VatControllerAtisHandler_f handler,
    void *ref);

/*!
 Installs the TemperatureDataHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatTemperatureDataHandler_f
*/
VATLIB_API void Vat_SetTemperatureDataHandler(
    VatFsdClient *fsdClient,
    VatTemperatureDataHandler_f handler,
    void *ref);

/*!
 Installs the WindDataHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatWindDataHandler_f
*/
VATLIB_API void Vat_SetWindDataHandler(
    VatFsdClient *fsdClient,
    VatWindDataHandler_f handler,
    void *ref);

/*!
 Installs the CloudDataHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatCloudDataHandler_f
*/
VATLIB_API void Vat_SetCloudDataHandler(
    VatFsdClient *fsdClient,
    VatCloudDataHandler_f handler,
    void *ref);

/*!
 Installs the AircraftConfigHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatAircraftConfigHandler_f
*/
VATLIB_API void Vat_SetAircraftConfigHandler(
    VatFsdClient *fsdClient,
    VatAircraftConfigHandler_f handler,
    void *ref);

/*!
 Installs the CustomAtcPacketHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatCustomAtcPacketHandler_f
*/
VATLIB_API void Vat_SetCustomAtcPacketHandler(
    VatFsdClient *fsdClient,
    VatCustomAtcPacketHandler_f handler,
    void *ref);

/*!
 Installs the CustomPilotPacketHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatCustomPilotPacketHandler_f
*/
VATLIB_API void Vat_SetCustomPilotPacketHandler(
    VatFsdClient *fsdClient,
    VatCustomPilotPacketHandler_f handler,
    void *ref);

/*!
 Installs the AircraftInfoRequestHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatAircraftInfoRequestHandler_f
*/
VATLIB_API void Vat_SetAircraftInfoRequestHandler(
    VatFsdClient *fsdClient,
    VatAircraftInfoRequestHandler_f handler,
    void *ref);

/*!
 Installs the AircraftInfoHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatAircraftInfoResponseHandler_f
*/
VATLIB_API void Vat_SetAircraftInfoHandler(
    VatFsdClient *fsdClient,
    VatAircraftInfoResponseHandler_f handler,
    void *ref);

/*!
 Installs the LegacyAircraftInfoHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatLegacyAircraftInfoHandler_f
*/
VATLIB_API void Vat_SetLegacyAircraftInfoHandler(
    VatFsdClient *fsdClient,
    VatLegacyAircraftInfoHandler_f handler,
    void *ref);

/*!
 Installs the IPCHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatIPCHandler_f
*/
VATLIB_API void Vat_SetIPCHandler(
    VatFsdClient *fsdClient,
    VatIPCHandler_f handler,
    void *ref);

/*!
 Installs the TargetScratchpadHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatTargetsScratchpadHandler_f
*/
VATLIB_API void Vat_SetTargetScratchpadHandler(
    VatFsdClient *fsdClient,
    VatTargetsScratchpadHandler_f handler,
    void *ref);

/*!
 Installs the LandlineCommandHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatStateChangeHandler_f
*/
VATLIB_API void Vat_SetLandlineCommandHandler(
    VatFsdClient *fsdClient,
    VatLandlineCommandHandler_f handler,
    void *ref);

/*!
 Installs the ControllerBreakCommandHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatControllerBreakCommandHandler_f
*/
VATLIB_API void Vat_SetControllerBreakCommandHandler(
    VatFsdClient *fsdClient,
    VatControllerBreakCommandHandler_f handler,
    void *ref);

/*!
 Installs the SharedStateIDHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatSharedStateIDHandler_f
*/
VATLIB_API void Vat_SetSharedStateIDHandler(
    VatFsdClient *fsdClient,
    VatSharedStateIDHandler_f handler,
    void *ref);

/*!
 Installs the SharedStateDIHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatSharedStateDIHandler_f
*/
VATLIB_API void Vat_SetSharedStateDIHandler(
    VatFsdClient *fsdClient,
    VatSharedStateDIHandler_f handler,
    void *ref);

/*!
 Installs the PushFlightStripHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatPushFlightStripHandler_f
*/
VATLIB_API void Vat_SetPushFlightStripHandler(
    VatFsdClient *fsdClient,
    VatPushFlightStripHandler_f handler,
    void *ref);

/*!
 Installs the HelpCommandHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatStateChangeHandler_f
*/
VATLIB_API void Vat_SetHelpCommandHandler(
    VatFsdClient *fsdClient,
    VatHelpCommandHandler_f handler,
    void *ref);

/*!
 Installs the ServerErrorHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatServerErrorHandler_f
*/
VATLIB_API void Vat_SetServerErrorHandler(
    VatFsdClient *fsdClient,
    VatServerErrorHandler_f handler,
    void *ref);

/*!
 Installs the FsdMessageHandler callback.
 \param fsdClient The client session the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatFsdMessageHandler_f
*/
VATLIB_API void Vat_SetFsdMessageHandler(
    VatFsdClient *fsdClient,
    VatFsdMessageHandler_f handler,
    void *ref);

/****************************************************************
 * General Network Routines
 ****************************************************************/

/*!
 Dump all raw FSD messages to file.
 \param fsdClient The client session the callback should be installed to.
 \param filePath can be a filename, relative or absolute path. If filePath is nullptr, dumping is stopped.
*/
VATLIB_API void Vat_DumpFsdMessageToFile(
    VatFsdClient *fsdClient,
    const char *filePath);

/*!
 Execute all pending network tasks synchronous. Call this method
 regularly to keep vatlib busy. This method will actually trigger most of the callbacks.
 \param fsdClient Session instance
 \return Returns the time until it should be called again in milliseconds.
*/
VATLIB_API int Vat_ExecuteNetworkTasks(
    VatFsdClient *fsdClient);

/*!
 Execute all pending network tasks asynchronous. Call this method
 only once and vatlib will run in the background. Most callbacks will be called from a different
 thread in this case. In order to stop, call \see Vat_StopAsyncNetworkExecution.
 \param fsdClient Session instance
*/
VATLIB_API void Vat_ExecuteNetworkTasksAsync(
    VatFsdClient *fsdClient);

/*!
 Stops asynchronous execution
 \param fsdClient
 */
VATLIB_API void Vat_StopAsyncNetworkExecution(
    VatFsdClient *fsdClient);

/*!
 Vat_CreateNetworkSession creates a new networking client session. Most applications will have only one session however
 someday this may change.

 \param clientVersionString  A string name for your application including version number. Eg. "XTower 1.0". This information
        will be sent to other clients who may query your version.
 \param clientMajorVersion Integer major version number eg. the "1" in 1.3
 \param clientMinorVersion Integer minor version number eg. the "3" in 1.3
 \param hostApplication A string name for the host application. If you're a plugin for Flight Sim, you'd put the MSFS name here.
 \param publicClientID Your public client ID as issued by the VATSIM administrator. Leave NULL for other networks.
 \param privateKey Your private key as issued by the VATSIM administrator. Leave NULL for other networks.
 \param clientCapabilities Integer flags which capabilities this clients has.
 \return A fsd client object which is essentialy a pointer to your session instance.
 \see Vat_DestroyNetworkSession
*/
VATLIB_API VatFsdClient *Vat_CreateNetworkSession(
    VatServerType serverType,
    const char *clientVersionString,
    int clientMajorVersion,
    int clientMinorVersion,
    const char *hostApplication,
    int publicClientID,
    const char *privateKey,
    int clientCapabilities);

/*!
 Vat_DestroyNetworkSession deallocates an existing client session. This should be done when a session is
 disconnected and will not be used anymore.

 \param fsdClient The client object of the session that you want to terminate.
 \return None
 \see Vat_CreateNetworkSession
*/
VATLIB_API void Vat_DestroyNetworkSession(
    VatFsdClient *fsdClient);

/*!
 Vat_SpecifyPilotLogon is used to declare the information for a pilot connection to the network.
 This does NOT connect you to the network by itself.

 \param fsdClient The client object to be used for this operation.
 \param host A string containing the host address. It can be a string IP address or hostname.
 \param port The port to connect to the server on.
 \param id A string containing the CID of the pilot.
 \param password A string containing the password of the pilot.
 \param info A struct containing the pilot specific information necessary for the connection to the server.
 \return None
 \see VatPilotConnection_t
*/
VATLIB_API void Vat_SpecifyPilotLogon(
    VatFsdClient *fsdClient,
    const char *host,
    int port,
    const char *id,
    const char *password,
    const VatPilotConnection *info);

/*!
 Vat_SpecifyATCLogon is used to declare the information for an ATC connection to the network.
 This does NOT connect you to the network by itself.

 \param fsdClient The client object to be used for this operation.
 \param host A string containing the server address. It can be a string IP address or hostname.
 \param port A port > 0 to connect to the server on.
 \param id A string containing the CID of the controller.
 \param password A string containing the password of the controller.
 \param info A struct containing the controller specific information necessary for the connection to the server.
 \return None
 \see VatATCConnection_t
*/
VATLIB_API void Vat_SpecifyATCLogon(
    VatFsdClient *fsdClient,
    const char *host,
    int port,
    const char *id,
    const char *password,
    const VatAtcConnection *info);

/*!
 Vat_Logon attempts to actually logon to the server or reconnect if disconnected.
 You must have already specified connection information using Vat_SpecifyPilotLogon or Vat_SpecifyATCLogon.

 \param fsdClient The client object to be used for this operation.
 \see Vat_Logoff
 \see Vat_SpecifyPilotLogon
 \see Vat_SpecifyATCLogon
*/
VATLIB_API void Vat_Logon(
    VatFsdClient *fsdClient);

/*!
 Vat_Logoff attempts to logoff from the network.

 \param fsdClient The client object to be used for this operation.
 \return None
 \see Vat_Logon
*/
VATLIB_API void Vat_Logoff(
    VatFsdClient *fsdClient);

/*!
 Vat_GetStatus reads the status of your connection. Note that you should avoid using this function because
 it is better to install a notifier to detect when a status has changed. This routine does NOT read new
 status changes so no work is done and no callbacks will get called.

 \param fsdClient  The client object to be used for this operation.
 \return Current connection status
*/
VATLIB_API VatConnectionStatus Vat_GetStatus(
    VatFsdClient *fsdClient);


/****************************************************************
 * Communication Routines
 ****************************************************************/

/*!
 Send a private 1-to-1 chat message.

 \param fsdClient  The client object to be used for this operation.
 \param receiver A string containing the receiver callsign
 \param message  A string containing the text message
 \return None
*/
VATLIB_API void Vat_SendTextMessage(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *message);

/*!
 Send a radio message on one or more frequencies, specified as an array of ints.

 \param fsdClient The client object to be used for this operation.
 \param frequencies Pointer to array of frequencies as integer values, e.g. 12345 for 123.45 MHz
 \param freqCount Size of frequency array
 \param message A string containing the text message
 \return None
*/
VATLIB_API void Vat_SendRadioMessage(
    VatFsdClient *fsdClient,
    int *frequencies,
    unsigned int freqCount,
    const char *message);

/*!
 Send a message to all clients with rating SUP or ADM

 \param fsdClient The client object to be used for this operation.
 \param message A string containing the text message
 \return None
*/
VATLIB_API void Vat_SendWallop(
    VatFsdClient *fsdClient,
    const char *message);

/*!
 Used to request information from a client or server. Passing null as
 callsign will probe the server for information.

 \param fsdClient The client object to be used for this operation.
 \param type Type of information you want to query
 \param callsign Client callsign, you want to probe
 \return None
*/
VATLIB_API void Vat_SendClientQuery(
    VatFsdClient *fsdClient,
    VatClientQueryType type,
    const char *callsign);

/*!
 Reply to a client query.

 \param fsdClient The client object to be used for this operation.
 \param type Type of information you want to request
 \param callsign Client callsign, you want to probe

 \param payload Array of data depending on the type of query response.
 \param payloadSize Size of the payload array

 The client is only allowed to send two types of client query responses:

 \li vatClientQueryFreq.
 \li vatClientQueryName

 The rest is handled internally in vatlib.

 vatClientQueryFreq:
 payload should have ONE string with the frequency in decimal, e.g. "119.90".
 The unit is per default MHz and does not need to be added.

 vatClientQueryInfo:
 Don't use it! Vatlib will send it itself. If you try anyway, it
 will be ignored.

 vatClientQueryAtis:
 Don't use it. Use Vat_SendControllerAtis instead.

 vatClientQueryServer:
 Don't use it! Vatlib will send it itself. If you try anyway, it
 will be ignored.

 vatClientQueryName:
 Payload should contain two strings. The first one is the users name and is
 mandatory. The second one should contain any additional usefull information
 about the user himself. This is for example used by Euroscope to send the
 used AIRAC information. Leave it blank if you don't want to send anything.

 vatClientQueryAtc:
 Don't use it! Vatlib will send it itself. If you try anyway, it
 will be ignored.

 vatClientQueryCaps:
 Don't use it! Vatlib will send it itself. If you try anyway, it
 will be ignored.

 vatClientQueryIP:
 Don't use it! Vatlib will send it itself. If you try anyway, it
 will be ignored.

 \return None
 \see VatClientQueryType, Vat_SendControllerAtis
*/
VATLIB_API void Vat_SendClientQueryResponse(
    VatFsdClient *fsdClient,
    VatClientQueryType type,
    const char *callsign,
    const char **payload,
    int payloadSize);

/*!
 Request CSL model info from someone else.

 \param fsdClient The client object to be used for this operation.
 \param callsign Client callsign, you want to request from
 \return None
*/
VATLIB_API void Vat_RequestAircraftInfo(
    VatFsdClient *fsdClient,
    const char *callsign);

/****************************************************************
 * ATC routines
 ****************************************************************/

/*!
 Send an ATC position update packet.

 \param fsdClient The client object to be used for this operation.
 \param info Struct containing ATC position details
 \return None
 \see VatAtcPosition
*/
VATLIB_API void Vat_SendATCUpdate(
    VatFsdClient *fsdClient,
    const VatAtcPosition *info);

/*!
 Requests an ATC Handoff.

 \param fsdClient The client object to be used for this operation.
 \param receiver Controller callsign, from which you are requesting
 \param target Aircraft callsign, you would to hand off
 \return None
 \see VatAtcPosition
*/
VATLIB_API void Vat_RequestHandoff(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *target);

/*!
 Accept an ATC Handoff

 \param fsdClient The client object to be used for this operation.
 \param receiver Controller callsign, who requested the handoff
 \param target Aircraft callsign, which is handed off
 \return None
*/
VATLIB_API void Vat_AcceptHandoff(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *target);

/*!
 Cancel a handoff with another ATC.

 \param fsdClient The client object to be used for this operation.
 \param receiver Controller callsign, who requested the handoff
 \param target Aircraft callsign, which is handed off
 \return None
*/
VATLIB_API void Vat_CancelHandoff(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *target);

/*!
 Amend a flight plan. The one stored on the server will be overwritten
 with the one you are sending now.

 \param fsdClient The client object to be used for this operation.
 \param callsign Aircraft callsign, which flightplan needs to be amended.
 \param flightplan New full flightplan including all changed you have made.
 \return None
*/
VATLIB_API void Vat_AmendFlightPlan(
    VatFsdClient *fsdClient,
    const char *callsign,
    const VatFlightPlan *flightplan);

/*!
 Send an ASRC-type ID to check if someone is a next-gen client.

 \param fsdClient The client object to be used for this operation.
 \param atc
 \return None
*/
VATLIB_API void Vat_SendSharedStateID(
    VatFsdClient *fsdClient,
    const char *atc);

/*!
 Send a land line command to other aircraft.

 \param fsdClient The client object to be used for this operation.
 \param callsign Aircraft callsign
 \param command Landline command
 \param type landline command type
 \param ip
 \param port
 \return None
*/
VATLIB_API void Vat_SendLandlineCommand(
    VatFsdClient *fsdClient,
    const char *callsign,
    VatLandlineCmd command,
    VatLandlineType type,
    const char *ip,
    int port);

/*!
 Send a tracking command.

 \param fsdClient The client object to be used for this operation.
 \param atc ATC callsign, may be null if not needed.
 \param aircraft Aircraft callsign
 \param command Tracking command
 \return None
*/
VATLIB_API void Vat_SendTrackingCommand(
    VatFsdClient *fsdClient,
    const char *atc,
    const char *aircraft,
    VatTrackingCmd command);

/*!
 Turns the break on and off

 \param fsdClient The client object to be used for this operation.
 \param wantsBreak Pass true to turn the break on and false to turn it off.
 \return None
*/
VATLIB_API void Vat_SendBreakCommand(
    VatFsdClient *fsdClient,
    bool wantsBreak);

/*!
 Send controller atis

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiver callsign
 \param atis Controller atis struct
 \return None
*/
VATLIB_API void Vat_SendControllerAtis(
    VatFsdClient *fsdClient,
    const char *receiver,
    const VatControllerAtis *atis);

/*!
 Sends a secondary ATC position update

 \param fsdClient The client object to be used for this operation.
 \param index ???
 \param latitude
 \param longitude
 \return None
*/
VATLIB_API void Vat_SendSecondaryATCUpdate(
    VatFsdClient *fsdClient,
    int index,
    double latitude,
    double longitude);

/*!
 Pushes a pilots flight strip with a type identifier and array of annotations - Added by RAC on 02/28/2007

 \param fsdClient The client object to be used for this operation.
 \param receiver Controller callsign the flight strip is pushed to.
 \param target Aircraft callsign, the flight strip belongs to.
 \param type ???
 \param annotations Array of strings with annotations
 \return None
*/
VATLIB_API void Vat_PushFlightStripEx(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *target,
    int type,
    const char **annotations);

/*!
 Turns the request for help on and off

 \param fsdClient The client object to be used for this operation.
 \param needHelp Pass true to turn the help request on and false to turn it off.
 \param message Pass in a optional message. May be null if not needed.
 \return None
*/
VATLIB_API void Vat_SendHelpCommand(
    VatFsdClient *fsdClient,
    bool needHelp,
    const char *message);

/*!
 Notifies all in-range clients that our controller info or ATIS has changed.
 This will automatically increase the ATIS identifier for other ATC clients
 in range.

 \param fsdClient The client object to be used for this operation.
 \return None
*/
VATLIB_API void Vat_NotifyNewInfo(
    VatFsdClient *fsdClient);

/****************************************************************
 * Pilot Routines
 ****************************************************************/

/*!
 Send a pilot position update packet.

 \param fsdClient The client object to be used for this operation.
 \param info Struct with pilot position details
 \return None
 \see VatPilotPosition
*/
VATLIB_API void Vat_SendPilotUpdate(
    VatFsdClient *fsdClient,
    const VatPilotPosition *info);

/*!
 Send an interim pilot position update packet. This is sent to a specific
 callsign only instead of broadcasted.
 It depends on the receiving clients software if they accept intermin pilot
 position updates.

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiving callsign
 \param pilotPosition Struct with pilot position details
 \return None
 \see VatPilotPosition
*/
VATLIB_API void Vat_SendInterimPilotUpdate(
    VatFsdClient *fsdClient,
    const char *receiver,
    const VatInterimPilotPosition *pilotPosition);

/*!
 Sends the server a flight plan.

 \param fsdClient The client object to be used for this operation.
 \param flightPlan Your flightplan
 \return None
*/
VATLIB_API void Vat_SendFlightPlan(
    VatFsdClient *fsdClient,
    const VatFlightPlan *flightPlan);

/*!
 Tell another client about your plane's info (CSL model, etc.)

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiver callsign
 \param engineType Engine Type
 \param CSLModel A string containing your CSL model
 \return None
 \deprecated Do not use this method anymore. This legacy is just
 here for SB2.3 clients. Most should use the modern one.
 \see Vat_SendAircraftInfo
*/
VATLIB_API void Vat_SendLegacyPlaneInfo(
    VatFsdClient *fsdClient,
    const char *receiver,
    VatEngineType engineType,
    const char *CSLModel);

/*!
 Tell another client about your plane's info (CSL model, etc.).
 This is the modern send plane info that SB3, FSInn and XSB will use.

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiver callsign
 \param info Struct with plane information
 \return None
*/
VATLIB_API void Vat_SendAircraftInfo(
    VatFsdClient *fsdClient,
    const char *receiver,
    const VatAircraftInfo *info);

/*!
 Send JSON encoded aircraft config to target callsign.

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiver callsign
 \param aircraftConfig JSON string
 \return None
*/
VATLIB_API void Vat_SendAircraftConfig(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *aircraftConfig);

/*!
 Broadcast JSON encoded aircraft config to everyone in range.

 \param fsdClient The client object to be used for this operation.
 \param aircraftConfig JSON string
 \return None
*/
VATLIB_API void Vat_SendAircraftConfigBroadcast(
    VatFsdClient *fsdClient,
    const char *aircraftConfig);

/*!
 Send a custom pilot packet to target. This can be any subtype, as long
 as it does not clash with the existing subtypes which are known to vatlib.

 \param fsdClient The client object to be used for this operation.
 \param receiver Receiver callsign
 \param subType Packet subtype
 \param data Array of strings containing the packet tokens
 \param dataSize Size of tokens
 \return None
*/
VATLIB_API void Vat_SendCustomPilotPacket(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *subType,
    const char **data,
    int dataSize);


/****************************************************************
 * Admin Routines
 ****************************************************************/

/*!
 Kills a user off of the server with a message

 \param fsdClient The client object to be used for this operation.
 \param receiver A string containing the callsign to be killed from the network
 \param message A string containing a message to the user
 \return None
*/
VATLIB_API void Vat_KillUser(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *message);

/*!
 Sends a ping packet to the server.
 Pass in a callsign to ping a user, *A to ping all ATC or ?? to ping everyone.

 \param fsdClient The client object to be used for this operation.
 \param receiver A string containing the receiver callsign
 \return None
*/
VATLIB_API void Vat_SendPing(
    VatFsdClient *fsdClient,
    const char *receiver);

/****************************************************************
 * Weather Routines
 ****************************************************************/

/*!
 Sends a Metar request.

 \param fsdClient The client object to be used for this operation.
 \param station A string with ICAO identifier of the station
 \return None
*/
VATLIB_API void Vat_RequestMetar(
    VatFsdClient *fsdClient,
    const char *station);

/*!
 Sends Metar data.

 \param fsdClient The client object to be used for this operation.
 \param receiver A string containing the receiver callsign
 \param metar Metar
 \return None
*/
VATLIB_API void Vat_SendMetar(
    VatFsdClient *fsdClient,
    const char *receiver,
    const char *metar);

/*!
 Sends a request to the server for weather. The response from
 the server are detailed layers of wind, clouds and temperature.
 They are calculated from the local METAR and some additional
 woodoo. So their usage is questionable. Consider using
 \see Vat_RequestMetar instead.

 \param fsdClient The client object to be used for this operation.
 \param station The station whether is requested for
 \return None
*/
VATLIB_API void Vat_RequestWeather(
    VatFsdClient *fsdClient,
    const char *station);

/*!
 **************************************************************
    General Voice Notes
 **************************************************************

 Your application is responsable for memory management of all
 voice objects - each one has a Create and Delete routine.

 Like Network, most callbacks take a void parameter. You can
 use this to pass data from your call out of voice to your
 callback.
*/

/*!
 Configures the log handler callback up to a maximum severity.
 \param severityLevel Initial maximum severity level. Handler will be called
                      only for log message equal or below this level. Default is None.
 \param handler Function to be called back or NULL to disable the callback.
                If no callback is set, all log messages are streamed to stderr.

*/
VATLIB_API void Vat_SetVoiceLogHandler(
    VatSeverityLevel severityLevel,
    VatlibLogHandler_t handler);

/*!
 Vat_SetVoiceLogSeverityLevel modifies the maximum log severity level.
 \param severityLevel Maximum severity level
*/
VATLIB_API void Vat_SetVoiceLogSeverityLevel(
    VatSeverityLevel severityLevel);

/****************************************************************
 * Audio Service Routines
 ****************************************************************/

/*!
 \class VatAudioService

 Audio service object. This class acts as the class processing and triggering all internal audio tasks.
*/
class VatAudioService;

/*!
 Vat_CreateAudioService creates a new audio service instance. You should only
 need one of these in your client.

 \return Upon success Vat_CreateAudioService returns a valid pointer to a
 VatAudioService object. If something went wrong a NULL pointer will be returned
 and must be handeled by the calling application.
*/
VATLIB_API VatAudioService *Vat_CreateAudioService();

/*!
 Vat_DestroyAudioService
 Deallocates a VatAudioService object. Delete your service on cleanup.
 \param audioService a valid VatAudioService object.
 */
VATLIB_API void Vat_DestroyAudioService(
    VatAudioService *audioService);

/*!
  \brief Execute all pending audio tasks synchronously.

  This routine does all processing necessary to run the voice
  subsystem.  Call this routine often! It will return the
  number of seconds until it needs to be called again.  You
  may receive a call back from this routine if you have notifiers
  functions installed on some of your objects.

  (Note: currently the time returned by Vat_ExecuteVoiceTasks may not
  be reliable. I suggest simply calling it on a fixed timer for
  now, perhaps 50 ms. This will be addressed in future lib
  updates.)

  During normal operations, this is the only vatlib routine you need
  to call - it takes care of all audio processing.

  \param audioService Pointer to the audio service, which should execute tasks
  \return Returns the time until it should be called again in milliseconds.

  \sa Vat_AsyncExecuteTasks
 */
VATLIB_API int Vat_ExecuteVoiceTasks(
    VatAudioService *audioService);

/*!
  \brief Start async audio processing

  This routine does all processing necessary to run the voice
  subsystem asynchronous. It will return the immediatly und run
  as until you stop it again. You may receive a call back from
  this routine if you have notifiers functions installed on
  some of your objects.

  \param audioService Audio service object.

  \sa Vat_StopAsyncExecution
  \sa Vat_ExecuteVoiceTasks
 */
VATLIB_API void Vat_AsyncExecuteTasks(
    VatAudioService *audioService);


/*!
  \brief Stop async audio processing

  This routine will stop any asynchronous running audio processing.
  You won't hear any audio any more and recorded audio will not
  sent to the voice channel any more.

  \param audioService Audio service object
  \sa Vat_StopAsyncExecution
  \sa Vat_ExecuteVoiceTasks
 */
VATLIB_API void Vat_StopAsyncExecution(
    VatAudioService *audioService);


/****************************************************************
 * General Voice Routines
 ****************************************************************/

/*!
 The base type for all Voice audio objects is either
 AudioProducer, AudioConsumer or both. Those objects represent an audio
 device that both send and/or receive audio.

 Producers only:
 \li VatLocalInputCodec

 Consumers only:
 \li VatLocalOutputCodec

 Producers and consumers:
 \li VatAudioMixer
 \li VatVoiceChannel
 \li VatFileBasedRecorder

 Each producer and consumer has a number of ports, specified by
 64 bit integers.  You can receive audio on any port number, and
 does port numbering have to be contiguous. The idea is to allow
 an audio consumer to identify audio from different sources.

 Most audio devices can receive and send on only one port; port
 0 is used by convention.

 Input and output audio ports are separate and distinct; you
 can connect a device to one device for input on port 0 and
 another for output on port 0. To wire both input port 0 and
 output port 0 you must make two connections.
*/

/*! Audio Producer Type */
typedef enum
{
    VatProducerVoiceChannel,
    VatProducerLocalInput,
    VatProducerAudioMixer,
    VatProducerFileBasedRecorder
} VatAudioProducerType;

/*! Audio Consumer Type */
typedef enum
{
    VatConsumerNone,
    VatConsumerVoiceChannel,
    VatConsumerLocalOutput,
    VatConsumerAudioMixer,
    VatConsumerFileBasedRecorder
} VatAudioConsumerType;

/*!
 This routine links an audio source and an audio destination.
 Specify the source and destination types, objects and the port numbers
 for each one. If the objects were previously connected, any
 old connections are severed. You can connect a producer to a
 NULL consumer to disconnect it.
 Important: (The producer cannot be NULL.)

 \param producerType Type of audio producing object
 \param producer Pointer to audio producing object
 \param producerPort Selected producer's output port
 \param consumerType Type of audio consuming object
 \param consumer Pointer to audio consuming object
 \param consumerPort Selected consumer's input port
 */
VATLIB_API void Vat_VoiceConnect(
    VatAudioProducerType producerType,
    void *producer,
    int producerPort,
    VatAudioConsumerType consumerType,
    void *consumer,
    int consumerPort);

/****************************************************************
 * UDP Audio Port
 ****************************************************************/

/*!
 \class VatUDPAudioPort

 This object is an actual UDP port that voice data is sent through.
 You should not have to use it directly; simply instantiate an object
 and pass it to any direct voice links and channels as you make them.

 A single UDP audio port is adequate for all voice configurations.
 The UDP audio port is capable of sorting out multiple audio streams
 all coming to the single UDP port.
*/

class VatUDPAudioPort;

/*!
    Vat_CreateUDPAudioPort creates a new UDP audio port. You should only
    need one of these in your client.

    Pass an existing \param audioService which will take care of task execution.

    \param port is the UDP port number you want to use. If you pass 0, an ephemeral port will
    be taken and is the recommended usage. You can also set a fixed port and vatlib will
    try to bind to it.

    \return Upon success Vat_CreateUDPAudioPort() returns a valid pointer to a
    VatUDPAudioPort object. If something went wrong (e.g. the fixed port is already taken)
    a NULL pointer will be returned and must be handeled by the calling application.
 */
VATLIB_API VatUDPAudioPort *Vat_CreateUDPAudioPort(
    VatAudioService *audioService,
    int port);

/*!
 Vat_DestroyUDPAudioPort
 Deallocates a VVLUDPAudioPort object. Delete your port on cleanup.
 \param audioPort A valid VatUDPAudioPort object.
 */
VATLIB_API void Vat_DestroyUDPAudioPort(
    VatUDPAudioPort *audioPort);

/*!
 This method returns the used port number. If a fixed port
 was given in Vat_CreateUDPAudioPort() and the allocation was
 successful, it will return the same port. If a ephemeral port
 was taken, this method returns the actual UDP port.
 \param udpAudioPort A valid VatUDPAudioPort object.
 \return Port number as integer value
 \see Vat_CreateUDPAudioPort
 */
VATLIB_API int Vat_GetPortNumber(
    VatUDPAudioPort *udpAudioPort);

/****************************************************************
 * Local Audio Devices
 ****************************************************************/

/*!
 \class VatLocalInputCodec

 The local input codec object receives audio from a sound card, e.g. from a mic. It also contains the codec
 that encodes and decodes audio.

 The local input codec also contains code to do automatic gain correction (AGC), Acoustic Echo Cancellation
 and hardware selection and has APIs to manage these functions.
*/
class VatLocalInputCodec;

/*!
 \class VatLocalOutputCodec

 The local output codec object sends audio to a sound card, e.g. to speakers. It also contains the codec
 that encodes and decodes audio.

 The local codec also contains code to do hardware selection and has APIs to manage these functions.
*/
class VatLocalOutputCodec;

/*! This callback is called when audio is playing. */
typedef void (* VatLocalCodecAudioHandler_f)(
    bool isAudioPlaying,
    void *ref);

/*! This callback is called once for each hardare device when you attempt to "enumerate" hardware. */
typedef void (* VatLocalCodecHardwareIterator_f)(
    int deviceIndex,
    const char *hardwareName,
    void *ref);

/*! Audio codec type */
typedef enum
{
    vatCodecLegacy  = (1 << 0), /*!< Legacy codec */
//    vatCodecOpus    = (1 << 1), /*!< Opus codec */
}
VatAudioCodec;

/*!
 Create a new local output codec object.

 \param audioService Pointer to audio service instance
 \param codec Selected audio codec
 \return Returns a pointer to a new VatLocalInputCodec instance
*/
VATLIB_API VatLocalInputCodec *Vat_CreateLocalInputCodec(
    VatAudioService *audioService,
    VatAudioCodec codec);

/*!
 Create a new local output codec object.

 \param audioService Pointer to audio service instance
 \param codec Selected audio codec
 \return Returns a pointer to a new VatLocalOutputCodec instance
*/
VATLIB_API VatLocalOutputCodec *Vat_CreateLocalOutputCodec(
    VatAudioService *audioService,
    VatAudioCodec codec);

/*!
 This destroys the input codec object - call this when you are done.
 \param inputCodec Object to be destroyed
*/
VATLIB_API void Vat_DestroyLocalInputCodec(
    VatLocalInputCodec *inputCodec);

/*!
 This destroys the output codec object - call this when you are done.
 \param localCodec Object to be destroyed
*/
VATLIB_API void Vat_DestroyLocalOutputCodec(
    VatLocalOutputCodec *localCodec);

/*!
 This routine iterates all hardware devices. Your callbacks will be called
 once for each INPUT hardware device. Hardware is numbered starting from 0.

 \param localCodec local output codec object
 \param inputIterator If a valid callback is passed, this callback is called for each found input hardware device.
 \param ref Pointer to user defined data, which will be passed to the callback
 \param currentInput If a valid pointer is passed, its value will be modified to the current input device index.
*/
VATLIB_API void Vat_GetInputDeviceInfo(
    VatLocalInputCodec *localCodec,
    VatLocalCodecHardwareIterator_f inputIterator,
    void *ref,
    int *currentInput);

/*!
 This routine iterates all hardware devices. Your callbacks will be called
 once for each OUTPUT hardware device. Hardware is numbered starting from 0.

 \param localCodec local output codec object
 \param outputIterator If a valid callback is passed, this callback is called for each found output hardware device.
 \param ref pointer to user defined data, which will be passed to the callback
 \param currentOutput If a valid pointer is passed, its value will be modified to the current output device index.
*/
VATLIB_API void Vat_GetOutputDeviceInfo(
    VatLocalOutputCodec *localCodec,
    VatLocalCodecHardwareIterator_f outputIterator,
    void *ref,
    int *currentOutput);

/*!
 This routine changes the input device.
 \param localCodec Local input object.
 \param inputIndex Zero-based index of the selected audio device.
 \return True if successful and recording started
 \sa Vat_GetInputDeviceInfo
*/
VATLIB_API bool Vat_SetAudioInputDevice(
    VatLocalInputCodec *localCodec,
    int inputIndex);

/*!
 This routine changes the output device.
 \param localCodec Local output object.
 \param outputIndex Zero-based index of the selected audio device.
 \sa Vat_GetOutputDeviceInfo
*/
VATLIB_API void Vat_SetAudioOutputDevice(
    VatLocalOutputCodec *localCodec,
    int outputIndex);

/*!
 This routine changes the total volume of the given local codec.
 \param localCodec Local output instance
 \param volume Volume value is accepted as percent from 0 (muted) - 300 ( 3x gain ).
*/
VATLIB_API void Vat_SetOutputVolume(
    VatLocalOutputCodec *localCodec,
    int volume);

/*!
 This routine returns total volume of the given local codec
 \param localCodec Local output instance.
 \return Volume setting as percent from 0 (muted) - 300 ( 3x gain )
*/
VATLIB_API int Vat_GetOutputVolume(
    VatLocalOutputCodec *localCodec);

/*!
 Returns true if the input hardware device is functioning, false if it is not.
 \param localCodec Local input instance.
 \return Returns true is everything is ok.
*/
VATLIB_API bool Vat_IsRecordingAlive(
    VatLocalInputCodec *localCodec);

/*!
 Returns whether the hardware device is functioning for output.
 \param localCodec Local output instance
 \return True if everything is ok
 \sa Vat_IsRecordingAlive
*/
VATLIB_API bool Vat_IsPlaybackAlive(
    VatLocalOutputCodec *localCodec);

/*!
 The local codec comes with an optional band-pass filter that makes
 transmissions sound more like they might in an airplane. Default
 is off.
 \warning Currently not implemented
 \param localCodec Local output hardware
 \param enable Set true to enable, false otherwise
*/
VATLIB_API void Vat_EnableFilter(
    VatLocalOutputCodec *localCodec,
    bool enable);

/*!
 This can be used to indicate if the PTT button is pressed, for the purpsoe of
 generating "side tone". If your application supports side-tone, call this
 when the PTT key is toggled.

 This function really controls side tone, NOT audio out of the
 hardware device. The audio codec always sends audio - use the mixer to
 disable transmission to radios, for example.

 \param localCodec Local output object
*/
VATLIB_API void Vat_PlaySideTone(
    VatLocalOutputCodec *localCodec);

/*!
 This routine sets the level of side tone. By default it is muted.
 \param localCodec Local output object
 \param volume Volume as a percent integer from 0 to 100. 0 means muted and 100
        maximum volume.
*/
VATLIB_API void Vat_SetSideToneVolume(
    VatLocalOutputCodec *localCodec,
    int volume);

/*!
 Sets the local codec audio handler callback. It will be called as soon as
 there is audio played on the output device.
 Use this for example to indicate it via LED to the user.
 \param localCodec Local output object.
 \param handler Function to be called back or NULL to disable the callback.
 \param ref Pointer to user defined data, which will be passed to the callback.
 \
*/
VATLIB_API void Vat_SetLocalCodecAudioHandler(
    VatLocalOutputCodec *localCodec,
    VatLocalCodecAudioHandler_f handler,
    void *ref);

/****************************************************************
 * Voice Channel
 ****************************************************************/

/*!
 \class VatVoiceChannel

 VatVoiceChannel represents a connection to a voice room.  All audio from any
 member of the room comes out output port 0. Any audio sent into input port
 0 is sent to all members of the room. A VVLVoiceChannel must be connected and
 can be connected to many different rooms, but only one room at a time.

 You can attach a handler to the room to find out the status of the room in a
 number of ways.

 Because voice channels send audio over UDP, you need a single VatUDPAudioPort
 to make one or more VatVoiceChannels.
*/
class VatVoiceChannel;

/*! The state of your voice room connection has changed. */
typedef void (* VatConnectionChangedHandler_f)(
    VatVoiceChannel *channel,
    VatConnectionStatus oldStatus,
    VatConnectionStatus newStatus,
    void *ref);

/*! A client has joined the voice room */
typedef void (* VatClientJoinedHandler_f)(
    VatVoiceChannel *channel,
    int id,
    const char *callsign,
    void *ref);

/*! A client has left the voice room */
typedef void (* VatClientLeftHandler_f)(
    VatVoiceChannel *channel,
    int id,
    const char *callsign,
    void *ref);

/*! The state of room voice reception has changed (started/stopped) */
typedef void (* VatVoiceReceptionHandler_f)(
    VatVoiceChannel *channel,
    bool isVoiceReceiving,
    void *ref);

/*! This callback receipes info about each member of the room. */
typedef void (* VatVoiceChannelRoomIterator_f)(
    VatVoiceChannel *channel,
    int id,
    const char *name,
    void *ref);

/*!
 This creates a new voice room object. When the room is created,
 it starts in a disconnected state.

 \param audioService Audio service instance.
 \param ip The IP address of the server.
 \param port The port of the server, usually 3782.
 \param channel The string name of the voice room to join.
 \param callsign Your callsign as it will appear in the room. By convention
        this is your VATSIM callsign followed by your CID.
 \param udpAudioPort An UDP audio port object.
 \return Returns a new VatVoiceChannel instance
*/
VATLIB_API VatVoiceChannel *Vat_CreateVoiceChannel(
    VatAudioService *audioService,
    const char *ip,
    int port,
    const char *channel,
    const char *callsign,
    VatUDPAudioPort *udpAudioPort);

/*!
 This will destroy an existing voice channel.
 \param channel Existing voice channel instance
*/
VATLIB_API void Vat_DestroyVoiceChannel(
    VatVoiceChannel *channel);

/*!
 Returns useful room info, including the room name, your callsign, your
 assigned ID, and a vector of all clients in the room (including yourself!)

 \param channel Channel object
 \param iterator Information callback
 \param roomName String containing the room name after routine has returned
 \param roomID Integer containing your room id after routine has returned
 \param callsign String containing your callsign in the room
 \param ref User defined data for RoomIterator callback
*/
VATLIB_API void Vat_GetRoomInfo(
    VatVoiceChannel *channel,
    VatVoiceChannelRoomIterator_f iterator,
    char *roomName,
    int16_t *roomID,
    char *callsign,
    void *ref);

/*!
 Use this function to change the room you want to join.
 Call this right before connecting to pick a new room.
 \warning Only use when you are NOT connected!
 \param channel Voice channel object
 \param hostName Voice server hostname
 \param port Voice server port
 \param channelName Channel name on the voice server
 \param callsign Your own callsign
*/
VATLIB_API void Vat_SetRoomInfo(
    VatVoiceChannel *channel,
    const char *hostName,
    int port,
    const char *channelName,
    const char *callsign);

/*!
 Begin connecting to the room. You will receive status messages for either a
 success or failure to join.
 \param channel Voice channel object
*/
VATLIB_API void Vat_JoinRoom(VatVoiceChannel *channel);

/*!
 Disconnect from the room. Disconnect message will be sent later when the disconnect
 completes.
 \param channel Voice channel object
*/
VATLIB_API void Vat_DisconnectFromRoom(
    VatVoiceChannel *channel);

/*!
 Installs the voice channel connection changed callback.
 \param channel The voice channel the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatConnectionChangedHandler_f
*/

VATLIB_API void Vat_SetConnectionChangedHandler(
    VatVoiceChannel *channel,
    VatConnectionChangedHandler_f handler,
    void *ref);

/*!
 Installs the voice channel client joined callback.
 \param channel The voice channel the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatClientJoinedHandler_f
*/
VATLIB_API void Vat_SetClientJoinedHandler(
    VatVoiceChannel *channel,
    VatClientJoinedHandler_f handler,
    void *ref);

/*!
 Installs the voice channel client left callback.
 \param channel The voice channel the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatClientJoinedHandler_f
*/
VATLIB_API void Vat_SetClientLeftHandler(
    VatVoiceChannel *channel,
    VatClientJoinedHandler_f handler,
    void *ref);

/*!
 Installs the voice channel reception callback.
 \param channel The voice channel the callback should be installed to.
 \param handler Pass a function pointer to install or nullptr to delete it.
 \param ref Pointer to user defined data, which will be passed to the callback
 \warning Your callback must be thread safe, because it is not guaranteed that
 the callback is always called from the main thread.
 \see VatVoiceReceptionHandler_f
*/
VATLIB_API void Vat_SetVoiceReceptionHandler(
    VatVoiceChannel *channel,
    VatVoiceReceptionHandler_f handler,
    void *ref);

/****************************************************************
 * File Based Recorder
 ****************************************************************/

/*!
 \class VatFileBasedRecorder

 CFileBasedRecorder plays back and records compressed audio to disk.
 It can be used to create recorded messages, like an ATIS recording.

 CFileBasedRecorder is very primitive in its state handling.  You cannot
 record and play back at the same time; one must be stopped before the other
 starts.

 Playback will automatically stop at the end of file unless looping is
 enabled.

 FILE FORMAT:

 Audio files are formed as follows:

 For each compressed audio packet:
 A 32-bit unsigned long big endian time stamp in miliseconds. 0 represents
 the start of audio playback. Note that packets may not be continuous or
 may represent multiple recordings.

 A 32-bit unsigned long big endian size for the following audio data.

 A variable sized audio packet, including envelope information but not
 including client ID, etc.

 The file simply ends at end of file, but must end on a predicted packet
 ending.
*/
class VatFileBasedRecorder;

/*!
 This creates a new file based recorder object.
 \param audioService Pointer to audio service instance
 \return Returns a pointer to a new VatFileBasedRecorder instance
*/
VATLIB_API VatFileBasedRecorder	*Vat_CreateFileBasedRecorder(
    VatAudioService *audioService);

/*!
 Destroys a file based recorder object
 \param recorder Object to be destroyed
*/
VATLIB_API void Vat_DestroyFileBasedRecorder(
    VatFileBasedRecorder *recorder);

/*!
 You must set the file name at least once to specify where to
 record or play back to.  The file name takes effect on the next
 record or playback operation.  One recorder can only play or
 record one file at a time.
 \param recorder Recorder instance
 \param fileName Recorder file name
*/
VATLIB_API void	Vat_FileBasedRecorderSetFileName(
    VatFileBasedRecorder *recorder,
    const char *fileName);

/*!
 Recording control.
 Starting recording overwrites any exsiting
 file data.
 \param recorder Recorder instance
*/
VATLIB_API void Vat_FileBasedRecorderStartRecording(
    VatFileBasedRecorder *recorder);

/*!
 Stops recording of the file.
 \param recorder Recorder instance
*/
VATLIB_API void Vat_FileBasedRecorderStopRecording(
    VatFileBasedRecorder *recorder);

/*!
 Is recorder recording?
 \param recorder Recorder instance
 \return Returns true if record is in progress, false otherwise.
*/
VATLIB_API bool Vat_FileBasedRecorderIsRecording(
    VatFileBasedRecorder *recorder);

/*!
 Playback control.
 Playback automatically stops at the end of the
 file unless looping is enabled
 \param recorder Recorder instance
 \param loop Pass true to enable looping
*/
VATLIB_API void Vat_FileBasedRecorderStartPlaying(
    VatFileBasedRecorder *recorder,
    bool loop);

/*!
 Stops playback.
 \param recorder Recorder instance
*/
VATLIB_API void	Vat_FileBasedRecorderStopPlaying(
    VatFileBasedRecorder *recorder);

/*!
 Is recorder playing?
 \param recorder Recorder instance
 \return True if playing, false if not.
*/
VATLIB_API bool Vat_FileBasedRecorderIsPlaying(
    VatFileBasedRecorder *recorder);

/****************************************************************
 * Audio Mixer
 ****************************************************************/

/*!
 \class VatAudioMixer

 The audio mixer is an arbitrary audio router. Its two main capabilities
 are merging incoming audio from multiple producers into one consumer and
 splitting/replicating audio from one producer to multiple consumers.

 (Because audio devices can only be connected one input to one output,
 the mixer is the only way to split or merge audio.)

 The mixer can be thuoght of as an NxM matrix, with each square
 representing whether audio will make it from the input to the output.

 For example:
 <pre>

                 Output Port
                 0   1   2   3
             0       X
 Input Port  1   X   X
             2               X

 </pre>
 (Here we have a mixer connections from 1->0, 1->1, 0->1, and 2->3.

 In this example, input on port 0 will be sent to output port 1.
 Ipnput on port 1 will be sent to output ports 0 and 1, and
 input on port 2 will be sent to output port 3.  Note that output
 port 1 is fed by two input ports and inpnut port 1 feeds two
 output ports.

 An example of why you might do this is to share one codec with
 multiple voice rooms. For example:
 <pre>

                             Output Port
                 Speaker     V1  V2  V3
             Mic             X       X
 Input Port  V1      X
             V2      X
             V3
 </pre>

 In this example the Mic is routed to voice rooms 1 and 3 but not
 2; it's as if we're listening only in voice room 2.  Further more
 Voice rooms 1 and 2 but not 3 are routed to the speaker, so we'll
 hear voice rooms 1 and 2 but 3 is 'muted'.

 You can also do whacky thngs like this:

 <pre>
                             Output Port
                 Speaker     V1  V2  V3
             Mic             X       X
 Input Port  V1      X
             V2      X               X
             V3
 </pre>

 In this example voice room 2 is routed to voice  room 3.  So
 everything said in voice room 2 will be echoed to voice room 3
 but not vice versa!  Note that connections are either input or
 output, so any full-duplex source like a voice room must be routed
 in two places, its input and its output.

 As a final strange example of what you can do with the mixer,
 you can in fact route the mixer into itself!  Why would you ever
 want to do this?!?  The answer is to provide an easy way to mute
 whole sets of audio.  For example, consider this very complicated
 topology (just the outputs are shown for convenience):

         Speaker V1  V2  V3  V4  L1  L2  L3
 Mic             X   X   X   X

 In this case we have four voice rooms and 3 land line clals.  To
 mute every voice call we have to hit he mixer in four cases.  But
 you can also do this:
                 Speaker Voice Send V1 V2 V3 V4 Land Send L1 L2 L3
 Mic                         1                       2
 Voice Return                5       3 3  3  3
 Land Return                                              4  4  4

 I have numbered the mixer points for clarity:
 1. This connects the Mic to the 'voice send' bus.  Deactivating this
 point mutes the Mic for all voice rooms at once.
 2. This connects the Mic to the 'land line send' bus.  Deactivating
 this point mutes the Mic for all land lines.
 3. These points conect the voice room feed to individual voice rooms.
 Any one can be deactivated to mute a specific voice room.
 4. These points connect the land line feed to individual land lines.
 Any one of these points can be deactivated to kill an individual
 land line.
 5. This point would form a feedback loop sending the voice return
 back into the voice send.  This is a VERY BAD IDEA!  Do not EVER
 route the mixer into itself.

 The point of the above example is that since you can have about 4
 billion inputs and outputs on the mixer, it can be useful to group
 related sets.  In this case to implement the functionality of muting
 all voice rooms when a land line call is in effect, the land line
 code must simply tap point 1 on the board, not all of points 3.
*/
class VatAudioMixer;

/*!
 This routine creates a new audio mixer. Most apps only require
 one audio mixer.

 \return Returns a pointer to a new instance of VatAudioMixer.
 */
VATLIB_API VatAudioMixer *Vat_CreateAudioMixer();

/*!
 Destroys the audio mixer object.
 \param mixer Object to be destroyed.
*/
VATLIB_API void Vat_DestroyAudioMixer(
    VatAudioMixer *mixer);

/*!
 This routine creates or removes a mixer connection.  Audio
 received on inPort will or will not be sent to outPort
 based on 'connect'. There is no limit to how many inputs
 one output can take a feed from or how many outputs one input
 can feed.
 \param mixer Audio mixer object.
 \param inPort Port with the input connected to it
 \param outPort Port with the output connected to it
 \param connect Pass true to start transmitting between in and output port.
                False to stop.
*/
VATLIB_API void Vat_MakeMixerConnection(
    VatAudioMixer *mixer,
    int inPort,
    int outPort,
    bool connect);

/*!
 This routine returns whether a mixer connection is on
 or not.  Use this to figure out what is muted.
 \param mixer Audio mixer instance
 \param inPort Input port
 \param outPort Output port
 \return True if connection is on, false otherwise
*/
VATLIB_API bool Vat_HasMixerConnection(
    VatAudioMixer *mixer,
    int inPort,
    int outPort);

/*!
 These routines tell whether an input port has
 sent data.
 \param mixer Audio mixer instance
 \param port Input port number
 \return Time in seconds since last send. -1 indicates no data has ever been sent or received.
*/
VATLIB_API float Vat_GetLastInputTime(
    VatAudioMixer *mixer,
    int port);

/*!
 These routines tell whether an output port has
 received data.
 \param mixer Audio mixer instance
 \param port Output port number
 \return Time in seconds since last send. -1 indicates no data has ever been sent or received.
*/
VATLIB_API float Vat_GetLastOutputTime(
    VatAudioMixer *mixer,
    int port);

#ifdef __cplusplus
}
#endif

#endif

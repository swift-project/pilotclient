// Wind structure (size 16 bytes)
typedef struct _NewWind
{	unsigned short UpperAlt;	// Metres
	unsigned short Speed;		// Knots
	unsigned short Gust;		// Max gust speed difference, knots
	unsigned short Direction;	// usual 65536 = 360 units
	unsigned char  Turbulence;	// 0-4
	unsigned char  Shear;		// 0-3
	unsigned short Variance;	// direction variability, same units as Direction
	unsigned short SpeedFract;	// 1/65536ths of Knots, for more accurate vector calcs
	unsigned short GapAbove; // Metres
		 //	This is the gap from the top of surface altitude to first layer's base
	   // GapAbove is ignored in layers other than the surface layer, and needs
	   // FSUIC 4.748 or 3.998k minimum. 
} NewWind;

// Visibility structure (size 8 bytes)
typedef struct _NewVis
{	unsigned short UpperAlt;	// Metres
	signed short   LowerAlt;	// Metres
	unsigned short Range;		// in 1/100ths sm
	unsigned short Spare;
} NewVis;

// Cloud structure (size 16 bytes)
typedef struct _NewCloud
{	unsigned short UpperAlt;	// Metres
	unsigned short LowerAlt;	// Metres
	unsigned short Deviation;	// Metres
	unsigned char  Coverage;	// Octas, 0-8
	unsigned char  Type;		// 1-10
	unsigned char  Turbulence;	// 0-4
	unsigned char  Icing;		// 0-4
	signed short   PrecipBase;	// Metres
	unsigned char  PrecipType;	// 0-2
	unsigned char  PrecipRate;	// 0-5
	unsigned char  TopShape;	// ?
	unsigned char  Spare;
} NewCloud;

// Temperature structure (size 8 bytes)
typedef struct _NewTemp
{	unsigned short Alt;		// Metres
	short Day;			// Degrees C
	short DayNightVar;		// Degrees C
	short DewPoint;			// Degrees C
} NewTemp;

// Pressure structure (size 4 bytes)
typedef struct _NewPress
{	unsigned short Pressure;	// 16 x mb
	short Drift;			// ? Maybe
} NewPress;

// Structure mapped to FSUIPC offsets
// C000, C400, C800 and CC00
typedef struct _NewWeather
{	unsigned short uCommand;	// C000 C400 C800 CC00
	unsigned short uFlags;		// C002 C402 C802 CC02 // Not used until FSX - see below
	unsigned int ulSignature;	// C004 C404 C804 CC04
	char chICAO[4];				// C008 C408 C808 CC08
	unsigned short uDynamics;	// C00C C40C C80C CC0C // 0=none, 4=extreme
	unsigned short uSeconds;	// C00E C40E C80E CC0E // FSX only -- see below
	double dLatitude;			// C010 C410 C810 CC10 // LLA zero for GLOB or unknown ICAO
	double dLongitude;			// C018 C418 C818 CC18
	int nElevation;				// C020 C420 C820 CC20 // metres * 65536
	unsigned int ulTimeStamp;	// C024 C424 C824 CC24 // mSecs since start of session
	NewPress Press;				// C028 C428 C828 CC28
	NewVis Vis;					// C02C C42C C82C CC2C // Base Vis -- for upper Vis layers see 
	int nTempCtr;				// C034 C434 C834 CC34 // Number of temperature layers
	NewTemp Temp[24];			// C038 C438 C838 CC38
	int nWindsCtr;				// C0F8 C4F8 C8F8 CCF8 // Number of wind layers
	NewWind Wind[24];			// C0FC C4FC C8FC CCFC
	int nCloudsCtr;				// C27C C67C CA7C CE7C // Number of Cloud layers (max 16 in FSX, was 24
	NewCloud Cloud[16];			// C280 C680 CA80 CE80
	char chUTCstamp[6];			// C380 C780 CB80 CF80 // UTC time stamp on METAR (internal use)
	unsigned short fWriteback;	// C386 C786 CB86 CF86 // Flags for writing back to FSX (internal use only)
	unsigned long ulSettingTimeStamp; // C388 c788 CB88 CF88 // Timestamp METAR sent (internal use)
	int nUpperVisCtr;			// C38C C78C CB8C CF8C // Number of upper Vis layer (new in FSX)
	NewVis UpperVis[12];		// C390 C790 CB90 CF90
	unsigned short uSpare[8];	// C3F0	C7F0 CBF0 CFF0 
} NewWeather;

#define MAXVISLAYERS 12
#define MAXTEMPLAYERS 24
#define MAXWINDLAYERS 24
#define MAXCLOUDLAYERS 16

// Commands (for set weather area only)
#define NW_SET	1	// Set weather via FSUIPC user filters
#define NW_SETEXACT 2	// Set weather bypassing user filters
#define NW_CLEAR 3	// Clear all weather, but leave dynamic setting alone
#define NW_DYNAMICS 4	// Set weather dynamics (from uDynamics value)
#define NW_GLOBAL 5 // (FSX) Put FS into Global weather mode
// You sohuld write only 'GLOB' weather after this, which will be applied everywhere.

#define NW_METAR 128 // DON'T USE! (Internal setting when weather written via string at B000. (FSX))

#define NW_SET_PENDING 257 // Set weather using filters, but don't activate in FS yet
#define NW_SETEXACT_PENDING 258 // Set weather bypassing filters, but don't activate in FS yet

#define NW_ACTIVATE 256 // Activate pending weather settings

// Flags for FSX weather setting
#define NWF_SECONDS 1
/* Set this flag if "Seconds" value is to be used in setting weather in FSX
   This applies to weather set via NWI values or via the NEXT Metar string
   written to B000. It's a "one shot" flag, cleared after use. 

   The Seconds value determines whether the weather change is effective
   immediately or after a delay, gradually "blending" into the existing
   weather.
*/

// The 4 distinct areas are used as follows:
//		C000 - read only, maintains current interpolated weather at aircraft
//		C400 - read only, maintains last written global weather values
//		C800 - write area to set weather according to Command, flags, etc etc.
//		CC00 - selected read (via signature + ICAO), reads weather at weather station or GLOB


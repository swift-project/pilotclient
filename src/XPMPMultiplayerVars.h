/*
 * Copyright (c) 2005, Ben Supnik and Chris Serio.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef XPLMMULTIPLAYERVARS_H
#define XPLMMULTIPLAYERVARS_H

/*
 * This file contains the various internal definitions
 * and globals for the model rendering code. 
 *
 */

#include <vector>
#include <set>
#include <string>
#include <map>

#include "XObjDefs.h"

using namespace std;

#include "XPMPMultiplayer.h"
#include "XPMPMultiplayerObj8.h"	// for obj8 attachment info

template <class T>
inline
const T&
XPMP_TMIN(const T& a, const T& b)
{
	return b < a ? b : a;
}

template <class T>
inline
const T&
XPMP_TMAX(const T& a, const T& b)
{
	return a < b ? b : a;
}


const	double	kFtToMeters = 0.3048;
const	double	kMaxDistTCAS = 40.0 * 6080.0 * kFtToMeters;


/****************** MODEL MATCHING CRAP ***************/

enum {
	plane_Austin,
	plane_Obj,
	plane_Lights,
	plane_Obj8,
	plane_Count
};

// This plane struct represents one model in one CSL packpage.
// It has a type, a single file path for whatever we have to load,
// and then implementation-specifc stuff.
struct	CSLPlane_t {

    string                      modelName;       // Unique model name
    string                      icao;           // Icao type of this model
    string                      airline;        // Airline identifier. Can be empty.
    string                      livery;         // Livery identifier. Can be empty.

	int							plane_type;		// What kind are we?
	string						file_path;		// Where do we load from (oz and obj, debug-use-only for OBJ8)
	bool						moving_gear;	// Does gear retract?

	// plane_Austin
	int							austin_idx;

	// plane_Obj
	int							obj_idx;
	int							texID;			// can be 0 for no customization
	int							texLitID;		// can be 0 for no customization

	// plane_Obj8
	vector<obj_for_acf>			attachments;
	
};

// These enums define the six levels of matching we might possibly
// make.  For each level of matching, we use a single string as a key.
// (The string's contents vary with model - examples are shown.)
enum {
	match_icao_airline_livery = 0,		//	B738 SWA SHAMU
	match_icao_airline,					//	B738 SWA
	match_group_airline_livery,			//	B731 B732 B733 B734 B735 B736 B737 B738 B739 SWA SHAMU
	match_group_airline,				//	B731 B732 B733 B734 B735 B736 B737 B738 B739 SWA
	match_icao,							//	B738
	match_group,						//	B731 B732 B733 B734 B735 B736 B737 B738 B739
	match_count
};

// A CSL package - a vector of planes and six maps from the above matching 
// keys to the internal index of the plane.
struct	CSLPackage_t {

    bool hasValidHeader() const
    {
        return !name.empty() && !path.empty();
    }

	string						name;
    string                      path;
	vector<CSLPlane_t>			planes;
	map<string, int>			matches[match_count];

};

extern vector<CSLPackage_t>		gPackages;

extern map<string, string>		gGroupings;

/**************** Model matching using ICAO doc 8643
		(http://www.icao.int/anb/ais/TxtFiles/Doc8643.txt) ***********/

struct CSLAircraftCode_t {
	string				icao;		// aircraft ICAO code
	string				equip;		// equipment code (L1T, L2J etc)
	char				category;	// L, M, H, V (vertical = helo)
};

extern map<string, CSLAircraftCode_t>	gAircraftCodes;

/**************** PLANE OBJECTS ********************/

// This plane struct reprents one instance of a 
// multiplayer plane.
struct	XPMPPlane_t {

	// Modeling properties
	string					icao;
	string					airline;
	string					livery;
    CSLPlane_t *			model = nullptr; // May be null if no good match
	bool					good_livery;	// is our paint correctly matched?
	
	// This callback is used to pull data from the client for posiitons, etc.
	XPMPPlaneData_f			dataFunc;
    void *					ref = nullptr;
	
	// This is last known data we got for the plane, with timestamps.
	int						posAge;
	XPMPPlanePosition_t		pos;
	int						surfaceAge;
	XPMPPlaneSurfaces_t		surface;
	int						radarAge;
	XPMPPlaneRadar_t		radar;
	
};

typedef	XPMPPlane_t *								XPMPPlanePtr;
typedef	vector<XPMPPlanePtr>						XPMPPlaneVector;

// Notifiers - clients can install callbacks and be told when a plane's
// data changes.
typedef	pair<XPMPPlaneNotifier_f, void *>			XPMPPlaneNotifierPair;
typedef	pair<XPMPPlaneNotifierPair, XPLMPluginID>	XPMPPlaneNotifierTripple;
typedef	vector<XPMPPlaneNotifierTripple>			XPMPPlaneNotifierVector;

// Prefs funcs - the client provides callbacks to pull ini key values 
// for various functioning.

extern int			(* gIntPrefsFunc)(const char *, const char *, int);
extern float		(* gFloatPrefsFunc)(const char *, const char *, float);

extern XPMPPlaneVector					gPlanes;				// All planes
extern XPMPPlaneNotifierVector			gObservers;				// All notifiers
extern XPMPRenderPlanes_f				gRenderer;				// The actual rendering func
extern void *							gRendererRef;			// The actual rendering func
extern int								gDumpOneRenderCycle;	// Debug
extern int 								gEnableCount;			// Hack - see TCAS support

extern string							gDefaultPlane;			// ICAO of default plane

#endif

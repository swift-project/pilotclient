/*
 * Copyright (c) 2004, Ben Supnik and Chris Serio.
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

#include "XPMPPlaneRenderer.h"
#include "XPMPMultiplayer.h"
#include "XPMPMultiplayerCSL.h"
#include "XPMPMultiplayerVars.h"
#include "XPMPMultiplayerObj.h"

#include "XPLMGraphics.h"
#include "XPLMDisplay.h"
#include "XPLMCamera.h"
#include "XPLMPlanes.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"

#include <stdio.h>
#include <math.h>

#if IBM
#include <GL/gl.h>
#elif APL
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <vector>
#include <string>
#include <set>
#include <map>

// Turn this on to get a lot of diagnostic info on who's visible, etc.
#define 	DEBUG_RENDERER 0
// Turn this on to put rendering stats in datarefs for realtime observatoin.
#define		RENDERER_STATS 0

// Maximum altitude difference in feet for TCAS blips
#define		MAX_TCAS_ALTDIFF		5000

std::vector<XPLMDataRef>			gMultiRef_X;
std::vector<XPLMDataRef>			gMultiRef_Y;
std::vector<XPLMDataRef>			gMultiRef_Z;

/* UTILITY FUNCTIONS */

static	inline double sqr(double a) { return a * a; }
static	inline double CalcDist3D(double x1, double y1, double z1, double x2, double y2, double z2)
{
	return sqrt(sqr(x2-x1) + sqr(y2-y1) + sqr(z2-z1));
}


static	inline double	CalcAngle(double dy, double dx)
{
	double angle;
	if (dx == 0.0)
		angle = (dy > 0.0) ? 0.0 : 180.0;
	else 
		angle = 90.0 - (atan(dy/dx) * 180.0 / 3.14159265);
	if (dx < 0.0)
		angle += 180.0;
	return angle;
}

static	inline double	DiffAngle(double a1, double a2)
{
	double diff = (a2 - a1);
	if (diff >= 180.0)
		diff -= 360.0;
	if (diff <= -180.0)
		diff += 360.0;
	return fabs(diff);
}


#if RENDERER_STATS

static int		GetRendererStat(void * inRefcon)
{
	return *((int *) inRefcon);
}

#endif

static	int		gTotPlanes = 0;			// Counters
static	int		gACFPlanes = 0;			// Number of Austin's planes we drew in full
static	int		gNavPlanes = 0;			// Number of Austin's planes we drew with lights only
static	int		gOBJPlanes = 0;			// Number of our OBJ planes we drew in full
static	int		gHackFOVLast = 0;

static	XPLMDataRef		gFOVDataRef = NULL;		// Current FOV for culling.
static	XPLMDataRef		gVisDataRef = NULL;		// Current air visiblity for culling.
static	XPLMDataRef		gAltitudeRef = NULL;	// Current aircraft altitude (for TCAS)
static	float			gOverRes = 1.0;			// Wide-screen support.  (May be messed up.)


void			XPMPInitDefaultPlaneRenderer(void)
{
	// SETUP - mostly just fetch datarefs.
	int	width;
	XPLMGetScreenSize(&width, NULL);
	gOverRes = ((double) width) / 1024.0;

	gFOVDataRef = XPLMFindDataRef("sim/graphics/view/field_of_view_deg");
	if (gFOVDataRef == NULL)
		XPLMDebugString("WARNING: Default renderer could not find FOV data in the sim.\n");
	gVisDataRef = XPLMFindDataRef("sim/graphics/view/visibility_effective_m");
	if (gVisDataRef == NULL) gVisDataRef = XPLMFindDataRef("sim/weather/visibility_effective_m");
	if (gVisDataRef == NULL)
		XPLMDebugString("WARNING: Default renderer could not find effective visibility in the sim.\n");

	if(gAltitudeRef == NULL) gAltitudeRef = XPLMFindDataRef("sim/flightmodel/position/elevation");

#if RENDERER_STATS
	XPLMRegisterDataAccessor("hack/renderer/planes", xplmType_Int, 0, GetRendererStat, NULL, 
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				&gTotPlanes, NULL);
	XPLMRegisterDataAccessor("hack/renderer/navlites", xplmType_Int, 0, GetRendererStat, NULL, 
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
				&gNavPlanes, NULL);
	XPLMRegisterDataAccessor("hack/renderer/objects", xplmType_Int, 0, GetRendererStat, NULL, 
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,	
				&gOBJPlanes, NULL);
	XPLMRegisterDataAccessor("hack/renderer/acfs", xplmType_Int, 0, GetRendererStat, NULL, 
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,	
				&gACFPlanes, NULL);
	XPLMRegisterDataAccessor("hack/renderer/fov", xplmType_Int, 0, GetRendererStat, NULL, 
				NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,	
				&gHackFOVLast, NULL);
#endif		

	// We don't know how many multiplayer planes there are - fetch as many as we can.

	int 		n = 1;
	char		buf[100];
	XPLMDataRef	d;
	while (1)
	{
		sprintf(buf,"sim/multiplayer/position/plane%d_x", n);
		d = XPLMFindDataRef(buf);
		if (!d) break;
		gMultiRef_X.push_back(d);
		sprintf(buf,"sim/multiplayer/position/plane%d_y", n);
		d = XPLMFindDataRef(buf);
		if (!d) break;
		gMultiRef_Y.push_back(d);
		sprintf(buf,"sim/multiplayer/position/plane%d_z", n);
		d = XPLMFindDataRef(buf);
		if (!d) break;
		gMultiRef_Z.push_back(d);
		++n;
	}	
}

// PlaneToRender struct: we prioritize planes radially by distance, so...
// we use this struct to remember one visible plane.  Once we've
// found all visible planes, we draw the closest ones.

struct	PlaneToRender_t {
	float					x;			// Positional info
	float					y;
	float					z;
	float					pitch;
	float					heading;
	float					roll;
	CSLPlane_t *			model;		// What model are we?
	bool					full;		// Do we need to draw the full plane or just lites?
	bool					cull;		// Are we visible on screen?
	bool					tcas;		// Are we visible on TCAS?
	XPLMPlaneDrawState_t	state;		// Flaps, gear, etc.
	float					dist;
	xpmp_LightStatus		lights;		// lights status
};
typedef	std::map<float, PlaneToRender_t>	RenderMap;


void			XPMPDefaultPlaneRenderer(void)
{
	long	planeCount = XPMPCountPlanes();
#if DEBUG_RENDERER
	char	buf[50];
	sprintf(buf,"Renderer Planes: %d\n", planeCount);
	XPLMDebugString(buf);
#endif
	if (planeCount == 0)		// Quick exit if no one's around.
	{
		if (gDumpOneRenderCycle)
		{
			gDumpOneRenderCycle = false;
			XPLMDebugString("No planes this cycle.\n");
		}
		return;
	}
	
	if (gFOVDataRef == NULL)	// No FOV - we're probably totally screwed up.
		return;

	// Culling - read the camera pos«and figure out what's visible.

	XPLMCameraPosition_t		cameraPos;
	double							fov;
	
	XPLMReadCameraPosition(&cameraPos);
	fov = XPLMGetDataf(gFOVDataRef);
	double maxDist = XPLMGetDataf(gVisDataRef);
	
	double	kFullPlaneDist = gOverRes * (5280.0 / 3.2) * (gFloatPrefsFunc ? gFloatPrefsFunc("planes","full_distance", 3.0) : 3.0);	// Only draw planes fully within 3 miles.
	int		kMaxFullPlanes = gIntPrefsFunc ? gIntPrefsFunc("planes","max_full_count", 100) : 100;						// Draw no more than 100 full planes!
	
	// Field of view modification.  We don't yet handle camera roll because I'm too stupid
	// to do the math.  (I took linear algebra but came out of the course knowing less than
	// when I went in.  So instead we...assume that our field of view is vertical, apply
	// a 5:3 aspect ratio to widen it and a 10% fudge factor to account for airplanes
	// not being point objects.  The 5:3 is based on allowing for the worst case, that
	// we're rolled to make a perfect diagonal making our monitor as wide as possible.d
	// We divide by camera zoom so that when we're zoomed in we're not taking people
	// that have been zoomed out of our peripheral vision.
	fov = fov * 5.0 / 4.0 / cameraPos.zoom;

	gTotPlanes = planeCount;	
	gNavPlanes = gACFPlanes = gOBJPlanes = 0;

	int modelCount, active, plugin, tcas;
	XPLMCountAircraft(&modelCount, &active, &plugin);
	tcas = modelCount - 1;	// This is how many tcas blips we can have!
		
	RenderMap	myPlanes;
	
	/************************************************************************************
	 * CULLING AND STATE CALCULATION LOOP
	 ************************************************************************************/	
	
	if (gDumpOneRenderCycle)
	{
		XPLMDebugString("Dumping one cycle map of planes.\n");	
		char	fname[256], bigbuf[1024], foo[32];
		for (int n = 1; n < modelCount; ++n)
		{
			XPLMGetNthAircraftModel(n, fname, bigbuf);
			sprintf(foo, " [%d] - ", n);
			XPLMDebugString(foo);
			XPLMDebugString(fname);
			XPLMDebugString(" - ");
			XPLMDebugString(bigbuf);
			XPLMDebugString("\n");
		}
	}
	
	// Go through every plane.  We're going to figure out if it is visible and if so remember it for drawing later.
	for (long index = 0; index < planeCount; ++index)
	{
		XPMPPlaneID id = XPMPGetNthPlane(index);
		
		XPMPPlanePosition_t	pos;
		pos.size = sizeof(pos);

		if (XPMPGetPlaneData(id, xpmpDataType_Position, &pos) != xpmpData_Unavailable)
		{
			// First figure out where the plane is!

			double	x,y,z;
			XPLMWorldToLocal(pos.lat, pos.lon, pos.elevation * kFtToMeters, &x, &y, &z);
			
			double	distMeters = CalcDist3D(x,y,z,cameraPos.x, cameraPos.y, cameraPos.z);
			if (cameraPos.zoom != 0.0)
				distMeters /= cameraPos.zoom;	// If we're 2x zoomed, pretend the AC is half as far away.
			
			// If the plane is farther than our TCAS range, it's just not visible.  Drop it!
			if (distMeters > kMaxDistTCAS)
				continue;
				
			// Only draw if it's in range.
			bool cull = (distMeters > maxDist);
			
			XPMPPlaneRadar_t radar;
			radar.size = sizeof(radar);
			bool tcas = true;
			if (XPMPGetPlaneData(id, xpmpDataType_Radar, &radar) != xpmpData_Unavailable)
				if (radar.mode == xpmpTransponderMode_Standby)
					tcas = false;

			// check for altitude - if difference exceeds 3000ft, don't show
			double acft_alt = XPLMGetDatad(gAltitudeRef) / kFtToMeters;
			double alt_diff = pos.elevation - acft_alt;
			if(alt_diff < 0) alt_diff *= -1;
			if(alt_diff > MAX_TCAS_ALTDIFF) tcas = false;

			// Calculate the heading from the camera to the target (hor, vert).
			// Calculate the angles between the camera angles and the real angles.
			// Cull if we exceed half the FOV.
			
			double	headingToTarget = CalcAngle(cameraPos.z - z, x - cameraPos.x);
			double	pitchToTarget = CalcAngle(sqrt(sqr(x - cameraPos.x) + sqr(cameraPos.z - z)), y - cameraPos.y);
			
			double	headOff = DiffAngle(headingToTarget, cameraPos.heading);
			double	pitchOff = DiffAngle(pitchToTarget,cameraPos.pitch);

#if DEBUG_RENDERER
			char	cullBuf[1024];
			sprintf(cullBuf, "Target Pos = %f,%f,%f, Camera Pos = %f,%f,%f.\n",
				x,y,z,cameraPos.x,cameraPos.y,cameraPos.z);				
			XPLMDebugString(cullBuf);
			sprintf(cullBuf, "Camera p=%f,h=%f, object p=%f,h=%f,diff p=%f,h=%f.\n",
				cameraPos.pitch, cameraPos.heading, pitchToTarget, headingToTarget, pitchOff, headOff);
			XPLMDebugString(cullBuf);
#endif			
			
			
			float fov_fudged = fov + atan(200.0 / distMeters) * 180.0 / 3.141592565;
#if RENDERER_STATS			
			gHackFOVLast = fov_fudged;
#endif			
			// View frustum check - if we're off screen, we don't draw.  But remember us - we may be visible on TCAS.
			if ((headOff > (fov_fudged / 2.0)) ||
				(pitchOff > (fov_fudged / 2.0)))
			{
				cull = true;
			}
			
			// Full plane or lites based on distance.
			bool	drawFullPlane = (distMeters < kFullPlaneDist);
			
#if DEBUG_RENDERER

			char	icao[128], livery[128];
			char	debug[512];

			XPMPGetPlaneICAOAndLivery(id, icao, livery);						
			sprintf(debug,"Queueing plane %d (%s/%s) at lle %f, %f, %f (xyz=%f, %f, %f) pitch=%f,roll=%f,heading=%f,model=1.\n", index, icao, livery,
					pos.lat, pos.lon, pos.elevation,
				x, y, z, pos.pitch, pos.roll, pos.heading);
			XPLMDebugString(debug);
#endif

			// Stash one render record with the plane's position, etc.
			{
				PlaneToRender_t		renderRecord;
				renderRecord.x = x;
				renderRecord.y = y;
				renderRecord.z = z;
				renderRecord.pitch = pos.pitch;
				renderRecord.heading = pos.heading;
				renderRecord.roll = pos.roll;
				renderRecord.model=((XPMPPlanePtr)id)->model;
				renderRecord.cull = cull;						// NO other planes.  Doing so causes a lot of things to go nuts!
				renderRecord.tcas = tcas;	

				XPMPPlaneSurfaces_t	surfaces;
				surfaces.size = sizeof(surfaces);
				if (XPMPGetPlaneData(id, xpmpDataType_Surfaces, &surfaces) != xpmpData_Unavailable)
				{
					renderRecord.state.structSize = sizeof(renderRecord.state);
					renderRecord.state.gearPosition 	= surfaces.gearPosition 	;
					renderRecord.state.flapRatio 		= surfaces.flapRatio 		;
					renderRecord.state.spoilerRatio 	= surfaces.spoilerRatio 	;
					renderRecord.state.speedBrakeRatio 	= surfaces.speedBrakeRatio 	;
					renderRecord.state.slatRatio 		= surfaces.slatRatio 		;
					renderRecord.state.wingSweep 		= surfaces.wingSweep 		;
					renderRecord.state.thrust 			= surfaces.thrust 			;
					renderRecord.state.yolkPitch 		= surfaces.yolkPitch 		;
					renderRecord.state.yolkHeading 		= surfaces.yolkHeading 		;
					renderRecord.state.yolkRoll 		= surfaces.yolkRoll 		;

					renderRecord.lights.lightFlags		= surfaces.lights.lightFlags;
				
				} else {
					renderRecord.state.structSize = sizeof(renderRecord.state);
					renderRecord.state.gearPosition = (pos.elevation < 70) ?  1.0 : 0.0;
					renderRecord.state.flapRatio = (pos.elevation < 70) ? 1.0 : 0.0;
					renderRecord.state.spoilerRatio = renderRecord.state.speedBrakeRatio = renderRecord.state.slatRatio = renderRecord.state.wingSweep = 0.0;
					renderRecord.state.thrust = (pos.pitch > 30) ? 1.0 : 0.6;
					renderRecord.state.yolkPitch = pos.pitch / 90.0;
					renderRecord.state.yolkHeading = pos.heading / 180.0;
					renderRecord.state.yolkRoll = pos.roll / 90.0;	

					// use some smart defaults
					renderRecord.lights.bcnLights = 1;
					renderRecord.lights.navLights = 1;

				}
				if (renderRecord.model && !renderRecord.model->moving_gear)
					renderRecord.state.gearPosition = 1.0;
				renderRecord.full = drawFullPlane;	
				renderRecord.dist = distMeters;
				
				myPlanes.insert(RenderMap::value_type(distMeters, renderRecord));

			} // State calculation
			
		} // Plane has data available
		
	} // Per-plane loop

	if (gDumpOneRenderCycle)
		XPLMDebugString("End of cycle dump.\n");	
	
	/************************************************************************************
	 * ACTUAL RENDERING LOOP
	 ************************************************************************************/
	
	// We're going to go in and render the first N planes in full, and the rest as lites.
	// We're also going to put the x-plane multiplayer vars in place for the first N 
	// TCAS-visible planes, so they show up on our moving map.
	// We do this in two stages: building up what to do, then doing it in the optimal
	// OGL order.
	
		int	renderedCounter = 0;
		
	vector<PlaneToRender_t *>			planes_obj_lites;
	multimap<int, PlaneToRender_t *>	planes_austin;
	multimap<int, PlaneToRender_t *>	planes_obj;

	vector<PlaneToRender_t *>::iterator			planeIter;
	multimap<int, PlaneToRender_t *>::iterator	planeMapIter;

	// In our first iteration pass we'll go through all planes and handle TCAS, draw planes that have no
	// CSL model, and put CSL planes in the right 'bucket'.

	for (RenderMap::iterator iter = myPlanes.begin(); iter != myPlanes.end(); ++iter)
	{	
		// This is the case where we draw a real plane.
		if (!iter->second.cull)
		{	
			// Max plane enforcement - once we run out of the max number of full planes the 
			// user allows, force only lites for framerate
			if (gACFPlanes >= kMaxFullPlanes)
				iter->second.full = false;

#if DEBUG_RENDERER	
			char	debug[512];
			sprintf(debug,"Drawing plane: %s at %f,%f,%f (%fx%fx%f full=%d\n",
				iter->second.model ? iter->second.model->file_path.c_str() : "<none>", iter->second.x, iter->second.y, iter->second.z,
				iter->second.pitch, iter->second.roll, iter->second.heading, iter->second.full ? 1 : 0);
			XPLMDebugString(debug);
#endif

			if (iter->second.model)
			{
				if (iter->second.model->plane_type == plane_Austin)
				{
					planes_austin.insert(multimap<int, PlaneToRender_t *>::value_type(CSL_GetOGLIndex(iter->second.model), &iter->second));
				} 
				else if (iter->second.model->plane_type == plane_Obj)
				{
					planes_obj.insert(multimap<int, PlaneToRender_t *>::value_type(CSL_GetOGLIndex(iter->second.model), &iter->second));
					planes_obj_lites.push_back(&iter->second);					
				}
			
			} else {
				// If it's time to draw austin's planes but this one
				// doesn't have a model, we draw anything.
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(iter->second.x, iter->second.y, iter->second.z);			
				glRotatef(iter->second.heading, 0.0, -1.0, 0.0);
				glRotatef(iter->second.pitch, 01.0, 0.0, 0.0);
				glRotatef(iter->second.roll, 0.0, 0.0, -1.0);

				// Safety check - if plane 1 isn't even loaded do NOT draw, do NOT draw plane 0.
				// Using the user's planes can cause the internal flight model to get f-cked up.
				// Using a non-loaded plane can trigger internal asserts in x-plane.
				if (modelCount > 1)
					XPLMDrawAircraft(1,
					(float) iter->second.x, (float) iter->second.y, (float) iter->second.z, 
					iter->second.pitch, iter->second.roll, iter->second.heading, 
					iter->second.full ? 1 : 0, &iter->second.state);

				glPopMatrix();
			}

		}

		// TCAS handling - if the plane needs to be drawn on TCAS and we haven't yet, move one of Austin's planes.
		if (iter->second.tcas && renderedCounter < gMultiRef_X.size())
		{
			XPLMSetDataf(gMultiRef_X[renderedCounter], iter->second.x);
			XPLMSetDataf(gMultiRef_Y[renderedCounter], iter->second.y);
			XPLMSetDataf(gMultiRef_Z[renderedCounter], iter->second.z);
			++renderedCounter;
		}
	}
	
	// PASS 1 - draw Austin's planes.

	for (planeMapIter = planes_austin.begin(); planeMapIter != planes_austin.end(); ++planeMapIter)	
	{
		CSL_DrawObject(	planeMapIter->second->model, 
						planeMapIter->second->dist,
						planeMapIter->second->x, 
						planeMapIter->second->y, 
						planeMapIter->second->z, 
						planeMapIter->second->pitch, 
						planeMapIter->second->roll, 
						planeMapIter->second->heading, 
						plane_Austin, 
						planeMapIter->second->full ? 1 : 0, 
						planeMapIter->second->lights, 
						&planeMapIter->second->state);

		if (planeMapIter->second->full)
			++gACFPlanes;
		else
			++gNavPlanes;
	}
	
	// PASS 2 - draw OBJs
	
	for (planeMapIter = planes_obj.begin(); planeMapIter != planes_obj.end(); ++planeMapIter)	
	{	
		CSL_DrawObject(
					planeMapIter->second->model, 
					planeMapIter->second->dist,
					planeMapIter->second->x, 
					planeMapIter->second->y, 
					planeMapIter->second->z, 
					planeMapIter->second->pitch, 
					planeMapIter->second->roll, 
					planeMapIter->second->heading, 
					plane_Obj, 
					planeMapIter->second->full ? 1 : 0, 
					planeMapIter->second->lights, 
				   &planeMapIter->second->state);	
			++gOBJPlanes;
	}

	// PASS 3 - draw OBJ lights.

	if (!planes_obj_lites.empty())
	{
		OBJ_BeginLightDrawing();
		for (planeIter = planes_obj_lites.begin(); planeIter != planes_obj_lites.end(); ++planeIter)
		{
			// this thing draws the lights of a model
			CSL_DrawObject( (*planeIter)->model, 
							(*planeIter)->dist,
							(*planeIter)->x,
							(*planeIter)->y,
							(*planeIter)->z,
							(*planeIter)->pitch,
							(*planeIter)->roll,
							(*planeIter)->heading,
							plane_Lights,
							(*planeIter)->full ? 1 : 0,
							(*planeIter)->lights,
						   &(*planeIter)->state);
		}
	}
	
	// Final hack - leave a note to ourselves for how many of Austin's planes we relocated to do TCAS.
	if (tcas > renderedCounter)	
		tcas = renderedCounter;
	gEnableCount = (tcas+1);
	
	gDumpOneRenderCycle = 0;
}

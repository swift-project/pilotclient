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

// Even in good weather we don't want labels on things
// that we can barely see.  Cut labels at 5 km.
#define		MAX_LABEL_DIST			5000.0


std::vector<XPLMDataRef>			gMultiRef_X;
std::vector<XPLMDataRef>			gMultiRef_Y;
std::vector<XPLMDataRef>			gMultiRef_Z;

bool gDrawLabels = true;

struct cull_info_t {					// This struct has everything we need to cull fast!
	float	model_view[16];				// The model view matrix, to get from local OpenGL to eye coordinates.
	float	proj[16];					// Proj matrix - this is just a hack to use for gluProject.
	float	nea_clip[4];				// Four clip planes in the form of Ax + By + Cz + D = 0 (ABCD are in the array.)
	float	far_clip[4];				// They are oriented so the positive side of the clip plane is INSIDE the view volume.
	float	lft_clip[4];
	float	rgt_clip[4];
	float	bot_clip[4];
	float	top_clip[4];
};

static void setup_cull_info(cull_info_t * i)
{
	// First, just read out the current OpenGL matrices...do this once at setup because it's not the fastest thing to do.
	glGetFloatv(GL_MODELVIEW_MATRIX ,i->model_view);
	glGetFloatv(GL_PROJECTION_MATRIX,i->proj);
	
	// Now...what the heck is this?  Here's the deal: the clip planes have values in "clip" coordinates of: Left = (1,0,0,1)
	// Right = (-1,0,0,1), Bottom = (0,1,0,1), etc.  (Clip coordinates are coordinates from -1 to 1 in XYZ that the driver
	// uses.  The projection matrix converts from eye to clip coordinates.)
	//
	// How do we convert a plane backward from clip to eye coordinates?  Well, we need the transpose of the inverse of the
	// inverse of the projection matrix.  (Transpose of the inverse is needed to transform a plane, and the inverse of the
	// projection is the matrix that goes clip -> eye.)  Well, that cancels out to the transpose of the projection matrix,
	// which is nice because it means we don't need a matrix inversion in this bit of sample code.
	
	// So this nightmare down here is simply:
	// clip plane * transpose (proj_matrix)
	// worked out for all six clip planes.  If you squint you can see the patterns:
	// L:  1  0 0 1
	// R: -1  0 0 1
	// B:  0  1 0 1
	// T:  0 -1 0 1
	// etc.
	
	i->lft_clip[0] = i->proj[0]+i->proj[3];	i->lft_clip[1] = i->proj[4]+i->proj[7];	i->lft_clip[2] = i->proj[8]+i->proj[11];	i->lft_clip[3] = i->proj[12]+i->proj[15];
	i->rgt_clip[0] =-i->proj[0]+i->proj[3];	i->rgt_clip[1] =-i->proj[4]+i->proj[7];	i->rgt_clip[2] =-i->proj[8]+i->proj[11];	i->rgt_clip[3] =-i->proj[12]+i->proj[15];
	
	i->bot_clip[0] = i->proj[1]+i->proj[3];	i->bot_clip[1] = i->proj[5]+i->proj[7];	i->bot_clip[2] = i->proj[9]+i->proj[11];	i->bot_clip[3] = i->proj[13]+i->proj[15];
	i->top_clip[0] =-i->proj[1]+i->proj[3];	i->top_clip[1] =-i->proj[5]+i->proj[7];	i->top_clip[2] =-i->proj[9]+i->proj[11];	i->top_clip[3] =-i->proj[13]+i->proj[15];

	i->nea_clip[0] = i->proj[2]+i->proj[3];	i->nea_clip[1] = i->proj[6]+i->proj[7];	i->nea_clip[2] = i->proj[10]+i->proj[11];	i->nea_clip[3] = i->proj[14]+i->proj[15];
	i->far_clip[0] =-i->proj[2]+i->proj[3];	i->far_clip[1] =-i->proj[6]+i->proj[7];	i->far_clip[2] =-i->proj[10]+i->proj[11];	i->far_clip[3] =-i->proj[14]+i->proj[15];
}

static int sphere_is_visible(const cull_info_t * i, float x, float y, float z, float r)
{
	// First: we transform our coordinate into eye coordinates from model-view.
	float xp = x * i->model_view[0] + y * i->model_view[4] + z * i->model_view[ 8] + i->model_view[12];
	float yp = x * i->model_view[1] + y * i->model_view[5] + z * i->model_view[ 9] + i->model_view[13];
	float zp = x * i->model_view[2] + y * i->model_view[6] + z * i->model_view[10] + i->model_view[14];

	// Now - we apply the "plane equation" of each clip plane to see how far from the clip plane our point is.  
	// The clip planes are directed: positive number distances mean we are INSIDE our viewing area by some distance;
	// negative means outside.  So ... if we are outside by less than -r, the ENTIRE sphere is out of bounds.
	// We are not visible!  We do the near clip plane, then sides, then far, in an attempt to try the planes
	// that will eliminate the most geometry first...half the world is behind the near clip plane, but not much is
	// behind the far clip plane on sunny day.
	if ((xp * i->nea_clip[0] + yp * i->nea_clip[1] + zp * i->nea_clip[2] + i->nea_clip[3] + r) < 0)	return false;
	if ((xp * i->bot_clip[0] + yp * i->bot_clip[1] + zp * i->bot_clip[2] + i->bot_clip[3] + r) < 0)	return false;
	if ((xp * i->top_clip[0] + yp * i->top_clip[1] + zp * i->top_clip[2] + i->top_clip[3] + r) < 0)	return false;
	if ((xp * i->lft_clip[0] + yp * i->lft_clip[1] + zp * i->lft_clip[2] + i->lft_clip[3] + r) < 0)	return false;
	if ((xp * i->rgt_clip[0] + yp * i->rgt_clip[1] + zp * i->rgt_clip[2] + i->rgt_clip[3] + r) < 0)	return false;
	if ((xp * i->far_clip[0] + yp * i->far_clip[1] + zp * i->far_clip[2] + i->far_clip[3] + r) < 0)	return false;
	return true;	
}

static float sphere_distance_sqr(const cull_info_t * i, float x, float y, float z)
{
	float xp = x * i->model_view[0] + y * i->model_view[4] + z * i->model_view[ 8] + i->model_view[12];
	float yp = x * i->model_view[1] + y * i->model_view[5] + z * i->model_view[ 9] + i->model_view[13];
	float zp = x * i->model_view[2] + y * i->model_view[6] + z * i->model_view[10] + i->model_view[14];
	return xp*xp+yp*yp+zp*zp;
}

static void convert_to_2d(const cull_info_t * i, const float * vp, float x, float y, float z, float w, float * out_x, float * out_y)
{
	float xe = x * i->model_view[0] + y * i->model_view[4] + z * i->model_view[ 8] + w * i->model_view[12];
	float ye = x * i->model_view[1] + y * i->model_view[5] + z * i->model_view[ 9] + w * i->model_view[13];
	float ze = x * i->model_view[2] + y * i->model_view[6] + z * i->model_view[10] + w * i->model_view[14];
	float we = x * i->model_view[3] + y * i->model_view[7] + z * i->model_view[11] + w * i->model_view[15];

	float xc = xe * i->proj[0] + ye * i->proj[4] + ze * i->proj[ 8] + we * i->proj[12];
	float yc = xe * i->proj[1] + ye * i->proj[5] + ze * i->proj[ 9] + we * i->proj[13];
//	float zc = xe * i->proj[2] + ye * i->proj[6] + ze * i->proj[10] + we * i->proj[14];
	float wc = xe * i->proj[3] + ye * i->proj[7] + ze * i->proj[11] + we * i->proj[15];
	
	xc /= wc;
	yc /= wc;	
//	zc /= wc;

	*out_x = vp[0] + (1.0f + xc) * vp[2] / 2.0f;
	*out_y = vp[1] + (1.0f + yc) * vp[3] / 2.0f;
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

static	XPLMDataRef		gVisDataRef = NULL;		// Current air visiblity for culling.
static	XPLMDataRef		gAltitudeRef = NULL;	// Current aircraft altitude (for TCAS)


void			XPMPInitDefaultPlaneRenderer(void)
{
	// SETUP - mostly just fetch datarefs.

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
	string					label;
	
};
typedef	std::map<float, PlaneToRender_t>	RenderMap;


void			XPMPDefaultPlaneRenderer(int is_blend)
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
	
	cull_info_t			gl_camera;
	setup_cull_info(&gl_camera);
	XPLMCameraPosition_t x_camera;	
		
	XPLMReadCameraPosition(&x_camera);	// only for zoom!

	// Culling - read the camera pos«and figure out what's visible.

	double	maxDist = XPLMGetDataf(gVisDataRef);	
	double  labelDist = min(maxDist, MAX_LABEL_DIST) * x_camera.zoom;		// Labels get easier to see when users zooms.
	double	fullPlaneDist = x_camera.zoom * (5280.0 / 3.2) * (gFloatPrefsFunc ? gFloatPrefsFunc("planes","full_distance", 3.0) : 3.0);	// Only draw planes fully within 3 miles.
	int		maxFullPlanes = gIntPrefsFunc ? gIntPrefsFunc("planes","max_full_count", 100) : 100;						// Draw no more than 100 full planes!	

	gTotPlanes = planeCount;	
	gNavPlanes = gACFPlanes = gOBJPlanes = 0;

	int modelCount, active, plugin, tcas;
	XPLMCountAircraft(&modelCount, &active, &plugin);
	tcas = modelCount - 1;	// This is how many tcas blips we can have!
		
	RenderMap						myPlanes;		// Planes - sorted by distance so we can do the closest N and bail
	
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
		pos.label[0] = 0;

		if (XPMPGetPlaneData(id, xpmpDataType_Position, &pos) != xpmpData_Unavailable)
		{
			// First figure out where the plane is!

			double	x,y,z;
			XPLMWorldToLocal(pos.lat, pos.lon, pos.elevation * kFtToMeters, &x, &y, &z);
			
			float distMeters = sqrt(sphere_distance_sqr(&gl_camera,
                                                        static_cast<float>(x),
                                                        static_cast<float>(y),
                                                        static_cast<float>(z)));
			
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
			
			if(!cull && !sphere_is_visible(&gl_camera, static_cast<float>(x),
                                                       static_cast<float>(y),
                                                       static_cast<float>(z), 50.0))
            {
				cull = true;
            }
			
			// Full plane or lites based on distance.
			bool	drawFullPlane = (distMeters < fullPlaneDist);
			
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
				renderRecord.x = static_cast<float>(x);
				renderRecord.y = static_cast<float>(y);
				renderRecord.z = static_cast<float>(z);
				renderRecord.pitch = pos.pitch;
				renderRecord.heading = pos.heading;
				renderRecord.roll = pos.roll;
				renderRecord.model = static_cast<XPMPPlanePtr>(id)->model;
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
					renderRecord.state.yokePitch 		= surfaces.yokePitch 		;
					renderRecord.state.yokeHeading 		= surfaces.yokeHeading 		;
					renderRecord.state.yokeRoll 		= surfaces.yokeRoll 		;

					renderRecord.lights.lightFlags		= surfaces.lights.lightFlags;
				
				} else {
					renderRecord.state.structSize = sizeof(renderRecord.state);
					renderRecord.state.gearPosition = (pos.elevation < 70) ?  1.0f : 0.0f;
					renderRecord.state.flapRatio = (pos.elevation < 70) ? 1.0f : 0.0f;
					renderRecord.state.spoilerRatio = renderRecord.state.speedBrakeRatio = renderRecord.state.slatRatio = renderRecord.state.wingSweep = 0.0;
					renderRecord.state.thrust = (pos.pitch > 30) ? 1.0f : 0.6f;
					renderRecord.state.yokePitch = pos.pitch / 90.0f;
					renderRecord.state.yokeHeading = pos.heading / 180.0f;
					renderRecord.state.yokeRoll = pos.roll / 90.0f;	

					// use some smart defaults
					renderRecord.lights.bcnLights = 1;
					renderRecord.lights.navLights = 1;

				}
				if (renderRecord.model && !renderRecord.model->moving_gear)
					renderRecord.state.gearPosition = 1.0;
				renderRecord.full = drawFullPlane;	
				renderRecord.dist = distMeters;
				renderRecord.label = pos.label;
				
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
	
		size_t	renderedCounter = 0;
		
	vector<PlaneToRender_t *>			planes_obj_lites;
	multimap<int, PlaneToRender_t *>	planes_austin;
	multimap<int, PlaneToRender_t *>	planes_obj;
	vector<PlaneToRender_t *>			planes_obj8;

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
			if (gACFPlanes >= maxFullPlanes)
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
				else if(iter->second.model->plane_type == plane_Obj8)
				{
					planes_obj8.push_back(&iter->second);
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
				if(!is_blend)
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

	if(!is_blend)
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
	// Blend for solid OBJ7s?  YES!  First, in HDR mode, they DO NOT draw to the gbuffer properly -
	// they splat their livery into the normal map, which is terrifying and stupid.  Then they are also
	// pre-lit...the net result is surprisingly not much worse than regular rendering considering how many
	// bad things have happened, but for all I know we're getting NaNs somewhere.
	// 
	// Blending isn't going to hurt things in NON-HDR because our rendering is so stupid for old objs - there's
	// pretty much never translucency so we aren't going to get Z-order fails.  So f--- it...always draw blend.<
	if(is_blend)
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

	for(planeIter = planes_obj8.begin(); planeIter != planes_obj8.end(); ++planeIter)
	{
		CSL_DrawObject( (*planeIter)->model, 
						(*planeIter)->dist,
						(*planeIter)->x,
						(*planeIter)->y,
						(*planeIter)->z,
						(*planeIter)->pitch,
						(*planeIter)->roll,
						(*planeIter)->heading,
						plane_Obj8,
						(*planeIter)->full ? 1 : 0,
						(*planeIter)->lights,
					   &(*planeIter)->state);
	}

	if(!is_blend)
		obj_draw_solid();

	// PASS 3 - draw OBJ lights.

	if(is_blend)
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
	
	if(is_blend)
		obj_draw_translucent();
	obj_draw_done();	
	
	// PASS 4 - Labels
	if(is_blend)
	if ( gDrawLabels )
	{
		GLfloat	vp[4];
		glGetFloatv(GL_VIEWPORT,vp);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, vp[2], 0, vp[3], -1, 1);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		float c[4] = { 1, 1, 0, 1 };
		
		
		for (RenderMap::iterator iter = myPlanes.begin(); iter != myPlanes.end(); ++iter)
			if(iter->first < labelDist)
				if(!iter->second.cull)		// IMPORTANT - airplane BEHIND us still maps XY onto screen...so we get 180 degree reflections.  But behind us acf are culled, so that's good.
				{
					float x, y;
					convert_to_2d(&gl_camera, vp, iter->second.x, iter->second.y, iter->second.z, 1.0, &x, &y);
					
					float rat = 1.0f - (iter->first / static_cast<float>(labelDist));
					c[0] = c[1] = 0.5f + 0.5f * rat;
					c[2] = 0.5f - 0.5f * rat;		// gray -> yellow - no alpha in the SDK - foo!
					
					XPLMDrawString(c, static_cast<int>(x), static_cast<int>(y)+10, (char *) iter->second.label.c_str(), NULL, xplmFont_Basic);
				}
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();	
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
	}

	
	// Final hack - leave a note to ourselves for how many of Austin's planes we relocated to do TCAS.
	if (tcas > static_cast<int>(renderedCounter))	
		tcas = static_cast<int>(renderedCounter);
	gEnableCount = (tcas+1);
	
	gDumpOneRenderCycle = 0;
}

void XPMPEnableAircraftLabels()
{
	gDrawLabels = true;
}

void XPMPDisableAircraftLabels()
{
	gDrawLabels = false;
}

bool XPMPDrawingAircraftLabels()
{
	return gDrawLabels;
}


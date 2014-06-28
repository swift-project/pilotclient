/* 
 * Copyright (c) 2013, Laminar Research.
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

#include "XPMPMultiplayerObj8.h"
#include "XPMPMultiplayerVars.h"
#include "XPLMScenery.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include <stddef.h>
#include <vector>
using namespace std;


struct	one_inst {
	one_inst * next;
	
	XPLMDrawInfo_t			location;
	xpmp_LightStatus		lights;
	XPLMPlaneDrawState_t *	state;
	
	~one_inst() { delete next; }
};

struct	one_obj {
	one_obj *			next;
	obj_for_acf *		model;
	one_inst *			head;
	
	~one_obj() { delete head; delete next; }
};

static	one_obj *	s_worklist = NULL;


static one_inst *	s_cur_plane = NULL;

enum {
	gear_rat = 0,
	flap_rat,
	spoi_rat,
	sbrk_rat,
	slat_rat,
	swep_rat,
	thrs_rat,
	ptch_rat,
	head_rat,
	roll_rat,

	lan_lite_on,
	bcn_lite_on,
	str_lite_on,
	nav_lite_on,
	
	dref_dim
};

const char * dref_names[dref_dim] = {
	"libxplanemp/controls/gear_ratio",
	"libxplanemp/controls/flap_ratio",
	"libxplanemp/controls/spoiler_ratio",
	"libxplanemp/controls/speed_brake_ratio",
	"libxplanemp/controls/slat_ratio",
	"libxplanemp/controls/wing_sweep_ratio",
	"libxplanemp/controls/thrust_ratio",
	"libxplanemp/controls/yoke_pitch_ratio",
	"libxplanemp/controls/yoke_heading_ratio",
	"libxplanemp/controls/yoke_roll_ratio",

	"libxplanemp/controls/landing_lites_on",
	"libxplanemp/controls/beacon_lites_on",
	"libxplanemp/controls/strobe_lites_on",
	"libxplanemp/controls/nav_lites_on"
};


static float obj_get_float(void * inRefcon)
{
	if(s_cur_plane == NULL) return 0.0f;
	
	intptr_t v = reinterpret_cast<intptr_t>(inRefcon);
	switch(v) 
	{
		case gear_rat:			return s_cur_plane->state->gearPosition;		break;
		case flap_rat:			return s_cur_plane->state->flapRatio;			break;
		case spoi_rat:			return s_cur_plane->state->spoilerRatio;		break;
		case sbrk_rat:			return s_cur_plane->state->speedBrakeRatio;		break;
		case slat_rat:			return s_cur_plane->state->slatRatio;			break;
		case swep_rat:			return s_cur_plane->state->wingSweep;			break;
		case thrs_rat:			return s_cur_plane->state->thrust;				break;
		case ptch_rat:			return s_cur_plane->state->yokePitch;			break;
		case head_rat:			return s_cur_plane->state->yokeHeading;			break;
		case roll_rat:			return s_cur_plane->state->yokeRoll;			break;

		case lan_lite_on:		return static_cast<float>(s_cur_plane->lights.landLights);			break;
		case bcn_lite_on:		return static_cast<float>(s_cur_plane->lights.bcnLights);			break;
		case str_lite_on:		return static_cast<float>(s_cur_plane->lights.strbLights);			break;
		case nav_lite_on:		return static_cast<float>(s_cur_plane->lights.navLights);			break;			
			
		default:
			return 0.0f;
	}
}

int obj_get_float_array(
                                   void *               inRefcon,    
                                   float *              inValues,    
                                   int                  /*inOffset*/,    
                                   int                  inCount)
{
	if(inValues == NULL) 
		return 1;
	float rv = obj_get_float(inRefcon);
	for(int i = 0; i < inCount; ++i)
		inValues[i] = rv;
	return inCount;
}










static void (*XPLMLoadObjectAsync_p)(
                                   const char *         inPath,    
                                   XPLMObjectLoaded_f   inCallback,    
                                   void *               inRefcon)=NULL;

void	obj_init()
{
	int sim, xplm;
	XPLMHostApplicationID app;
	XPLMGetVersions(&sim,&xplm,&app);
	// Ben says: we need the 2.10 SDK (e.g. X-Plane 10) to have async load at all.  But we need 10.30 to pick up an SDK bug
	// fix where async load crashes if we queue a second load before the first completes.  So for users on 10.25, they get
	// pauses.
	if(sim >= 10300 && xplm >= 210)
	{
		XPLMLoadObjectAsync_p = (void (*)(const char *, XPLMObjectLoaded_f, void *)) XPLMFindSymbol("XPLMLoadObjectAsync");
	}
	
	for(int i = 0; i < dref_dim; ++i)
	{
		XPLMRegisterDataAccessor(
		dref_names[i], xplmType_Float|xplmType_FloatArray, 0,
		NULL, NULL, 
		obj_get_float, NULL, 
		NULL, NULL, 
		NULL, NULL, 
		obj_get_float_array, NULL, 
		NULL, NULL, reinterpret_cast<void *>(i), NULL);
	}	
}



static void		draw_objects_for_mode(one_obj * who, int want_translucent)
{
	while(who)
	{
		obj_draw_type dt = who->model->draw_type;
		if((want_translucent && dt == draw_glass) ||
		   (!want_translucent && dt != draw_glass))
		{
			for(one_inst * i = who->head; i; i = i->next)
			{
				s_cur_plane = i;
				// set dataref ptr to light + obj sate from "one_inst".			
				XPLMDrawObjects(who->model->handle, 1, &i->location, 1, 0);
			}
		}
		who = who->next;
	}
	s_cur_plane = NULL;
}

void obj_loaded_cb(XPLMObjectRef obj, void * refcon)
{
	XPLMObjectRef * targ = (XPLMObjectRef *) refcon;
	*targ = obj;
} 


void	obj_schedule_one_aircraft(
						CSLPlane_t *			model,
						double 					x,
						double 					y,
						double 					z,
						double 					pitch,
						double 					roll,
						double 					heading,
						int	   					/*full*/,		// 
						xpmp_LightStatus		lights,
						XPLMPlaneDrawState_t *	state)
{
	one_obj * iter;
	
	for(vector<obj_for_acf>::iterator att = model->attachments.begin(); att != model->attachments.end(); ++att)
	{
		obj_for_acf * model = &*att;
	
		if(model->handle == NULL &&
			!model->file.empty())
		{
			if(XPLMLoadObjectAsync_p)
				XPLMLoadObjectAsync_p(model->file.c_str(),obj_loaded_cb,(void *) &model->handle);
			else
				model->handle = XPLMLoadObject(model->file.c_str());			
			model->file.clear();
		}
	
	
		for(iter = s_worklist; iter; iter = iter->next)
		{
			if(iter->model == model)
				break;
		}
		if(iter == NULL)
		{
			iter = new one_obj;
			iter->next = s_worklist;
			s_worklist = iter;
			iter->model = model;
			iter->head = NULL;
		}
		
		if(iter->model->handle)
		{		
			one_inst * i = new one_inst;
			i->next = iter->head;
			iter->head = i;
			i->lights = lights;
			i->state = state;
			i->location.structSize = sizeof(i->location);
			i->location.x = static_cast<float>(x);
			i->location.y = static_cast<float>(y);
			i->location.z = static_cast<float>(z);
			i->location.pitch = static_cast<float>(pitch);
			i->location.roll = static_cast<float>(roll);
			i->location.heading = static_cast<float>(heading);
		}
	}	
}
						
void	obj_draw_solid()
{
	draw_objects_for_mode(s_worklist, false);
}

void	obj_draw_translucent()
{
	draw_objects_for_mode(s_worklist, true);\
}

void	obj_draw_done()
{
	delete s_worklist;
	s_worklist = NULL;
}

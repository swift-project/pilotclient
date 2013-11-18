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

#ifndef XPMPMultiplayerObj8_h
#define XPMPMultiplayerObj8_h

#include "XPLMScenery.h"
#include "XPLMPlanes.h"
#include "XPMPMultiplayer.h"
#include <string>

/*

	OBJ8_AIRCRAFT
		OBJ8 LOW_LOD NO foo.obj
		OBJ8 GLASS YES bar.obj
	AIRLINE DAL
	ICAO B732 B733
	
 */

enum obj_draw_type {

	draw_lights = 0,
	draw_low_lod,
	draw_solid,
	draw_glass

};

struct	obj_for_acf {

	std::string			file;
	XPLMObjectRef		handle;
	obj_draw_type		draw_type;
	bool				needs_animation;

};

struct	CSLPlane_t;

bool	obj_load_one_attached_obj(
						const char *		file_name,
						bool				needs_anim,
						obj_draw_type		draw_type,
						obj_for_acf&		out_attachment);

/////


void	obj_schedule_one_aircraft(
						CSLPlane_t *			model,
						double 					x,
						double 					y,
						double 					z,
						double 					pitch,
						double 					roll,
						double 					heading,
						int	   					full,		// 
						xpmp_LightStatus		lights,
						XPLMPlaneDrawState_t *	state);
						
						
void	obj_draw_solid();
void	obj_draw_translucent();
void	obj_draw_done();




#endif
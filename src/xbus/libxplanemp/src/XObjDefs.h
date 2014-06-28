/* 
 * Copyright (c) 2004, Laminar Research.
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
#ifndef XOBJDEFS_H
#define XOBJDEFS_H

#include <string>
#include <vector>
using namespace std;

enum {

	type_None = 0,
	type_PtLine,
	type_Poly,
	type_Attr
	
};	

enum {
	obj_End = 0,
	obj_Light,
	obj_Line,
	obj_Tri,
	obj_Quad,
	obj_Quad_Hard,
	obj_Quad_Cockpit,
	obj_Smoke_Black,
	obj_Smoke_White,
	obj_Movie,
	obj_Polygon,
	obj_Quad_Strip,
	obj_Tri_Strip,
	obj_Tri_Fan,
	attr_Shade_Flat,
	attr_Shade_Smooth,
	attr_Ambient_RGB,
	attr_Diffuse_RGB,
	attr_Emission_RGB,
	attr_Specular_RGB,
	attr_Shiny_Rat,
	attr_No_Depth,
	attr_Depth,
	attr_LOD,
	attr_Reset,
	attr_Cull,
	attr_NoCull,
	attr_Offset,
	attr_Max	
};	

struct	cmd_info {
	int				cmd_id;
	int				cmd_type;
	const char *	name;
	int				elem_count;
};

extern	cmd_info	gCmds[];

struct	vec_tex {
	float	v[3];
	float	st[2];
};	

struct	vec_rgb {
	float	v[3];
	float	rgb[3];
};	

struct XObjCmd {

	int				cmdType;	// Are we a line, poly or attribute?
	int				cmdID;		// What command are we?
	
	vector<float>	attributes;
	vector<vec_tex>	st;
	vector<vec_rgb>	rgb;

};

struct	XObj {

	string			texture;
	vector<XObjCmd>	cmds;

};

int	FindObjCmd(const char * inToken);

int	FindIndexForCmd(int inCmd);

#endif
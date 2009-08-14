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
#include "XObjDefs.h"
#include <string.h>

cmd_info	gCmds[] = {

{	obj_End,			type_None,		"end",				0 },
{	obj_Light,			type_PtLine,	"light",			1 },
{   obj_Line,			type_PtLine,    "line",				2 },
{   obj_Tri,			type_Poly,		"tri",				3 },
{   obj_Quad,			type_Poly,		"quad",				4 },
{   obj_Quad_Cockpit,	type_Poly,		"quad_cockpit",		4 },
{   obj_Quad_Hard,		type_Poly,		"quad_hard",		4 },
{   obj_Smoke_Black,	type_Attr,		"smoke_black",		4 },
{   obj_Smoke_White,	type_Attr,		"smoke_white",		4 },
{   obj_Movie,			type_Poly,		"quad_movie",		4 },
{   obj_Polygon,		type_Poly,		"polygon",			0 },
{   obj_Quad_Strip,		type_Poly,		"quad_strip",		0 },
{   obj_Tri_Strip,		type_Poly,		"tri_strip",		0 },
{   obj_Tri_Fan,		type_Poly,		"tri_fan",			0 },
{   attr_Shade_Flat,	type_Attr,		"ATTR_shade_flat",	0 },
{   attr_Shade_Smooth,	type_Attr,		"ATTR_shade_smooth",0 },
{   attr_Shade_Flat,	type_Attr,		"shade_flat",		0 },
{   attr_Shade_Smooth,	type_Attr,		"shade_smooth",		0 },
{   attr_Ambient_RGB,	type_Attr,		"ATTR_ambient_rgb",	3 },
{   attr_Diffuse_RGB,	type_Attr,		"ATTR_difuse_rgb",	3 },
{   attr_Emission_RGB,	type_Attr,		"ATTR_emission_rgb",3 },
{	attr_Specular_RGB,	type_Attr,		"ATTR_specular_rgb",3 },
{   attr_Shiny_Rat,		type_Attr,		"ATTR_shiny_rat",	1 },
{   attr_No_Depth,		type_Attr,		"ATTR_no_depth",	0 },
{	attr_Depth,			type_Attr,		"ATTR_depth",		0 },
{   attr_LOD,			type_Attr,		"ATTR_LOD",			2 },
{	attr_Reset,			type_Attr,		"ATTR_reset",		0 },
{	attr_Cull,			type_Attr,		"ATTR_cull",		0 },
{	attr_NoCull,		type_Attr,		"ATTR_no_cull",		0 },
{	attr_Offset,		type_Attr,		"ATTR_poly_os",		1 },
{   attr_Max,			type_None,		NULL,				0 }
};


int	FindObjCmd(const char * inToken)
{
	int n = 0;
	while (gCmds[n].name)
	{
		if (!strcmp(inToken, gCmds[n].name))
			return n;
		++n;
	}
	
	return attr_Max;
}

int	FindIndexForCmd(int inCmd)
{
	int n = 0;
	while (gCmds[n].name)
	{
		if (gCmds[n].cmd_id == inCmd)
			return n;
		++n;
	}
	return 0;
}

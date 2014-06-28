/* 
 * Copyright (c) 2006, Laminar Research.
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
#ifndef _Interpolation_h_
#define _Interpolation_h_

/* Interpolation Utilities.  These are defined inline in the hope of speed. */

double	clamp(double v, double min, double max);

double	BilinearInterpolate1d(
					double		v0,
					double		v1,
					double		position);

double	BilinearInterpolate2d(
					double		v0,	double	v1,
					double		v2, double	v3,
					double		hPosition,
					double		vPosition);
					
double	BicubicInterpolate1d(
					double		v0,
					double		v1,
					double		v2,
					double		v3,
					double		position);

double	BicubicInterpolate2d(
					double		v0,		double	v1,		double	v2,		double	v3,
					double		v4,		double	v5,		double	v6,		double	v7,
					double		v8,		double	v9,		double	v10,	double	v11,
					double		v12,	double	v13,	double	v14,	double	v15,
					double		hPosition,
					double		vPosition);

#include "Interpolation.i"

#endif

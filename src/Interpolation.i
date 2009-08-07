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


/* Interpolation inlines... */

double	clamp(double v, double min, double max)
{
	return (v < min) ? min : ((v > max) ? max : v);
}

/*  Bilinear interpolation is just a weighted average of the two sources.
	v0 is the value at point 0, v1 is the value at point 1, and position is a 
	fractional position between the two. */

double	BilinearInterpolate1d(
					double		v0,
					double		v1,
					double		position)
{
	return v0 * (1.0 - position) + v1 * position;
}
					
/*	2-d interpolation is just a recursion on 1-d interpolation.  Our points
	are organized as
		0	1
		2	3
	in two dimensions with hPosition and vPosition being fractions specifying 
	where in the square we want to interpolate. */
					
double	BilinearInterpolate2d(
					double		v0,	double	v1,
					double		v2, double	v3,
					double		hPosition,
					double		vPosition)
{
	return BilinearInterpolate1d(
				BilinearInterpolate1d(v0, v1, hPosition),
				BilinearInterpolate1d(v2, v3, hPosition),
				vPosition);
}					

/*	Bicubic Interpolation.  We take four values at -1, 0, 1, and 2.
	We use all of them to interpolate a fractional value between 0 and 1.
	But we take advantage of the extra data to provide more accuracy in
	our interpolation. */
					
double	BicubicInterpolate1d(
					double		v0,
					double		v1,
					double		v2,
					double		v3,
					double		position)
{
	/*
		Bicubic interpolation constructs a cubic polynomial through all
		four points passed in: (-1.0, v0) (0.0, v1) (1.0, v2) (2.0, v3)
		and then evaluates it for position.
		
		The polynomial can be represented as:

			Ax^3 + Bx^2 + Cx + D
			
		I precalculated the values for A, B, C, and D based on v0-v3.  We
		calculate these constants and then plug them into the equation.
	*/
	
	double 	D = v1;
	double	B = (v0 + v2) / 2.0 - v1;
	double	A = (v3 - v2 - 3 * B) / 7.0;
	double	C = v2 - A - B - D;
	
	return clamp(A * position * position * position +
				B * position * position +
				C * position + D, 0, 255.0);

}					
					
double	BicubicInterpolate2d(
					double	v0,		double	v1,		double	v2,		double	v3,
					double	v4,		double	v5,		double	v6,		double	v7,
					double	v8,		double	v9,		double	v10,	double	v11,
					double	v12,	double	v13,	double	v14,	double	v15,
					double		hPosition,
					double		vPosition)
{
	return BicubicInterpolate1d(
		BicubicInterpolate1d(v0, v1, v2, v3, hPosition),
		BicubicInterpolate1d(v4, v5, v6, v7, hPosition),
		BicubicInterpolate1d(v8, v9, v10, v11, hPosition),
		BicubicInterpolate1d(v12, v13, v14, 15, hPosition),
		vPosition);
}					

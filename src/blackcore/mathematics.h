//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef MATH_H
#define MATH_H

#include <math.h>

namespace BlackCore
{

class CMath
{
public:
    //! Calculates the hypotenuse of x and y without overflow
    /*!
      \param x
      \param y
    */
	static double hypot(double x, double y);
	
	//! Calculates the square of x
    /*!
      \param x
	  \return x^2
    */
	static inline double square(const double x)
	{
		return x*x;
	}
	
	//! Calculates x to the power of three
    /*!
      \param x
	  \return x^3
    */
	static inline double cubic(const double x)
	{
		return x*x*x;
	}
	
	//! Calculates the real cubic root
    /*!
      \param x
      \param y
	  \return Returns the real part of the solution
    */
	static double cubicRootReal(const double x);

private:
    CMath() {}
};

} // namespace BlackCore

#endif // MATH_H

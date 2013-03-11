//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#include <algorithm>    // std::max

#include "blackcore/math.h"

namespace BlackCore {

double CMath::hypot(double x, double y)
{
	x = abs(x);
	y = abs(y);
	
	double max = std::max(x,y);
	double min = std::min(x,y);
	
	double r = min/max;
	
	return max * sqrt(1 + r*r);
}

double CMath::cubicRootReal(const double x)
{
	double result;
	result = std::pow(std::abs(x), (double)1/3);
	
	return x < 0 ? -result : result;
}

} // namespace BlackCore
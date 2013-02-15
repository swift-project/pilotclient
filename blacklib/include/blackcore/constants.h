//! Copyright (C) 2013 Roland Winklmeier
//! This Source Code Form is subject to the terms of the Mozilla Public
//! License, v. 2.0. If a copy of the MPL was not distributed with this
//! file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "math.h"

namespace BlackCore
{
	namespace Constants
	{
		//! Conversion from Degree to Radians
		const double DegToRad = 4.0 * atan(1.0) / 180.0;
		
		//! Conversion from Radians to Degree
		const double RadToDeg = 180.0 / (4.0 * atan(1.0));
		
		//! Mathematical constant Pi
		const double PI = 4.0 * atan(1.0);
		
		//! 2 * Pi
		const double TwoPI	= 2.0 * PI;

		//! Conversion from feet to meter
		const double FeetToMeter = 0.3048;
		
		//! Conversion from meter to feed
		const double MeterToFeet = 3.28084;
		
		//! Conversion from knots to meter/second
		const double KnotsToMeterPerSecond = 0.5144444444;
		
		//! Equatorial radius of WGS84 ellipsoid (6378137 m)
		const double EarthRadius = 6378137.0;
		
		//! Flattening of WGS84 ellipsoid (1/298.257223563).
		const double Flattening = 1/298.257223563;

		//! First eccentricity squared
		const double e2 = (Flattening * (2 - Flattening));
		
		//! First eccentricity to the power of four
		const double e4 = CMath::square(e2);
		
		const double e2m = CMath::square(1 - Flattening);
		
		const double e2absolute = abs(e2);
	}

} // namespace BlackCore

#endif // CONSTANTS_H

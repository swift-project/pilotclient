/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "navdatareference.h"
#include <cmath>

namespace XSwiftBus
{

    //! Converts degree to radian
    inline double degreeToRadian(double angle)
    {
        return M_PI * angle / 180.0;
    }

    //! Returns the great circle distance between to nav data references
    double calculateGreatCircleDistance(const CNavDataReference &a, const CNavDataReference &b)
    {
        const static double c_earthRadiusKm = 6372.8;

        double latRad1 = degreeToRadian(a.latitude());
        double latRad2 = degreeToRadian(b.latitude());
        double lonRad1 = degreeToRadian(a.longitude());
        double lonRad2 = degreeToRadian(b.longitude());

        double diffLa = latRad2 - latRad1;
        double doffLo = lonRad2 - lonRad1;

        double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
        return 2 * c_earthRadiusKm * computation;
    }

}

/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "navdatareference.h"
#include <cmath>

namespace XSwiftBus
{
    inline double degreeToRadian(double angle)
    {
        static const double PI = acos(-1);
        return PI * angle / 180.0;
    }

    double calculateGreatCircleDistance(const CNavDataReference &a, const CNavDataReference &b)
    {
        const static double c_earthRadiusKm = 6372.8;

        const double latRad1 = degreeToRadian(a.latitude());
        const double latRad2 = degreeToRadian(b.latitude());
        const double lonRad1 = degreeToRadian(a.longitude());
        const double lonRad2 = degreeToRadian(b.longitude());

        const double diffLa = latRad2 - latRad1;
        const double doffLo = lonRad2 - lonRad1;

        const double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
        return 2 * c_earthRadiusKm * computation;
    }
} // ns

//! \endcond

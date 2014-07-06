/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathematics.h"
#include <algorithm> // std::max
#include <cmath>

namespace BlackMisc
{
    namespace Math
    {
        /*
         * Hypotenuse
         */
        double CMath::hypot(double x, double y)
        {
            x = qAbs(x);
            y = qAbs(y);
            double max = std::max(x, y);
            double min = std::min(x, y);
            double r = min / max;
            return max * sqrt(1 + r * r);
        }

        /*
         * Real part of cubic root
         */
        double CMath::cubicRootReal(double x)
        {
            double result;
            result = std::pow(qAbs(x), (double)1.0 / 3.0);
            return x < 0 ? -result : result;
        }

        /*
         * Round utility method
         */
        double CMath::round(double value, int digits)
        {
            // gosh, is there no Qt method for this??? It's year 2013
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (fractpart == 0) return value; // do not mess any "integers" to the worse
            double m = pow(10.0, digits);
            qint64 ri = qRound64(value * m); // do not loose any range here
            double rv = double(ri) / m;
            return rv;
        }

        /*
         * Round by given epsilon value
         */
        double CMath::roundEpsilon(double value, double epsilon)
        {
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (fractpart == 0) return value; // do not mess any "integers" to the worse
            qint64 ri = qRound(value / epsilon);
            double rv = double(ri) * epsilon; // do not loose any range here
            return rv;
        }

        /*
         * Equal, considering equal
         */
        bool CMath::epsilonEqual(double v1, double v2, double epsilon)
        {
            if (v1 == v2) return true;
            return qAbs(v1 - v2) <= epsilon;
        }

        /*
         * To radians
         */
        double CMath::deg2rad(double degree)
        {
            return degree * CMath::PI() / 180.0;
        }

        /*
         * To radians
         */
        double CMath::rad2deg(double radians)
        {
            return radians * 180.0 / CMath::PI();
        }

        /*
         * Normalize degrees
         */
        double CMath::normalizeDegrees(double degrees)
        {
            if (degrees == 0.0 || degrees == 360.0 || degrees == -360.0) return 0.0;
            if (degrees > 0)
            {
                while (degrees >= 360.0)
                {
                    degrees -= 360.0;
                }
            }
            else
            {
                while (degrees < 0.0)
                {
                    degrees += 360.0;
                }
            }
            return degrees;
        }

    } // namespace
} // namespace

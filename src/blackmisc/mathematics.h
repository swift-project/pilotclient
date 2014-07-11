/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_MATHEMATICS_H
#define BLACKMISC_MATHEMATICS_H
#include <QtCore/qmath.h>
#include <cmath>

namespace BlackMisc
{
    namespace Math
    {

        //! Math utils
        class CMath
        {
        public:

            //! Calculates the hypotenuse of x and y without overflow
            static double hypot(double x, double y);

            //! Calculates the square of x
            static inline double square(double x)
            {
                return x * x;
            }

            //! Calculates x to the power of three
            static inline double cubic(const double x)
            {
                return x * x * x;
            }

            //! Calculates the real cubic root
            static double cubicRootReal(double x);

            //! Utility round method
            static double round(double value, int digits);

            //! Round by given epsilon
            static double roundEpsilon(double value, double epsilon);

            //! Epsilon safe equal
            static bool epsilonEqual(double v1, double v2, double epsilon = 1E-06);

            //! Nearest integer not greater in magnitude than value, correcting for epsilon
            static inline double trunc(double value, double epsilon = 1e-10)
            {
                return value < 0 ? ceil(value - epsilon) : floor(value + epsilon);
            }

            //! Fractional part of value
            static inline double fract(double value)
            {
                double unused;
                return modf(value, &unused);
            }

            //! PI / 2
            static const double &PIHALF()
            {
                static double pi = 2.0 * qAtan(1.0);
                return pi;
            }

            //! PI
            static const double &PI()
            {
                static double pi = 4.0 * qAtan(1.0);
                return pi;
            }

            //! PI * 2
            static const double &PI2()
            {
                static double pi2 = 8.0 * qAtan(1.0);
                return pi2;
            }

            //! Degrees to radians
            static double deg2rad(double degree);

            //! Radians to degrees
            static double rad2deg(double radians);

            //! Normalize: 0≤ degrees <360
            static double normalizeDegrees(double degrees);

        private:
            //! No objects, just static
            CMath();
        };

    } // namespace
} // namespace
#endif // guard

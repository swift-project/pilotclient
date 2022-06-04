/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MATH_MATHUTILS_H
#define BLACKMISC_MATH_MATHUTILS_H

#include "blackmisc/blackmiscexport.h"

#include <QRandomGenerator>
#include <QtCore/qmath.h>
#include <QPair>
#include <cmath>
#include <numeric>

namespace BlackMisc::Math
{
    //! Math utils
    class BLACKMISC_EXPORT CMathUtils
    {
    public:
        //! No objects, just static
        CMathUtils() = delete;

        //! Utility round method
        static double round(double value, int digits);

        //! Round by given epsilon
        static double roundEpsilon(double value, double epsilon);

        //! Epsilon safe equal
        //! @{
        static bool epsilonEqual(float v1, float v2, float epsilon = 1E-06f);
        static bool epsilonEqual(double v1, double v2, double epsilon = 1E-06);
        static bool epsilonEqualLimits(double v1, double v2) { return qAbs(v1 - v2) <= std::numeric_limits<double>::epsilon(); }
        //! @}

        //! Epsilon safe zero
        //! @{
        static bool epsilonZero(double v, double epsilon) { return epsilonEqual(v, 0.0, epsilon); }
        static bool epsilonZero(double v) { return epsilonEqual(v, 0.0, 1E-06); }
        static inline bool epsilonZeroLimits(double v) { return qAbs(v) <= std::numeric_limits<double>::epsilon(); }
        //! @}

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

        //! Degrees to radians
        static double deg2rad(double degree);

        //! Radians to degrees
        static double rad2deg(double radians);

        //! Normalize: -180< degrees ≤180
        static double normalizeDegrees180(double degrees);

        //! Normalize: 0≤ degrees <360
        static double normalizeDegrees360(double degrees);

        //! Thread-local random generator
        static QRandomGenerator &randomGenerator();

        //! Random number between low and high
        static int randomInteger(int low, int high);

        //! Random double 0-max
        static double randomDouble(double max = 1);

        //! Random boolean
        static bool randomBool();

        //! Round numToRound to the nearest multiple of divisor
        static int roundToMultipleOf(int value, int divisor);

        //! Fractional part as integer string, e.g. 3.12 -> 12 / 3.012 -> 012
        //! \remark because of leading 0 returned as string
        static QString fractionalPartAsString(double value, int width = -1);

        //! Calculate the sum
        static double sum(const QList<double> &values);

        //! Calculate the mean
        static double mean(const QList<double> &values);

        //! Calculate the standard deviation
        static double standardDeviation(const QList<double> &values);

        //! Standard deviation (first) and mean (second)
        static QPair<double, double> standardDeviationAndMean(const QList<double> &values);

    private:
        //! Calculate the variance
        static double variance(const QList<double> &values);

        //! The squared differences to mean
        static QList<double> squaredDifferences(const QList<double> &values);

        //! The squared differences to mean
        static QList<double> squaredDifferences(const QList<double> &values, double meanValue);
    };
} // namespace

#endif // guard

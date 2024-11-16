// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_MATH_MATHUTILS_H
#define SWIFT_MISC_MATH_MATHUTILS_H

#include <QtCore/qmath.h>

#include <cmath>
#include <numeric>

#include <QPair>
#include <QRandomGenerator>

#include "misc/swiftmiscexport.h"

namespace swift::misc::math
{
    //! Math utils
    class SWIFT_MISC_EXPORT CMathUtils
    {
    public:
        //! No objects, just static
        CMathUtils() = delete;

        //! Utility round method
        static double round(double value, int digits);

        //! Round by given epsilon
        static double roundEpsilon(double value, double epsilon);

        //! @{
        //! Epsilon safe equal
        static bool epsilonEqual(float v1, float v2, float epsilon = 1E-06f);
        static bool epsilonEqual(double v1, double v2, double epsilon = 1E-06);
        static bool epsilonEqualLimits(double v1, double v2) { return qAbs(v1 - v2) <= std::numeric_limits<double>::epsilon(); }
        //! @}

        //! @{
        //! Epsilon safe zero
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
} // namespace swift::misc::math

#endif // guard

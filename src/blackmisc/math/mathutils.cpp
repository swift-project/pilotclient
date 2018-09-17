/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/math/mathutils.h"
#include "blackmisc/verify.h"

#include <QThreadStorage>
#include <QTime>
#include <QtGlobal>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace BlackMisc
{
    namespace Math
    {
        double CMathUtils::hypot(double x, double y)
        {
            x = qAbs(x);
            y = qAbs(y);
            const double max = std::max(x, y);
            const double min = std::min(x, y);
            double r = min / max;
            return max * sqrt(1 + r * r);
        }

        double CMathUtils::cubicRootReal(double x)
        {
            const double result = std::pow(qAbs(x), 1.0 / 3.0);
            return x < 0 ? -result : result;
        }

        double CMathUtils::round(double value, int digits)
        {
            // gosh, is there no Qt method for this??? It's year 2013
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (epsilonZeroLimits(fractpart)) { return value; } // do not mess any "integers" to the worse
            const double m = pow(10.0, digits);
            const qint64 ri = qRound64(value * m); // do not loose any range here
            const double rv = static_cast<double>(ri) / m;
            return rv;
        }

        QString CMathUtils::roundAsString(double value, int digits)
        {
            return QString::number(round(value, digits));
        }

        double CMathUtils::roundEpsilon(double value, double epsilon)
        {
            if (epsilonZeroLimits(epsilon)) { return value; } // avoid division by 0
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (epsilonZeroLimits(fractpart)) { return value; } // do not mess any "integers" to the worse
            const double roundValue = value / epsilon;
            const qint64 ri = qRound64(roundValue);
            const double rv = static_cast<double>(ri) * epsilon; // do not loose any range here
            return rv;
        }

        bool CMathUtils::epsilonEqual(float v1, float v2, float epsilon)
        {
            return qAbs(v1 - v2) <= epsilon;
        }

        bool CMathUtils::epsilonEqual(double v1, double v2, double epsilon)
        {
            return qAbs(v1 - v2) <= epsilon;
        }

        double CMathUtils::deg2rad(double degree)
        {
            return degree * CMathUtils::PI() / 180.0;
        }

        double CMathUtils::rad2deg(double radians)
        {
            return radians * 180.0 / CMathUtils::PI();
        }

        double CMathUtils::normalizeDegrees180(double degrees)
        {
            const double result = CMathUtils::normalizeDegrees360(degrees + 180.0) - 180.0;
            return (result <= -180.0) ? 180.0 : result; // -180->180
        }

        double CMathUtils::normalizeDegrees360(double degrees)
        {
            const double result = std::fmod(degrees, 360.0);
            return (result >= 0.0) ? result : result + 360.0;
        }

        int CMathUtils::randomInteger(int low, int high)
        {
            static QThreadStorage<uint> seeds;
            Q_ASSERT_X(high < INT_MAX, Q_FUNC_INFO, "Cannot add 1");
            Q_ASSERT_X(low >= 0 && high >= 0, Q_FUNC_INFO, "Only valid for positive values");
            if (!seeds.hasLocalData())
            {
                // seed is per thread!
                const uint seed = static_cast<uint>(QTime::currentTime().msec());
                qsrand(seed);
                seeds.setLocalData(seed);
            }
            const int r(qrand());
            const int mod = (high + 1) - low;
            Q_ASSERT_X(mod <= RAND_MAX, Q_FUNC_INFO, "RAND_MAX exceeded");
            return (r % mod) + low;
        }

        double CMathUtils::randomDouble(double max)
        {
            // on Win system, RAND_MAX is only 16bit, on other systems higher
            static const int MAX(RAND_MAX < INT_MAX ? RAND_MAX - 1 : INT_MAX - 1);
            const double r = randomInteger(0, MAX);
            return (r / MAX) * max;
        }

        int CMathUtils::roundToMultipleOf(int value, int divisor)
        {
            Q_ASSERT(divisor != 0);
            Q_ASSERT(divisor >= - std::numeric_limits<int>::max());
            divisor = std::abs(divisor);
            Q_ASSERT(std::abs(value) < std::numeric_limits<int>::max() - divisor / 2);

            int multiplier = value / divisor;
            int remainder = std::abs(value % divisor);
            int shortfall = divisor - remainder;

            if (shortfall < remainder) { multiplier += value < 0 ? -1 : 1; }

            return multiplier * divisor;
        }

        QString CMathUtils::fractionalPartAsString(double value, int width)
        {
            double intpart;
            const double fractpart = modf(value, &intpart);
            const QString f = QString::number(fractpart);
            const QString fInt = f.length() < 3 ? QString("0") : f.mid(2);
            if (width < 0) { return fInt; }
            if (fInt.length() >= width) { return fInt.left(width); }
            return fInt.leftJustified(width, '0');
        }

        double CMathUtils::sum(const QList<double> &values)
        {
            double sum = 0;
            for (double v : values) { sum += v; }
            return sum;
        }

        QList<double> CMathUtils::squaredDifferences(const QList<double> &values)
        {
            const double meanValue = mean(values);
            return squaredDifferences(values, meanValue);
        }

        QList<double> CMathUtils::squaredDifferences(const QList<double> &values, double meanValue)
        {
            QList<double> squaresDifferences;
            for (double v : values)
            {
                const double vd = v - meanValue;
                squaresDifferences.push_back(vd * vd);
            }
            return squaresDifferences;
        }

        double CMathUtils::mean(const QList<double> &values)
        {
            BLACK_VERIFY_X(!values.isEmpty(), Q_FUNC_INFO, "Need values");
            return sum(values) / values.size();
        }

        double CMathUtils::variance(const QList<double> &values)
        {
            const double variance = mean(squaredDifferences(values));
            return variance;
        }

        double CMathUtils::standardDeviation(const QList<double> &values)
        {
            const double sd = sqrt(variance(values));
            return sd;
        }

        QPair<double, double> CMathUtils::standardDeviationAndMean(const QList<double> &values)
        {
            const double meanValue = mean(values);
            const double varianceValue = mean(squaredDifferences(values, meanValue));
            const double sd = sqrt(varianceValue);
            return QPair<double, double>(sd, meanValue);
        }
    } // namespace
} // namespace

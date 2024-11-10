// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/math/constants.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/verify.h"

#include <QTime>
#include <QtGlobal>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace BlackMisc::Math
{
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
        return degree * c_pi / 180.0;
    }

    double CMathUtils::rad2deg(double radians)
    {
        return radians * 180.0 / c_pi;
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

    QRandomGenerator &CMathUtils::randomGenerator()
    {
        thread_local QRandomGenerator rng(QRandomGenerator::global()->generate());
        return rng;
    }

    int CMathUtils::randomInteger(int low, int high)
    {
        Q_ASSERT_X(high < INT_MAX, Q_FUNC_INFO, "Cannot add 1");
        return randomGenerator().bounded(low, high + 1);
    }

    double CMathUtils::randomDouble(double max)
    {
        constexpr int MAX(std::min(RAND_MAX - 1, INT_MAX - 1));
        const double r = randomInteger(0, MAX);
        return (r / MAX) * max;
    }

    bool CMathUtils::randomBool()
    {
        return randomInteger(0, 1);
    }

    int CMathUtils::roundToMultipleOf(int value, int divisor)
    {
        Q_ASSERT(divisor != 0);
        Q_ASSERT(divisor >= -std::numeric_limits<int>::max());
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
        const int prec = width >= 0 ? width + 1 : 10;
        const QString f = QString::number(fractpart, 'f', prec); // avoid scientific notation
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
        SWIFT_VERIFY_X(!values.isEmpty(), Q_FUNC_INFO, "Need values");
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

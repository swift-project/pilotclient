/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/math/mathutils.h"

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
            double max = std::max(x, y);
            double min = std::min(x, y);
            double r = min / max;
            return max * sqrt(1 + r * r);
        }

        double CMathUtils::cubicRootReal(double x)
        {
            double result;
            result = std::pow(qAbs(x), 1.0 / 3.0);
            return x < 0 ? -result : result;
        }

        double CMathUtils::round(double value, int digits)
        {
            // gosh, is there no Qt method for this??? It's year 2013
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (fractpart == 0) return value; // do not mess any "integers" to the worse
            double m = pow(10.0, digits);
            qint64 ri = qRound64(value * m); // do not loose any range here
            double rv = static_cast<double>(ri) / m;
            return rv;
        }

        double CMathUtils::roundEpsilon(double value, double epsilon)
        {
            if (epsilon == 0) { return value; } // avoid division by 0
            double fractpart, intpart;
            fractpart = modf(value, &intpart);
            if (fractpart == 0) { return value; } // do not mess any "integers" to the worse
            const double roundValue = value / epsilon;
            qint64 ri = qRound64(roundValue);
            double rv = static_cast<double>(ri) * epsilon; // do not loose any range here
            return rv;
        }

        bool CMathUtils::epsilonEqual(double v1, double v2, double epsilon)
        {
            if (v1 == v2) return true;
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

        double CMathUtils::normalizeDegrees(double degrees)
        {
            double result = std::fmod(degrees, 360.0);
            return (result >= 0.0) ? result : result + 360.0;
        }

        int CMathUtils::randomInteger(int low, int high)
        {
            static QThreadStorage<uint> seeds;
            if (!seeds.hasLocalData())
            {
                // seed is per thread!
                uint seed = static_cast<uint>(QTime::currentTime().msec());
                qsrand(seed);
                seeds.setLocalData(seed);
            }
            int r(qrand());
            return r % ((high + 1) - low) + low;
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
    } // namespace
} // namespace

/*  Copyright (C) 2013 VATSIM Community / authors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/mathematics.h"
#include <algorithm>    // std::max

namespace BlackMisc
{
namespace Math
{

/*
 * Hypotenuse
 */
qreal CMath::hypot(qreal x, qreal y)
{
    x = abs(x);
    y = abs(y);
    double max = std::max(x, y);
    double min = std::min(x, y);
    double r = min / max;
    return max * sqrt(1 + r * r);
}

/*
 * Real part of cubic root
 */
qreal CMath::cubicRootReal(qreal x)
{
    double result;
    result = std::pow(std::abs(x), (double)1 / 3);
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
    qint64 ri = qRound(value * m); // do not loose any range here
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

} // namespace
} // namespace

/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/pqunits.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*
 * Convert to SI
 */
double CTemperatureUnit::conversionToSiConversionUnit(double value) const
{
    double v = value + this->m_conversionOffsetToSi;
    v *= this->getConversionFactorToSI();
    return v;
}

/*
 * Convert from SI
 */
double CTemperatureUnit::conversionFromSiConversionUnit(double value) const
{
    double v = value / this->getConversionFactorToSI();
    v -= this->m_conversionOffsetToSi;
    return v;
}

/*
 * Convert from SI
 */
double CAngleUnit::conversionSexagesimalFromSi(const CMeasurementUnit &angleUnit, double value)
{
    // using rounding here, since fractions can lead to ugly sexagesimal conversion
    // e.g. 185.499999 gives 185 29' 59.9999"
    value = angleUnit.epsilonUpRounding(value * 180 / M_PI); // degree
    double v = floor(value);
    double c = value - v;
    double mr = c * 60.0;
    double m = floor(mr); // minutes
    double s = (mr - m) * 60; // seconds + rest fraction
    v = (v + (m / 100) + (s / 10000));
    return v;
}

/*
 * Convert to SI
 */
double CAngleUnit::conversionSexagesimalToSi(const CMeasurementUnit &, double value)
{
    double v = floor(value); // degrees
    double c = value - v;
    c = c * 100.0;
    double m = floor(c);
    c = c - m;
    m /= 60.0; // minutes back to decimals
    double s = c / 36.0; // seconds back to decimals
    v = v + m + s;
    return v / 180.0 * M_PI;
}

/*
 * Rounded to QString
 */
QString CAngleUnit::toQStringRounded(double value, int digits) const
{
    QString s;
    if ((*this) == CAngleUnit::sexagesimalDeg()) {
        // special formatting for sexagesimal degrees
        double de = floor(value);
        double mi = floor((value - de) * 100.0);
        double se = floor((value - de - mi / 100.0) * 1000000) / 100.0;
        QString ses = QLocale::system().toString(se, 'f', 2);
        s = QString::number(de).append(this->getUnitName()).append(QString::number(mi))
            .append("'").append(ses).append("\"");
    } else {
        s = CMeasurementUnit::toQStringRounded(value, digits);
    }
    return s;
}

} // namespace
} // namespace

/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/pqbase.h"
#include "blackmisc/mathematics.h"

using namespace BlackMisc::Math;

namespace BlackMisc
{
namespace PhysicalQuantities
{

// -----------------------------------------------------------------------
// --- Mulitplier --------------------------------------------------------
// -----------------------------------------------------------------------

/*
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const QString &name, const QString &unitName, double factor) :
    m_name(name), m_prefix(unitName), m_factor(factor)
{
}

/*
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const CMeasurementPrefix &other) :
    m_name(other.m_name), m_prefix(other.m_prefix), m_factor(other.m_factor)
{
}

/*
 * Assignment operator
 */
CMeasurementPrefix &CMeasurementPrefix::operator=(const CMeasurementPrefix &other)
{
    if (this == &other) return *this;
    this->m_name = other.m_name;
    this->m_prefix = other.m_prefix;
    this->m_factor = other.m_factor;
    return *this;
}

/*
 * Equal?
 */
bool CMeasurementPrefix::operator ==(const CMeasurementPrefix &other) const
{
    if (this == &other) return true;
    return this->m_factor == other.m_factor && this->m_name == other.m_name;
}

/*
 * Not equal
 */
bool CMeasurementPrefix::operator !=(const CMeasurementPrefix &other) const
{
    return !(*this == other);
}

/*
 * Greater?
 */
bool CMeasurementPrefix::operator >(const CMeasurementPrefix &other) const
{
    return this->m_factor > other.m_factor;
}

/*
 * Less?
 */
bool CMeasurementPrefix::operator <(const CMeasurementPrefix &other) const
{
    return this->m_factor < other.m_factor;
}

// -----------------------------------------------------------------------
// --- Measurement unit --------------------------------------------------
// -----------------------------------------------------------------------

/*
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &unitName, const QString &type, bool isSIUnit, bool isSIBaseUnit,
                                   double conversionFactorToSI, const CMeasurementPrefix &multiplier, qint32 displayDigits, double epsilon,
                                   UnitConverter toSiConverter, UnitConverter fromSiConverter):
    m_name(name), m_unitName(unitName), m_type(type), m_isSiUnit(isSIUnit), m_isSiBaseUnit(isSIBaseUnit),
    m_conversionFactorToSIConversionUnit(conversionFactorToSI),
    m_epsilon(epsilon), m_displayDigits(displayDigits), m_multiplier(multiplier), m_fromSiConverter(fromSiConverter), m_toSiConverter(toSiConverter)
{
}

/*
 * Copy constructor
 */
CMeasurementUnit::CMeasurementUnit(const CMeasurementUnit &other):
    m_name(other.m_name), m_unitName(other.m_unitName), m_type(other.m_type), m_isSiUnit(other.m_isSiUnit),
    m_isSiBaseUnit(other.m_isSiBaseUnit), m_conversionFactorToSIConversionUnit(other.m_conversionFactorToSIConversionUnit),
    m_epsilon(other.m_epsilon), m_displayDigits(other.m_displayDigits), m_multiplier(other.m_multiplier), m_fromSiConverter(other.m_fromSiConverter), m_toSiConverter(other.m_toSiConverter)
{
}

/*
 * Assigment operator
 */
CMeasurementUnit &CMeasurementUnit::operator =(const CMeasurementUnit &other)
{
    if (this == &other) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->m_name = other.m_name;
    this->m_unitName = other.m_unitName;
    this->m_type = other.m_type;
    this->m_isSiUnit = other.m_isSiUnit;
    this->m_isSiBaseUnit = other.m_isSiBaseUnit;
    this->m_conversionFactorToSIConversionUnit = other.m_conversionFactorToSIConversionUnit;
    this->m_multiplier = other.m_multiplier;
    this->m_displayDigits = other.m_displayDigits;
    this->m_epsilon = other.m_epsilon;
    this->m_fromSiConverter = other.m_fromSiConverter;
    this->m_toSiConverter = other.m_toSiConverter;
    return *this;
}

/*
 * Equal operator
 */
bool CMeasurementUnit::operator ==(const CMeasurementUnit &other) const
{
    if (this == &other) return true;
    if (this->m_type != other.m_type) return false;
    return this->m_multiplier == other.m_multiplier && this->m_name == other.m_name
           && this->m_isSiUnit == other.m_isSiUnit;
}

/*
 * Unequal operator
 */
bool CMeasurementUnit::operator !=(const CMeasurementUnit &other) const
{
    return !(other == *this);
}

/*
 * Conversion factor from unit x to y
 */
double CMeasurementUnit::conversionToUnit(double value, const CMeasurementUnit &to) const
{
    if (to == *this) return value;
    double siValue = this->convertToSiConversionUnit(value);
    return to.convertFromSiConversionUnit(siValue);
}

/*
 * Value to QString with unit, e.g. "5.00m"
 * @return
 */
QString CMeasurementUnit::valueRoundedWithUnit(double value, int digits, bool i18n) const
{
    return this->toQStringRounded(value, digits).append(this->getUnitName(i18n));
}

/*
 * Value rounded
 */
double CMeasurementUnit::valueRounded(double value, int digits) const
{
    if (digits < 0) digits = this->m_displayDigits;
    return CMath::round(value, digits);
}

/*
 * Rounded to QString
 */
QString CMeasurementUnit::toQStringRounded(double value, int digits, bool /* i18n */) const
{
    if (digits < 0) digits = this->m_displayDigits;
    double v = CMath::round(value, digits);
    QString s = QLocale::system().toString(v, 'f', digits);
    return s;
}

/*
 * Epsilon rounding
 */
double CMeasurementUnit::epsilonUpRounding(double value) const
{
    // Rounds a little up in order to avoid fractions
    double eps = value > 0 ? this->m_epsilon : -1.0 * this->m_epsilon;
    double v = floor((value + eps) / this->m_epsilon);
    v *= this->m_epsilon;
    return v;
}

} // namespace
} // namespace

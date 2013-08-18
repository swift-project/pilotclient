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
// --- Prefix ------------------------------------------------------------
// -----------------------------------------------------------------------

/*
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const QString &name, const QString &symbol, double factor) :
    m_name(name), m_symbol(symbol), m_factor(factor)
{
}

/*
 * Constructor
 */
CMeasurementPrefix::CMeasurementPrefix(const CMeasurementPrefix &other) :
    m_name(other.m_name), m_symbol(other.m_symbol), m_factor(other.m_factor)
{
}

/*
 * Assignment operator
 */
CMeasurementPrefix &CMeasurementPrefix::operator=(const CMeasurementPrefix &other)
{
    if (this == &other) return *this;
    this->m_name = other.m_name;
    this->m_symbol= other.m_symbol;
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

// -----------------------------------------------------------------------
// --- Measurement unit --------------------------------------------------
// -----------------------------------------------------------------------

/*
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &symbol, double factor, qint32 displayDigits, double epsilon) :
    m_name(name), m_symbol(symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_prefix(CMeasurementPrefix::One()), m_converter(new LinearConverter(factor))
{
}

/*
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &symbol, double factor, double offset, qint32 displayDigits, double epsilon) :
    m_name(name), m_symbol(symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_prefix(CMeasurementPrefix::One()), m_converter(new AffineConverter(factor, offset))
{
}

/*
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const QString &name, const QString &symbol, Converter *converter, qint32 displayDigits, double epsilon) :
    m_name(name), m_symbol(symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_prefix(CMeasurementPrefix::One()), m_converter(converter)
{
}

/*
 * Constructor
 */
CMeasurementUnit::CMeasurementUnit(const CMeasurementUnit &base, const CMeasurementPrefix &prefix, qint32 displayDigits, double epsilon) :
    m_name(base.m_name), m_symbol(base.m_symbol), m_epsilon(epsilon), m_displayDigits(displayDigits), m_prefix(prefix), m_converter(base.m_converter->clone(prefix))
{
}

/*
 * Copy constructor
 */
CMeasurementUnit::CMeasurementUnit(const CMeasurementUnit &other) :
    m_name(other.m_name), m_symbol(other.m_symbol), m_epsilon(other.m_epsilon), m_displayDigits(other.m_displayDigits), m_prefix(other.m_prefix), m_converter(other.m_converter)
{
}

/*
 * Assigment operator
 */
CMeasurementUnit &CMeasurementUnit::operator =(const CMeasurementUnit &other)
{
    if (this == &other) return *this; // Same object? Yes, so skip assignment, and just return *this
    this->m_name = other.m_name;
    this->m_symbol = other.m_symbol;
    this->m_prefix = other.m_prefix;
    this->m_displayDigits = other.m_displayDigits;
    this->m_epsilon = other.m_epsilon;
    this->m_converter = other.m_converter;
    return *this;
}

/*
 * Equal operator
 */
bool CMeasurementUnit::operator ==(const CMeasurementUnit &other) const
{
    if (this == &other) return true;
    return this->m_prefix == other.m_prefix && this->m_name == other.m_name;
}

/*
 * Unequal operator
 */
bool CMeasurementUnit::operator !=(const CMeasurementUnit &other) const
{
    return !(other == *this);
}

/*
 * Conversion
 */
double CMeasurementUnit::convertFrom(double value, const CMeasurementUnit &unit) const
{
    Q_ASSERT(this->m_converter);
    Q_ASSERT(unit.m_converter);
    if (this->m_converter == unit.m_converter) return value;
    return this->m_converter->fromDefault(unit.m_converter->toDefault(value));
}

/*
 * Value to QString with unit, e.g. "5.00m"
 * @return
 */
QString CMeasurementUnit::makeRoundedQStringWithUnit(double value, int digits, bool i18n) const
{
    return this->makeRoundedQString(value, digits).append(this->getSymbol(i18n));
}

/*
 * Value rounded
 */
double CMeasurementUnit::roundValue(double value, int digits) const
{
    if (digits < 0) digits = this->m_displayDigits;
    return CMath::round(value, digits);
}

/*
 * Rounded to QString
 */
QString CMeasurementUnit::makeRoundedQString(double value, int digits, bool /* i18n */) const
{
    if (digits < 0) digits = this->m_displayDigits;
    double v = CMath::round(value, digits);
    QString s = QLocale::system().toString(v, 'f', digits);
    return s;
}

} // namespace
} // namespace

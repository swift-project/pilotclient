/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/pqallquantities.h"

namespace BlackMisc
{
namespace PhysicalQuantities
{

/*
 * Constructor by integer
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(qint32 baseValue, const MU &unit, const MU &siConversionUnit) :
    m_unit(unit), m_conversionSiUnit(siConversionUnit)
{
    this->setUnitValue(baseValue);
}

/*
 * Constructor by double
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(double baseValue, const MU &unit, const MU &siConversionUnit) :
    m_unit(unit), m_conversionSiUnit(siConversionUnit)
{
    this->setUnitValue(baseValue);
}

/*
 * Copy constructor
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(const CPhysicalQuantity &other) :
    m_unitValueD(other.m_unitValueD), m_unitValueI(other.m_unitValueI), m_convertedSiUnitValueD(other.m_convertedSiUnitValueD),
    m_isIntegerBaseValue(other.m_isIntegerBaseValue), m_unit(other.m_unit), m_conversionSiUnit(other.m_conversionSiUnit)
{
}

/*
 * Destructor
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::~CPhysicalQuantity()
{
}

/*
 * Equal operator ==
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator ==(const CPhysicalQuantity<MU, PQ> &other) const
{
    if (this == &other) return true;
    if (this->m_unit.getType() != other.m_unit.getType()) return false;

    // some special cases for best quality
    double diff;
    const double lenient = 1.001; // even diff already has a rounding issue to be avoided
    bool eq = false;
    if (this->m_unit == other.m_unit)
    {
        // same unit
        if (this->m_isIntegerBaseValue && other.m_isIntegerBaseValue)
        {
            // pure integer comparison, no rounding issues
            eq = this->m_unitValueI == other.m_unitValueI;
        }
        else
        {
            // same unit, comparison based on double
            diff = qAbs(this->m_unitValueD - other.m_unitValueD);
            eq = diff <= (lenient * this->m_unit.getEpsilon());
        }
    }
    else
    {
        // based on SI value
        diff = qAbs(this->m_convertedSiUnitValueD - other.m_convertedSiUnitValueD);
        eq = diff <= (lenient * this->m_conversionSiUnit.getEpsilon());
    }
    return eq;
}

/*
 * Not equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator !=(const CPhysicalQuantity<MU, PQ> &other) const
{
    return !((*this) == other);
}

/*
 * Assignment operator =
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>& CPhysicalQuantity<MU, PQ>::operator=(const CPhysicalQuantity<MU, PQ> &other)
{
    if (this == &other) return *this;

    this->m_unitValueI = other.m_unitValueI;
    this->m_unitValueD = other.m_unitValueD;
    this->m_convertedSiUnitValueD = other.m_convertedSiUnitValueD;
    this->m_isIntegerBaseValue = other.m_isIntegerBaseValue;
    this->m_unit = other.m_unit;
    this->m_conversionSiUnit = other.m_conversionSiUnit;
    return *this;
}

/*
 * Plus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator +=(const CPhysicalQuantity<MU, PQ> &other)
{
    if (this->m_unit == other.m_unit)
    {
        // same unit
        if (this->m_isIntegerBaseValue && other.m_isIntegerBaseValue)
        {
            // pure integer, no rounding issues
            this->setUnitValue(other.m_unitValueI + this->m_unitValueI);
        }
        else
        {
            this->setUnitValue(other.m_unitValueD + this->m_unitValueD);
        }
    }
    else
    {
        double v = other.value(this->m_unit);
        this->setUnitValue(v + this->m_unitValueD);
    }
    return *this;
}

/*
 * Plus operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator +(const PQ &other) const
{
    PQ copy(other);
    copy += *this;
    return copy;
}

/*
 * Explicit plus
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::addUnitValue(double value)
{
    this->setUnitValue(this->m_unitValueD + value);
}

/*
 * Explicit minus
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::substractUnitValue(double value)
{
    this->setUnitValue(this->m_unitValueD - value);
}

/*
 * Minus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator -=(const CPhysicalQuantity<MU, PQ> &other)
{
    if (this->m_unit == other.m_unit)
    {
        // same unit
        if (this->m_isIntegerBaseValue && other.m_isIntegerBaseValue)
        {
            // pure integer, no rounding issues
            this->setUnitValue(other.m_unitValueI - this->m_unitValueI);
        }
        else
        {
            this->setUnitValue(other.m_unitValueD - this->m_unitValueD);
        }
    }
    else
    {
        double v = other.value(this->m_unit);
        this->setUnitValue(v - this->m_unitValueD);
    }
    return *this;
}

/*
 * Minus operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator -(const PQ &other) const
{
    PQ copy = *derived();
    copy -= other;
    return copy;
}

/*
 * Multiply operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator *=(double multiply)
{
    this->setUnitValue(this->m_unitValueD * multiply);
    return *this;
}

/*
 * Multiply operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator *(double multiply) const
{
    PQ copy = *derived();
    copy *= multiply;
    return copy;
}

/*
 * Divide operator /=
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator /=(double divide)
{
    this->setUnitValue(this->m_unitValueD / divide);
    return *this;
}

/*
 * Divide operator /
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator /(double divide) const
{
    PQ copy = *derived();
    copy /= divide;
    return copy;
}

/*
 * Less operator <
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator <(const CPhysicalQuantity<MU, PQ> &other) const
{
    if ((*this) == other) return false;

    // == considers epsilon, so we now have a diff > epsilon here
    double diff = this->m_convertedSiUnitValueD - other.m_convertedSiUnitValueD;
    return (diff < 0);
}

/*
 * Greater than
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator >(const CPhysicalQuantity<MU, PQ> &other) const
{
    if (this == &other) return false;
    return other < *this;
}

/*
 * Greater / Equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator >=(const CPhysicalQuantity<MU, PQ> &other) const
{
    if (this == &other) return true;
    return !(*this < other);
}

/*
 * Less equal
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator <=(const CPhysicalQuantity<MU, PQ> &other) const
{
    if (this == &other) return true;
    return !(*this > other);
}

/*
 * Switch to another unit
 */
template <class MU, class PQ> PQ &CPhysicalQuantity<MU, PQ>::switchUnit(const MU &newUnit)
{
    if (this->m_unit != newUnit)
    {
        double cf = this->m_unit.conversionToUnit(this->m_unitValueD, newUnit);
        this->m_unit = newUnit;
        this->setUnitValue(cf);
    }
    return *derived();
}

/*
 * Init by integer
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setUnitValue(qint32 baseValue)
{
    this->m_unitValueI = baseValue;
    this->m_unitValueD = double(baseValue);
    this->m_isIntegerBaseValue = true;
    this->setConversionSiUnitValue();
}

/*
 * Init by double
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setUnitValue(double baseValue)
{
    this->m_unitValueD = baseValue;
    this->m_unitValueI = qRound(baseValue);
    this->m_isIntegerBaseValue = false;
    this->setConversionSiUnitValue();
}

/*
 * Set SI value
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setConversionSiUnitValue()
{
    double si = this->m_unit.convertToSiConversionUnit(this->m_unitValueD);
    this->m_convertedSiUnitValueD = si;
}

/*
 * Round
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::unitValueToDoubleRounded(int digits) const
{
    return this->m_unit.valueRounded(this->m_unitValueD, digits);
}

/*
 * Rounded value to QString
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::unitValueToQStringRounded(int digits) const
{
    return this->m_unit.toQStringRounded(this->m_unitValueD, digits);
}

/*
 * Rounded with unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::unitValueRoundedWithUnit(int digits, bool i18n) const
{
    return this->m_unit.valueRoundedWithUnit(this->m_unitValueD, digits, i18n);
}

/*
 * Rounded SI value to QString
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::convertedSiValueToQStringRounded(int digits) const
{
    if (digits < 1) digits = this->m_conversionSiUnit.getDisplayDigits();
    return this->m_conversionSiUnit.toQStringRounded(this->m_convertedSiUnitValueD, digits);
}

/*
 * SI base unit value with unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::convertedSiValueRoundedWithUnit(int digits, bool i18n) const
{
    return this->m_conversionSiUnit.valueRoundedWithUnit(this->m_convertedSiUnitValueD, digits, i18n);
}

/*
 * Value rounded in unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(const MU &unit, int digits, bool i18n) const
{
    if (unit == this->m_unit) return this->unitValueRoundedWithUnit(digits, i18n);
    if (unit == this->m_conversionSiUnit) return this->convertedSiValueRoundedWithUnit(digits, i18n);
    return unit.valueRoundedWithUnit(this->value(unit), digits, i18n);
}

/*
 * Value rounded in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::valueRounded(const MU &unit, int digits) const
{
    return unit.valueRounded(this->value(unit), digits);
}

/*
 * Value in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::value(const MU &unit) const
{
    if (unit == this->m_unit) return this->m_unitValueD;
    if (unit == this->m_conversionSiUnit) return this->m_convertedSiUnitValueD;
    double v = unit.convertFromSiConversionUnit(this->m_convertedSiUnitValueD);
    return v;
}

/*
 * Round utility method
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::convertedSiValueToDoubleRounded(int digits) const
{
    if (digits < 1) digits = this->m_conversionSiUnit.getDisplayDigits();
    return this->m_conversionSiUnit.valueRounded(this->m_convertedSiUnitValueD, digits);
}

// see here for the reason of thess forward instantiations
// http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
template class CPhysicalQuantity<CLengthUnit, CLength>;
template class CPhysicalQuantity<CPressureUnit, CPressure>;
template class CPhysicalQuantity<CFrequencyUnit, CFrequency>;
template class CPhysicalQuantity<CMassUnit, CMass>;
template class CPhysicalQuantity<CTemperatureUnit, CTemperature>;
template class CPhysicalQuantity<CSpeedUnit, CSpeed>;
template class CPhysicalQuantity<CAngleUnit, CAngle>;
template class CPhysicalQuantity<CTimeUnit, CTime>;
template class CPhysicalQuantity<CAccelerationUnit, CAcceleration>;

} // namespace
} // namespace

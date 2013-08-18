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
 * Constructor by double
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(double value, const MU &unit) :
    m_value(value), m_unit(unit)
{
}

/*
 * Copy constructor
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ>::CPhysicalQuantity(const CPhysicalQuantity &other) :
    m_value(other.m_value), m_unit(other.m_unit)
{
}

/*
 * Equal operator ==
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator ==(const CPhysicalQuantity<MU, PQ> &other) const
{
    if (this == &other) return true;
    double diff = abs(this->m_value - other.value(this->m_unit));
    return diff <= this->m_unit.getEpsilon();
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

    this->m_value = other.m_value;
    this->m_unit = other.m_unit;
    return *this;
}

/*
 * Plus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator +=(const CPhysicalQuantity<MU, PQ> &other)
{
    this->m_value += other.value(this->m_unit);
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
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::addValueSameUnit(double value)
{
    this->m_value += value;
}

/*
 * Explicit minus
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::substractValueSameUnit(double value)
{
    this->m_value -= value;
}

/*
 * Minus operator
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator -=(const CPhysicalQuantity<MU, PQ> &other)
{
    this->m_value -= other.value(this->m_unit);
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
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator *=(double factor)
{
    this->m_value *= factor;
    return *this;
}

/*
 * Multiply operator
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator *(double factor) const
{
    PQ copy = *derived();
    copy *= factor;
    return copy;
}

/*
 * Divide operator /=
 */
template <class MU, class PQ> CPhysicalQuantity<MU, PQ> &CPhysicalQuantity<MU, PQ>::operator /=(double divisor)
{
    this->m_value /= divisor;
    return *this;
}

/*
 * Divide operator /
 */
template <class MU, class PQ> PQ CPhysicalQuantity<MU, PQ>::operator /(double divisor) const
{
    PQ copy = *derived();
    copy /= divisor;
    return copy;
}

/*
 * Less operator <
 */
template <class MU, class PQ> bool CPhysicalQuantity<MU, PQ>::operator <(const CPhysicalQuantity<MU, PQ> &other) const
{
    if ((*this) == other) return false;

    return (this->m_value < other.value(this->m_unit));
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
        this->m_value = newUnit.convertFrom(this->m_value, this->m_unit);
        this->m_unit = newUnit;
    }
    return *derived();
}

/*
 * Init by double
 */
template <class MU, class PQ> void CPhysicalQuantity<MU, PQ>::setValueSameUnit(double baseValue)
{
    this->m_value = baseValue;
}

/*
 * Value rounded in unit
 */
template <class MU, class PQ> QString CPhysicalQuantity<MU, PQ>::valueRoundedWithUnit(const MU &unit, int digits, bool i18n) const
{
    return unit.makeRoundedQStringWithUnit(this->value(unit), digits, i18n);
}

/*
 * Value rounded in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::valueRounded(const MU &unit, int digits) const
{
    return unit.roundValue(this->value(unit), digits);
}

/*
 * Value in unit
 */
template <class MU, class PQ> double CPhysicalQuantity<MU, PQ>::value(const MU &unit) const
{
    return unit.convertFrom(this->m_value, this->m_unit);
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

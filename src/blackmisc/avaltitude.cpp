/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avaltitude.h"

using BlackMisc::PhysicalQuantities::CLength;
using BlackMisc::PhysicalQuantities::CLengthUnit;

namespace BlackMisc
{
namespace Aviation
{

/**
 * Own implementation for streaming
 */
QString CAltitude::stringForStreamingOperator() const
{
    QString s = CLength::stringForStreamingOperator();
    return s.append(this->m_msl ? " MSL" : " AGL");
}

/**
 * Assigment
 */
CAltitude& CAltitude::operator =(const CAltitude &otherAltitude)
{
    // Check for self-assignment!
    if (this == &otherAltitude)  return *this;
    CLength::operator = (otherAltitude);
    this->m_msl = otherAltitude.m_msl;
    return (*this);
}

/**
 * Equal?
 */
bool CAltitude::operator ==(const CAltitude &otherAltitude)
{
    return otherAltitude.m_msl == this->m_msl && CLength::operator ==(otherAltitude);
}

/**
 * Unequal?
 */
bool CAltitude::operator !=(const CAltitude &otherAltitude)
{
    return !((*this) == otherAltitude);
}

} // namespace
} // namespace

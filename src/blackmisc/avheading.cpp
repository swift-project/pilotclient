/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avheading.h"

namespace BlackMisc
{
namespace Aviation
{

/**
 * Own implementation for streaming
 */
QString CHeading::stringForStreamingOperator() const
{
    QString s = CAngle::stringForStreamingOperator();
    return s.append(this->m_magnetic ? " magnetic" : " true");
}

/**
 * Assigment
 */
CHeading& CHeading::operator =(const CHeading &otherHeading)
{
    // Check for self-assignment!
    if (this == &otherHeading)  return *this;
    CAngle::operator = (otherHeading);
    this->m_magnetic = otherHeading.m_magnetic;
    return (*this);
}

/**
 * Equal?
 */
bool CHeading::operator ==(const CHeading &otherHeading)
{
    return otherHeading.m_magnetic == this->m_magnetic && CAngle::operator ==(otherHeading);
}

/**
 * Unequal?
 */
bool CHeading::operator !=(const CHeading &otherHeading)
{
    return !((*this) == otherHeading);
}

} // namespace
} // namespace

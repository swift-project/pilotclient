/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avheading.h"

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{
namespace Aviation
{

/*
 * Own implementation for streaming
 */
QString CHeading::convertToQString(bool i18n) const
{
    QString s = CAngle::convertToQString(i18n);
    return s.append(this->isMagneticHeading() ? " magnetic" : " true");
}

/*
 * Equal?
 */
bool CHeading::operator ==(const CHeading &other)
{
    return other.m_north == this->m_north && this->CAngle::operator ==(other);
}

/*
 * Unequal?
 */
bool CHeading::operator !=(const CHeading &other)
{
    return !((*this) == other);
}

/*!
 * \brief Register metadata of unit and quantity
 */
void CHeading::registerMetadata()
{
    qRegisterMetaType<CHeading>(typeid(CHeading).name());
    qDBusRegisterMetaType<CHeading>();
}

} // namespace
} // namespace

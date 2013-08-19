/*  Copyright (C) 2013 VATSIM Community
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avtrack.h"

using BlackMisc::PhysicalQuantities::CAngle;
using BlackMisc::PhysicalQuantities::CAngleUnit;

namespace BlackMisc
{
namespace Aviation
{

/*
 * Own implementation for streaming
 */
QString CTrack::convertToQString(bool i18n) const
{
    QString s = CAngle::convertToQString(i18n);
    return s.append(this->m_magnetic ? " magnetic" : " true");
}

/*
 * Equal?
 */
bool CTrack::operator ==(const CTrack &other)
{
    return other.m_magnetic == this->m_magnetic && this->CAngle::operator ==(other);
}

/*
 * Unequal?
 */
bool CTrack::operator !=(const CTrack &other)
{
    return !((*this) == other);
}

/*!
 * \brief Register metadata of unit and quantity
 */
void CTrack::registerMetadata()
{
    qRegisterMetaType<CTrack>(typeid(CTrack).name());
    qDBusRegisterMetaType<CTrack>();
}

} // namespace
} // namespace

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

/*
 * Own implementation for streaming
 */
QString CAltitude::convertToQString(bool /* i18n */) const
{
    QString s = this->CLength::convertToQString();
    return s.append(this->m_msl ? " MSL" : " AGL");
}

/*
 * Assigment
 */
CAltitude &CAltitude::operator =(const CAltitude &other)
{
    if (this == &other) return *this;
    CLength::operator = (other);
    this->m_msl = other.m_msl;
    return *this;
}

/*
 * Equal?
 */
bool CAltitude::operator ==(const CAltitude &other)
{
    return other.m_msl == this->m_msl && this->CLength::operator ==(other);
}

/*
 * Unequal?
 */
bool CAltitude::operator !=(const CAltitude &other)
{
    return !((*this) == other);
}

/*
 * Register metadata
 */
void CAltitude::registerMetadata()
{
    qRegisterMetaType<CAltitude>(typeid(CAltitude).name());
    qDBusRegisterMetaType<CAltitude>();
}

} // namespace
} // namespace

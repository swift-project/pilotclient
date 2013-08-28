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
    return s.append(this->isMeanSeaLevel() ? " MSL" : " AGL");
}

/*
 * Equal?
 */
bool CAltitude::operator ==(const CAltitude &other)
{
    return other.m_datum == this->m_datum && this->CLength::operator ==(other);
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

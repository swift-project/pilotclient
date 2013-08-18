/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/coordinategeodetic.h"

namespace BlackMisc
{
namespace Geo
{

/*
 * String for converter
 */
QString CCoordinateGeodetic::convertToQString(bool i18n) const
{
    QString s = "Geodetic: {%1, %2, %3}";
    return s.arg(this->m_latitude.valueRoundedWithUnit(6, i18n)).arg(this->m_longitude.valueRoundedWithUnit(6, i18n)).arg(this->m_height.valueRoundedWithUnit(i18n));
}

/*
 * Marshall to Dbus
 */
void CCoordinateGeodetic::marshallToDbus(QDBusArgument &argument) const
{
    argument << this->m_latitude;
    argument << this->m_longitude;
    argument << this->m_height;
}

/*
 * Unmarshall from Dbus
 */
void CCoordinateGeodetic::unmarshallFromDbus(const QDBusArgument &argument)
{
    argument >> this->m_latitude;
    argument >> this->m_longitude;
    argument >> this->m_height;
}

/*
 * Register metadata
 */
void CCoordinateGeodetic::registerMetadata()
{
    qRegisterMetaType<CCoordinateGeodetic>(typeid(CCoordinateGeodetic).name());
    qDBusRegisterMetaType<CCoordinateGeodetic>();
}

} // namespace
} // namespace

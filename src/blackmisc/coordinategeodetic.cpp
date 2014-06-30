/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "mathematics.h"
#include <QtCore/qmath.h>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

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
            return s.arg(this->m_latitude.valueRoundedWithUnit(6, i18n)).arg(this->m_longitude.valueRoundedWithUnit(6, i18n)).arg(this->m_height.valueRoundedWithUnit(6, i18n));
        }

        /*
         * metaTypeId
         */
        int CCoordinateGeodetic::getMetaTypeId() const
        {
            return qMetaTypeId<CCoordinateGeodetic>();
        }

        /*
         * is a
         */
        bool CCoordinateGeodetic::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CCoordinateGeodetic>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CCoordinateGeodetic::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CCoordinateGeodetic &>(otherBase);

            return compare(TupleConverter<CCoordinateGeodetic>::toTuple(*this), TupleConverter<CCoordinateGeodetic>::toTuple(other));
        }

        /*
         * Marshall to Dbus
         */
        void CCoordinateGeodetic::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CCoordinateGeodetic>::toTuple(*this);
        }

        /*
         * Unmarshall from Dbus
         */
        void CCoordinateGeodetic::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CCoordinateGeodetic>::toTuple(*this);
        }

        /*
         * Same coordinate
         */
        bool CCoordinateGeodetic::operator ==(const CCoordinateGeodetic &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CCoordinateGeodetic>::toTuple(*this) == TupleConverter<CCoordinateGeodetic>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CCoordinateGeodetic::operator !=(const CCoordinateGeodetic &other) const
        {
            return !((*this) == other);
        }

        /*
         * Register metadata
         */
        void CCoordinateGeodetic::registerMetadata()
        {
            qRegisterMetaType<CCoordinateGeodetic>();
            qDBusRegisterMetaType<CCoordinateGeodetic>();
        }

        /*
         * Hash
         */
        uint CCoordinateGeodetic::getValueHash() const
        {
            return qHash(TupleConverter<CCoordinateGeodetic>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CCoordinateGeodetic::toJson() const
        {
            return BlackMisc::serializeJson(CCoordinateGeodetic::jsonMembers(), TupleConverter<CCoordinateGeodetic>::toTuple(*this));
        }

        /*
         * To JSON
         */
        void CCoordinateGeodetic::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CCoordinateGeodetic::jsonMembers(), TupleConverter<CCoordinateGeodetic>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CCoordinateGeodetic::jsonMembers()
        {
            return TupleConverter<CCoordinateGeodetic>::jsonMembers();
        }

        /*
         * From WGS84 coordinates
         */
        CCoordinateGeodetic CCoordinateGeodetic::fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const CLength height)
        {
            CLatitude lat = CLatitude::fromWgs84(latitudeWgs84);
            CLongitude lon = CLongitude::fromWgs84(longitudeWgs84);
            return CCoordinateGeodetic(lat, lon, height);
        }

        /*
         * Great circle distance
         */
        PhysicalQuantities::CLength greatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
        {
            // same coordinate results in 0 distance
            if (coordinate1.latitude() == coordinate2.latitude() && coordinate1.longitude() == coordinate2.longitude())
            {
                return CLength(0, CLengthUnit::NM());
            }

            // first, prelimary distance calculation
            // http://www.geodatasource.com/developers/c
            double dist;
            double lon1rad = coordinate1.longitude().value(CAngleUnit::rad());
            double lon2rad = coordinate2.longitude().value(CAngleUnit::rad());
            double lat1rad = coordinate1.latitude().value(CAngleUnit::rad());
            double lat2rad = coordinate2.latitude().value(CAngleUnit::rad());
            double theta = lon1rad - lon2rad;
            dist = qSin(lat1rad) * qSin(lat2rad) + qCos(lat1rad) * qCos(lat2rad) * cos(theta);
            dist = qAcos(dist);
            dist = CMath::rad2deg(dist);
            dist = dist * 60; // dist in NM
            return CLength(qAbs(dist), CLengthUnit::NM());
        }

    } // namespace
} // namespace

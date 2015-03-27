/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "mathematics.h"
#include "variant.h"
#include <QtCore/qmath.h>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Math;

namespace BlackMisc
{
    namespace Geo
    {

        QString CCoordinateGeodetic::convertToQString(bool i18n) const
        {
            QString s = "Geodetic: {%1, %2, %3}";
            return s.arg(this->m_latitude.valueRoundedWithUnit(6, i18n)).arg(this->m_longitude.valueRoundedWithUnit(6, i18n)).arg(this->m_geodeticHeight.valueRoundedWithUnit(6, i18n));
        }

        CCoordinateGeodetic CCoordinateGeodetic::fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const CLength &geodeticHeight)
        {
            CLatitude lat = CLatitude::fromWgs84(latitudeWgs84);
            CLongitude lon = CLongitude::fromWgs84(longitudeWgs84);
            return CCoordinateGeodetic(lat, lon, geodeticHeight);
        }

        PhysicalQuantities::CLength calculateGreatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
        {
            // same coordinate results in 0 distance
            if (coordinate1.latitude() == coordinate2.latitude() && coordinate1.longitude() == coordinate2.longitude())
            {
                return CLength(0, CLengthUnit::m());
            }

            // first, prelimary distance calculation
            // http://www.movable-type.co.uk/scripts/latlong.html
            double earthRadiusM = 6371000.8;
            double lon1rad = coordinate1.longitude().value(CAngleUnit::rad());
            double lon2rad = coordinate2.longitude().value(CAngleUnit::rad());
            double lat1rad = coordinate1.latitude().value(CAngleUnit::rad());
            double lat2rad = coordinate2.latitude().value(CAngleUnit::rad());

            double dLat = lat2rad - lat1rad;
            double dLon = lon2rad - lon1rad;
            double a = qSin(dLat / 2.0) * qSin(dLat / 2.0) +
                       qCos(lat1rad) * qCos(lat2rad) * qSin(dLon / 2.0) * qSin(dLon / 2.0);
            double c = 2.0 * qAtan(qSqrt(a) / qSqrt(1.0 - a));
            double distance = earthRadiusM * c;

            return CLength(distance, CLengthUnit::m());
        }

        PhysicalQuantities::CAngle calculateBearing(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2)
        {
            // same coordinate results in 0 distance
            if (coordinate1.latitude() == coordinate2.latitude() && coordinate1.longitude() == coordinate2.longitude())
            {
                return CAngle(0, CAngleUnit::deg());
            }

            // http://www.yourhomenow.com/house/haversine.html
            double lon1rad = coordinate1.longitude().value(CAngleUnit::rad());
            double lon2rad = coordinate2.longitude().value(CAngleUnit::rad());
            double lat1rad = coordinate1.latitude().value(CAngleUnit::rad());
            double lat2rad = coordinate2.latitude().value(CAngleUnit::rad());
            double dLon = lon1rad - lon2rad;
            double y = qSin(dLon) * qCos(lat2rad);
            double x = qCos(lat1rad) * qSin(lat2rad) -
                       qSin(lat1rad) * qCos(lat2rad) * qCos(dLon);
            double bearing = qAtan2(y, x);
            bearing = CMath::rad2deg(bearing); // now in deg
            bearing = CMath::normalizeDegrees(bearing); // normalize
            return CAngle(bearing, CAngleUnit::deg());
        }

        CLength ICoordinateGeodetic::calculateGreatCircleDistance(const ICoordinateGeodetic &otherCoordinate) const
        {
            return Geo::calculateGreatCircleDistance((*this), otherCoordinate);
        }

        CAngle ICoordinateGeodetic::bearing(const ICoordinateGeodetic &otherCoordinate) const
        {
            return Geo::calculateBearing((*this), otherCoordinate);
        }

        CVariant ICoordinateGeodetic::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (!index.isMyself())
            {
                ColumnIndex i = index.frontCasted<ColumnIndex>();
                switch (i)
                {
                case IndexLatitude:
                    return this->latitude().propertyByIndex(index.copyFrontRemoved());
                case IndexLongitude:
                    return this->longitude().propertyByIndex(index.copyFrontRemoved());
                case IndexLatitudeAsString:
                    return CVariant(this->latitudeAsString());
                case IndexLongitudeAsString:
                    return CVariant(this->longitudeAsString());
                case IndexGeodeticHeight:
                    return this->geodeticHeight().propertyByIndex(index.copyFrontRemoved());
                case IndexGeodeticHeightAsString:
                    return CVariant(this->geodeticHeightAsString());
                default:
                    break;
                }
            }

            const QString m = QString("no property, index ").append(index.toQString());
            Q_ASSERT_X(false, "ICoordinateGeodetic", m.toLocal8Bit().constData());
            return CVariant::fromValue(m);
        }

        CVariant CCoordinateGeodetic::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            if (ICoordinateGeodetic::canHandleIndex(index))
            {
                return ICoordinateGeodetic::propertyByIndex(index);
            }
            else
            {
                return CValueObject::propertyByIndex(index);
            }
        }

        void CCoordinateGeodetic::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ICoordinateGeodetic::ColumnIndex i = index.frontCasted<ICoordinateGeodetic::ColumnIndex>();
            switch (i)
            {
            case IndexGeodeticHeight:
                this->m_geodeticHeight.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexLatitude:
                this->m_latitude.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexLongitude:
                this->m_longitude.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexLatitudeAsString:
                this->setLatitude(CLatitude::fromWgs84(variant.toQString()));
                break;
            case IndexLongitudeAsString:
                this->setLongitude(CLongitude::fromWgs84(variant.toQString()));
                break;
            case IndexGeodeticHeightAsString:
                this->m_geodeticHeight.parseFromString(variant.toQString());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        CCoordinateGeodetic &CCoordinateGeodetic::switchUnit(const CAngleUnit &unit)
        {
            this->m_latitude.switchUnit(unit);
            this->m_longitude.switchUnit(unit);
            return *this;
        }

        CCoordinateGeodetic &CCoordinateGeodetic::switchUnit(const CLengthUnit &unit)
        {
            this->m_geodeticHeight.switchUnit(unit);
            return *this;
        }

        CLength ICoordinateWithRelativePosition::calculcateDistanceToOwnAircraft(const ICoordinateGeodetic &position, bool updateValues)
        {
            if (!updateValues) { return Geo::calculateGreatCircleDistance(*this, position); }
            this->m_distanceToOwnAircraft = Geo::calculateGreatCircleDistance(*this, position);
            return this->m_distanceToOwnAircraft;
        }

        CLength ICoordinateWithRelativePosition::calculcateDistanceAndBearingToOwnAircraft(const ICoordinateGeodetic &position, bool updateValues)
        {
            if (!updateValues) { return Geo::calculateGreatCircleDistance(*this, position); }
            this->m_distanceToOwnAircraft = Geo::calculateGreatCircleDistance(*this, position);
            this->m_bearingToOwnAircraft = Geo::calculateBearing(*this, position);
            return this->m_distanceToOwnAircraft;
        }

        ICoordinateWithRelativePosition::ICoordinateWithRelativePosition()
        { }

    } // namespace
} // namespace

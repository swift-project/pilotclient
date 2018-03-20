/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/propertyindex.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Geo
    {
        QString CElevationPlane::convertToQString(bool i18n) const
        {
            static const QString s = "Geodetic: {%1, %2, %3} radius: %4";
            return s.arg(this->latitude().valueRoundedWithUnit(6, i18n),
                         this->longitude().valueRoundedWithUnit(6, i18n),
                         this->geodeticHeight().valueRoundedWithUnit(6, i18n),
                         m_radius.valueRoundedWithUnit(2, i18n));
        }

        const CAltitude &CElevationPlane::getAltitudeIfWithinRadius(const ICoordinateGeodetic &coordinate) const
        {
            return (isWithinRange(coordinate)) ? geodeticHeight() : CAltitude::null();
        }

        CAltitude CElevationPlane::getAltitudeInUnit(const CLengthUnit &unit) const
        {
            CAltitude a = this->geodeticHeight();
            a.switchUnit(unit);
            return a;
        }

        bool CElevationPlane::isNull() const
        {
            return m_radius.isNull() || CCoordinateGeodetic::isNull();
        }

        bool CElevationPlane::isWithinRange(const ICoordinateGeodetic &coordinate) const
        {
            if (coordinate.isNull()) { return false; }
            if (isNull()) { return false; }
            const CLength d = this->calculateGreatCircleDistance(coordinate);
            const bool inRange = (m_radius >= d);
            return inRange;
        }

        bool CElevationPlane::isWithinRange(const ICoordinateGeodetic &coordinate, const CLength &radius) const
        {
            if (coordinate.isNull()) { return false; }
            if (radius.isNull()) { return false; }
            if (this->isNull()) { return false; }
            const CLength d = this->calculateGreatCircleDistance(coordinate);
            const bool inRange = (radius >= d);
            return inRange;
        }

        void CElevationPlane::setSinglePointRadius()
        {
            m_radius = singlePointRadius();
        }

        void CElevationPlane::setMinorAirportRadius()
        {
            m_radius = minorAirportRadius();
        }

        void CElevationPlane::setMajorAirportRadius()
        {
            m_radius = majorAirportRadius();
        }

        CVariant CElevationPlane::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRadius: return m_radius.propertyByIndex(index.copyFrontRemoved());
            default: break;
            }
            return CCoordinateGeodetic::propertyByIndex(index);
        }

        void CElevationPlane::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CElevationPlane>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexRadius: m_radius.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            default: CCoordinateGeodetic::setPropertyByIndex(index, variant); break;
            }
        }

        int CElevationPlane::comparePropertyByIndex(const CPropertyIndex &index, const CElevationPlane &elevationPlane) const
        {
            return this->getAltitude().comparePropertyByIndex(index, elevationPlane.getAltitude());
        }

        // 100km/h 27,8m/s
        //  50km/h 13,9m/s
        // 100kts  51,4m/s
        const CLength &CElevationPlane::singlePointRadius()
        {
            static const CLength l(25.0, CLengthUnit::m());
            return l;
        }

        const CLength &CElevationPlane::minorAirportRadius()
        {
            static const CLength l(500.0, CLengthUnit::m());
            return l;
        }

        const CLength &CElevationPlane::majorAirportRadius()
        {
            static const CLength l(1000.0, CLengthUnit::m());
            return l;
        }

        const CElevationPlane &CElevationPlane::null()
        {
            static const CElevationPlane p;
            return p;
        }
    } // namespace
} // namespace

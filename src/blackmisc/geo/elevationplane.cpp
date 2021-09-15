/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/propertyindexref.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc::Geo
{
    QString CElevationPlane::convertToQString(bool i18n) const
    {
        const QString coordinate = CCoordinateGeodetic::toQString(i18n);
        return QStringLiteral("%1 radius: %2").arg(coordinate, m_radius.valueRoundedWithUnit(2, i18n));
    }

    CElevationPlane::CElevationPlane(const ICoordinateGeodetic &coordinate, const ICoordinateGeodetic &rangeCoordinate) :
        CCoordinateGeodetic(coordinate)
    {
        this->setRadiusOrMinimumRadius(this->calculateGreatCircleDistance(rangeCoordinate));
    }

    CElevationPlane::CElevationPlane(const ICoordinateGeodetic &coordinate, const CLength &radius) :
        CCoordinateGeodetic(coordinate), m_radius(radius)
    { }

    CElevationPlane::CElevationPlane(const ICoordinateGeodetic &coordinate, const CAltitude &altitude, const CLength &radius) :
        CCoordinateGeodetic(coordinate), m_radius(radius)
    {
        this->setGeodeticHeight(altitude);
    }

    CElevationPlane::CElevationPlane(const ICoordinateGeodetic &coordinate, double altitudeMSLft, const CLength &radius) :
        CCoordinateGeodetic(coordinate), m_radius(radius)
    {
        this->setGeodeticHeight(CAltitude(altitudeMSLft, CAltitude::MeanSeaLevel, CLengthUnit::ft()));
    }

    CElevationPlane::CElevationPlane(double latDeg, double lngDeg, double altitudeMSLft, const CLength &radius) :
        CCoordinateGeodetic(latDeg, lngDeg, altitudeMSLft), m_radius(radius)
    {
        Q_ASSERT_X(!std::isnan(altitudeMSLft), Q_FUNC_INFO, "elv.nan");
    }

    CElevationPlane::CElevationPlane(const CLatitude &lat, const CLongitude &lng, const CAltitude &altitude, const CLength &radius) :
        CCoordinateGeodetic(lat, lng, altitude), m_radius(radius)
    {
        Q_ASSERT_X(altitude.isMeanSeaLevel(), Q_FUNC_INFO, "Need MSL");
    }

    const CLength &CElevationPlane::getRadiusOrMinimumRadius() const
    {
        if (m_radius.isNull() || m_radius < CElevationPlane::singlePointRadius()) { return CElevationPlane::singlePointRadius(); }
        return m_radius;
    }

    void CElevationPlane::setRadiusOrMinimumRadius(const CLength &radius)
    {
        m_radius = ((radius.isNull() || radius < CElevationPlane::singlePointRadius())) ? CElevationPlane::singlePointRadius() : radius;
    }

    void CElevationPlane::fixRadius()
    {
        if (m_radius.isNull() || m_radius < CElevationPlane::singlePointRadius())
        {
            m_radius = CElevationPlane::singlePointRadius();
        }
    }

    void CElevationPlane::addAltitudeOffset(const CLength &offset)
    {
        if (offset.isNull() || offset.isZeroEpsilonConsidered()) { return; }
        const CAltitude newAlt = this->getAltitude().withOffset(offset);
        this->setGeodeticHeight(newAlt);
    }

    void CElevationPlane::switchAltitudeUnit(const CLengthUnit &unit)
    {
        if (unit.isNull() || this->getAltitudeUnit().isNull()) { return; }
        if (this->getAltitudeUnit() == unit) { return; }
        this->setGeodeticHeight(this->getAltitude().switchedUnit(unit));
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

    double CElevationPlane::getAltitudeValue(const CLengthUnit &unit) const
    {
        return this->getAltitude().value(unit);
    }

    bool CElevationPlane::isNull() const
    {
        return m_radius.isNull() || CCoordinateGeodetic::isNull();
    }

    bool CElevationPlane::isWithinRange(const ICoordinateGeodetic &coordinate) const
    {
        if (coordinate.isNull()) { return false; }
        if (this->isNull()) { return false; }
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

    QVariant CElevationPlane::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexRadius: return m_radius.propertyByIndex(index.copyFrontRemoved());
        default: break;
        }
        return CCoordinateGeodetic::propertyByIndex(index);
    }

    void CElevationPlane::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CElevationPlane>(); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexRadius: m_radius.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        default: CCoordinateGeodetic::setPropertyByIndex(index, variant); break;
        }
    }

    int CElevationPlane::comparePropertyByIndex(CPropertyIndexRef index, const CElevationPlane &elevationPlane) const
    {
        Q_UNUSED(index)
        return this->getAltitude().compare(elevationPlane.getAltitude());
    }

    // distance per second     distance per 5 secs (taxi speeds)
    // 100km/h 27,8m/s         10kts   51m/5secs
    //  50km/h 13,9m/s         20kts  102m/5secs
    // 100kts  51,4m/s
    const CLength &CElevationPlane::singlePointRadius()
    {
        static const CLength l(50.0, CLengthUnit::m());
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

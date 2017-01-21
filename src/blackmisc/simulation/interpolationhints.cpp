/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationhints.h"
#include "blackmisc/aviation/aircraftsituation.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolationHints::CInterpolationHints() { }

        CInterpolationHints::CInterpolationHints(bool isVtolAircraft) : m_isVtol(isVtolAircraft)
        { }

        CAltitude CInterpolationHints::getGroundElevation(const Aviation::CAircraftSituation &situation) const
        {
            if (m_elevationProvider) { return m_elevationProvider(situation); }
            if (m_elevation.isNull() || !m_elevation.isWithinRange(situation)) { return CAltitude::null(); }
            return m_elevation.geodeticHeight();
        }

        void CInterpolationHints::resetElevation()
        {
            m_elevation = CElevationPlane();
        }

        bool CInterpolationHints::isWithinRange(const Geo::ICoordinateGeodetic &coordinate) const
        {
            if (m_elevation.isNull()) return false;
            return m_elevation.isWithinRange(coordinate);
        }

        void CInterpolationHints::setAircraftParts(const CAircraftParts &parts, bool hasParts)
        {
            m_hasParts = hasParts;
            m_aircraftParts = parts;
        }

        CVariant CInterpolationHints::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCenterOfGravity:
                return this->m_cgAboveGround.propertyByIndex(index.copyFrontRemoved());
            case IndexElevationPlane:
                return this->m_elevation.propertyByIndex(index.copyFrontRemoved());
            case IndexIsVtolAircraft:
                return CVariant::fromValue(m_isVtol);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CInterpolationHints::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CInterpolationHints>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCenterOfGravity:
                this->m_cgAboveGround.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexElevationPlane:
                this->m_elevation.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexIsVtolAircraft:
                this->m_isVtol = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CInterpolationHints::convertToQString(bool i18n) const
        {
            static const QString s("%1 %2");
            return s.arg(m_elevation.toQString(i18n), m_cgAboveGround.toQString(i18n));
        }

        QString CInterpolationHints::debugInfo(const Geo::CElevationPlane &deltaElevation) const
        {
            static const QString s("Lat: %1 Lng: %2 Elv: %3");
            if (m_elevation.isNull() || deltaElevation.isNull()) return "null";
            return s.arg(
                       (deltaElevation.latitude() - m_elevation.latitude()).valueRoundedWithUnit(CAngleUnit::deg(), 10),
                       (deltaElevation.longitude() - m_elevation.longitude()).valueRoundedWithUnit(CAngleUnit::deg(), 10),
                       (deltaElevation.geodeticHeight() - m_elevation.geodeticHeight()).valueRoundedWithUnit(CLengthUnit::ft(), 2)
                   );
        }
    } // namespace
} // namespace

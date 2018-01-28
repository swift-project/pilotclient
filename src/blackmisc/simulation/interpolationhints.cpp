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

        CInterpolationHints::CInterpolationHints(bool isVtolAircraft, bool hasParts, bool log) :
            m_isVtol(isVtolAircraft), m_hasParts(hasParts), m_logInterpolation(log)
        { }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, bool useProvider, bool forceProvider) const
        {
            const bool validPlane = m_elevationPlane.isWithinRange(situation);
            Q_ASSERT_X(!(forceProvider && !useProvider), Q_FUNC_INFO, "Invalid parameter combination");
            if (forceProvider && useProvider && m_elevationProvider) { return m_elevationProvider(situation); }
            if (!validPlane   && useProvider && m_elevationProvider) { return m_elevationProvider(situation); }
            return validPlane ? this->m_elevationPlane.getAltitude() : CAltitude::null();
        }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, const CLength &validRadius, bool useProvider, bool forceProvider) const
        {
            const bool validPlane = m_elevationPlane.isWithinRange(situation, CLength::maxValue(validRadius, m_elevationPlane.getRadius()));
            Q_ASSERT_X(!(forceProvider && !useProvider), Q_FUNC_INFO, "Invalid parameter combination");
            if (forceProvider && useProvider && m_elevationProvider) { return m_elevationProvider(situation); }
            if (!validPlane   && useProvider && m_elevationProvider) { return m_elevationProvider(situation); }
            return validPlane ? this->m_elevationPlane.getAltitude() : CAltitude::null();
        }

        void CInterpolationHints::resetElevationPlane()
        {
            m_elevationPlane = CElevationPlane();
        }

        bool CInterpolationHints::isWithinRange(const Geo::ICoordinateGeodetic &coordinate) const
        {
            if (m_elevationPlane.isNull()) return false;
            return m_elevationPlane.isWithinRange(coordinate);
        }

        const CLength &CInterpolationHints::getCGAboveGroundOrZero() const
        {
            static const CLength zero;
            return this->hasCGAboveGround() ? this->getCGAboveGround() : zero;
        }

        void CInterpolationHints::setAircraftParts(const CAircraftParts &parts, bool hasParts)
        {
            m_hasParts = hasParts;
            m_aircraftParts = parts;
        }

        bool CInterpolationHints::hasElevationProvider() const
        {
            return static_cast<bool>(m_elevationProvider);
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
                return this->m_elevationPlane.propertyByIndex(index.copyFrontRemoved());
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
                this->m_elevationPlane.setPropertyByIndex(index.copyFrontRemoved(), variant);
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
            return s.arg(m_elevationPlane.toQString(i18n), m_cgAboveGround.toQString(i18n));
        }

        QString CInterpolationHints::debugInfo(const Geo::CElevationPlane &deltaElevation) const
        {
            static const QString s("Lat: %1 Lng: %2 Elv: %3");
            if (m_elevationPlane.isNull() || deltaElevation.isNull()) return "null";
            return s.arg(
                       (deltaElevation.latitude() - m_elevationPlane.latitude()).valueRoundedWithUnit(CAngleUnit::deg(), 10),
                       (deltaElevation.longitude() - m_elevationPlane.longitude()).valueRoundedWithUnit(CAngleUnit::deg(), 10),
                       (deltaElevation.geodeticHeight() - m_elevationPlane.geodeticHeight()).valueRoundedWithUnit(CLengthUnit::ft(), 2)
                   );
        }
    } // namespace
} // namespace

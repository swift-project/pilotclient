/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationhints.h"
#include "interpolationlogger.h"
#include "blackmisc/aviation/aircraftsituation.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

#include <QStringBuilder>

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

        const CElevationPlane &CInterpolationHints::getElevationPlane(ICoordinateGeodetic &reference, const CLength &radius, SituationLog *log) const
        {
            if (m_elevationPlane.isNull())
            {
                if (log)
                {
                    static const QString lm("By provider (no valid plane)");
                    log->elevationInfo = lm;
                }
                return CElevationPlane::null();
            }
            const CLength d = reference.calculateGreatCircleDistance(m_elevationPlane);
            if (d <= radius)
            {
                if (log)
                {
                    static const QString lm("Using elevation plane, distance: %1");
                    log->elevationInfo = lm.arg(d.valueRoundedWithUnit(CLengthUnit::m(), 1));
                }
                return m_elevationPlane;
            }
            else
            {
                if (log)
                {
                    static const QString lm("Invalid elevation plane, distance: %1");
                    log->elevationInfo = lm.arg(d.valueRoundedWithUnit(CLengthUnit::m(), 1));
                }
                return CElevationPlane::null();
            }
        }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, bool useProvider, bool forceProvider, SituationLog *log) const
        {
            return this->getGroundElevation(situation, CLength::null(), useProvider, forceProvider, log);
        }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, const CLength &validRadius, bool useProvider, bool forceProvider, SituationLog *log) const
        {
            Q_ASSERT_X(!(forceProvider && !useProvider), Q_FUNC_INFO, "Invalid parameter combination");
            if (forceProvider && useProvider && m_elevationProvider)
            {
                if (log)
                {
                    static const QString lm("By provider (forced)");
                    log->elevationInfo = lm;
                }
                Q_ASSERT_X(false, Q_FUNC_INFO, "Elevation provider must no longer be used");
                return m_elevationProvider(situation);
            }

            const CLength radius = CLength::maxValue(validRadius, m_elevationPlane.getRadius());
            const bool validPlane = m_elevationPlane.isWithinRange(situation, radius);
            if (!validPlane && useProvider && m_elevationProvider)
            {
                if (log)
                {
                    static const QString lm("By provider (no valid plane)");
                    log->elevationInfo = lm;
                }
                return m_elevationProvider(situation);
            }

            if (validPlane)
            {
                if (log)
                {
                    static const QString lm("Using elevation plane, radius: %1");
                    log->elevationInfo = lm.arg(radius.valueRoundedWithUnit(CLengthUnit::m(), 1));
                }
                return m_elevationPlane.getAltitude();
            }

            if (log)
            {
                static const QString lm("Not using provider, no valid plane");
                log->elevationInfo = lm;
            }

            return CAltitude::null();
        }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, const CLength &validRadius, SituationLog *log) const
        {
            return this->getGroundElevation(situation, validRadius, false, false, log);
        }

        CAltitude CInterpolationHints::getGroundElevation(const CAircraftSituation &situation, SituationLog *log) const
        {
            return this->getGroundElevation(situation, CLength::null(), log);
        }

        void CInterpolationHints::resetElevationPlane()
        {
            m_elevationPlane = CElevationPlane();
        }

        bool CInterpolationHints::isWithinRange(const Geo::ICoordinateGeodetic &coordinate) const
        {
            if (m_elevationPlane.isNull()) { return false; }
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
            case IndexCenterOfGravity: return m_cgAboveGround.propertyByIndex(index.copyFrontRemoved());
            case IndexElevationPlane: return m_elevationPlane.propertyByIndex(index.copyFrontRemoved());
            case IndexIsVtolAircraft: return CVariant::fromValue(m_isVtol);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CInterpolationHints::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CInterpolationHints>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCenterOfGravity:
                m_cgAboveGround.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexElevationPlane:
                m_elevationPlane.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexIsVtolAircraft:
                m_isVtol = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        QString CInterpolationHints::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return this->asString(true, true);
        }

        QString CInterpolationHints::asString(bool withParts, bool withElevationPlane) const
        {
            return
                QStringLiteral("VTOL: ") % boolToYesNo(m_isVtol) %
                QStringLiteral(" parts: ") % boolToYesNo(m_hasParts) %
                (
                    withParts && m_hasParts ?
                    QStringLiteral(" parts: ") % m_aircraftParts.toQString(true) :
                    QStringLiteral("")
                ) %
                QStringLiteral(" CG: ") % m_cgAboveGround.valueRoundedWithUnit(CLengthUnit::m(), 1) %
                (
                    withElevationPlane ?
                    QStringLiteral(" elv.plane: ") % m_elevationPlane.toQString(true) :
                    QStringLiteral(" elv.plane: ") % boolToNullNotNull(m_elevationPlane.isNull())
                ) %
                QStringLiteral(" elv.pr.: ") % boolToYesNo(m_elevationProvider ? true : false);
        }

        QString CInterpolationHints::debugInfo(const CElevationPlane &deltaElevation) const
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

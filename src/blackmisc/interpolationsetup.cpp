/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/interpolationsetup.h"
#include "stringutils.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    CInterpolationAndRenderingSetup::CInterpolationAndRenderingSetup()
    { }

    int CInterpolationAndRenderingSetup::InfiniteAircraft()
    {
        return 100;
    }

    bool CInterpolationAndRenderingSetup::isRenderingEnabled() const
    {
        if (m_maxRenderedAircraft < 1)  { return false; }
        if (!isMaxDistanceRestricted()) { return true; }
        return m_maxRenderedDistance.isPositiveWithEpsilonConsidered();
    }

    int CInterpolationAndRenderingSetup::getMaxRenderedAircraft() const
    {
        return (m_maxRenderedAircraft <= InfiniteAircraft()) ? m_maxRenderedAircraft : InfiniteAircraft();
    }

    bool CInterpolationAndRenderingSetup::setMaxRenderedAircraft(int maxRenderedAircraft)
    {
        if (maxRenderedAircraft == m_maxRenderedAircraft) { return false; }
        if (maxRenderedAircraft < 1)
        {
            // disable, we set both values to 0
            m_maxRenderedAircraft = 0;
            m_maxRenderedDistance = CLength(0.0, CLengthUnit::NM()); // real 0
        }
        else if (maxRenderedAircraft >= InfiniteAircraft())
        {
            m_maxRenderedAircraft = InfiniteAircraft();
        }
        else
        {
            m_maxRenderedAircraft = maxRenderedAircraft;
        }
        return true;
    }

    bool CInterpolationAndRenderingSetup::setMaxRenderedDistance(const CLength &distance)
    {

        if (distance == m_maxRenderedDistance) { return false; }
        if (distance.isNull() || distance.isNegativeWithEpsilonConsidered())
        {
            m_maxRenderedDistance = CLength(0.0, CLengthUnit::nullUnit());
        }
        else if (distance.isZeroEpsilonConsidered())
        {
            // zero means disabled, we disable max aircraft too
            this->m_maxRenderedAircraft = 0;
            this->m_maxRenderedDistance = CLength(0.0, CLengthUnit::NM()); // real 0
        }
        else
        {
            Q_ASSERT(!distance.isNegativeWithEpsilonConsidered());
            m_maxRenderedDistance = distance;
        }
        return true;
    }

    void CInterpolationAndRenderingSetup::disableMaxRenderedDistance()
    {
        this->setMaxRenderedDistance(CLength(0.0, CLengthUnit::nullUnit()));
    }

    bool CInterpolationAndRenderingSetup::isMaxAircraftRestricted() const
    {
        return m_maxRenderedAircraft < InfiniteAircraft();
    }

    void CInterpolationAndRenderingSetup::deleteAllRenderingRestrictions()
    {
        this->m_maxRenderedDistance = CLength(0, CLengthUnit::nullUnit());
        this->m_maxRenderedAircraft = InfiniteAircraft();
    }

    bool CInterpolationAndRenderingSetup::isMaxDistanceRestricted() const
    {
        return !m_maxRenderedDistance.isNull();
    }

    QString CInterpolationAndRenderingSetup::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        QString s("Setup: debug sim: ");
        s += boolToYesNo(this->m_simulatorDebugMessages);
        s += " debug interpolator: ";
        s += boolToYesNo(this->m_interpolatorDebugMessage);
        s += " force full interpolation: ";
        s += boolToYesNo(this->m_forceFullInterpolation);
        s += " max.aircraft:";
        s += QString::number(m_maxRenderedAircraft);
        s += " max.distance:";
        s += m_maxRenderedDistance.valueRoundedWithUnit(CLengthUnit::NM(), 2);
        return s;
    }

    CVariant CInterpolationAndRenderingSetup::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        if (index.isMyself()) { return CVariant::from(*this); }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInterpolatorDebugMessages:
            return CVariant::fromValue(m_interpolatorDebugMessage);
        case IndexSimulatorDebugMessages:
            return CVariant::fromValue(m_simulatorDebugMessages);
        case IndexForceFullInterpolation:
            return CVariant::fromValue(m_forceFullInterpolation);
        case IndexMaxRenderedAircraft:
            return CVariant::fromValue(m_maxRenderedAircraft);
        case IndexMaxRenderedDistance:
            return CVariant::fromValue(m_maxRenderedDistance);
        default:
            return CValueObject::propertyByIndex(index);
        }
    }

    void CInterpolationAndRenderingSetup::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        if (index.isMyself())
        {
            *this = variant.value<CInterpolationAndRenderingSetup>();
            return;
        }
        ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexInterpolatorDebugMessages:
            this->m_interpolatorDebugMessage = variant.toBool();
            break;
        case IndexSimulatorDebugMessages:
            this->m_simulatorDebugMessages = variant.toBool();
            break;
        case IndexForceFullInterpolation:
            this->m_forceFullInterpolation = variant.toBool();
            break;
        case IndexMaxRenderedAircraft:
            this->m_maxRenderedAircraft = variant.toInt();
            break;
        case IndexMaxRenderedDistance:
            this->m_maxRenderedDistance = variant.value<CLength>();
            break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }
} // ns

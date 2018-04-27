/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationrenderingsetup.h"
#include "blackmisc/network/client.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include <QStringBuilder>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    namespace Simulation
    {
        CInterpolationAndRenderingSetupBase::CInterpolationAndRenderingSetupBase()
        { }

        bool CInterpolationAndRenderingSetupBase::setEnabledGndFLag(bool enabled)
        {
            if (enabled == m_enabledGndFlag) { return false; }
            m_enabledGndFlag = enabled;
            return true;
        }

        bool CInterpolationAndRenderingSetupBase::setSendGndFlagToSimulator(bool sendFLag)
        {
            if (sendFLag == m_sendGndToSim) { return false; }
            m_sendGndToSim = sendFLag;
            return true;
        }

        void CInterpolationAndRenderingSetupBase::consolidateWithClient(const CClient &client)
        {
            m_enabledAircraftParts &= client.hasAircraftPartsCapability();
            // m_enabledGndFlag &= client.hasGndFlagCapability();
        }

        bool CInterpolationAndRenderingSetupBase::setEnabledAircraftParts(bool enabled)
        {
            if (m_enabledAircraftParts == enabled) { return false; }
            m_enabledAircraftParts = enabled;
            return true;
        }

        bool CInterpolationAndRenderingSetupBase::maskEnabledAircraftParts(bool mask)
        {
            const bool masked = mask && m_enabledAircraftParts;
            return this->setEnabledAircraftParts(masked);
        }

        CVariant CInterpolationAndRenderingSetupBase::propertyByIndex(const CPropertyIndex &index) const
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLogInterpolation: return CVariant::fromValue(m_logInterpolation);
            case IndexSimulatorDebugMessages: return CVariant::fromValue(m_simulatorDebugMessages);
            case IndexForceFullInterpolation: return CVariant::fromValue(m_forceFullInterpolation);
            case IndexEnabledAircraftParts: return CVariant::fromValue(m_enabledAircraftParts);
            case IndexEnableGndFlag: return CVariant::fromValue(m_enabledGndFlag);
            case IndexSendGndFlagToSimulator: return CVariant::fromValue(m_sendGndToSim);
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "Cannot handle index");
            return QString("Wrong index for %1").arg(i);
        }

        void CInterpolationAndRenderingSetupBase::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexLogInterpolation: m_logInterpolation = variant.toBool(); return;
            case IndexSimulatorDebugMessages: m_simulatorDebugMessages = variant.toBool(); return;
            case IndexForceFullInterpolation: m_forceFullInterpolation = variant.toBool(); return;
            case IndexEnabledAircraftParts: m_enabledAircraftParts = variant.toBool(); return;
            case IndexEnableGndFlag: m_enabledGndFlag = variant.toBool(); return;
            case IndexSendGndFlagToSimulator: m_sendGndToSim = variant.toBool(); return;
            default: break;
            }
            BLACK_VERIFY_X(false, Q_FUNC_INFO, "Cannot handle index");
        }

        QString CInterpolationAndRenderingSetupBase::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return
                QStringLiteral("Dbg.sim.msgs: ") % boolToYesNo(m_simulatorDebugMessages) %
                QStringLiteral(" | log interpolation: ") % boolToYesNo(m_logInterpolation) %
                QStringLiteral(" | force full interpolation: ") % boolToYesNo(m_forceFullInterpolation) %
                QStringLiteral(" | enable parts: ") % boolToYesNo(m_enabledAircraftParts) %
                QStringLiteral(" | enable gnd: ") % boolToYesNo(m_enabledGndFlag) %
                QStringLiteral(" | send gnd: ") % boolToYesNo(m_sendGndToSim);
        }

        bool CInterpolationAndRenderingSetupBase::canHandleIndex(int index)
        {
            return index >= CInterpolationAndRenderingSetupBase::IndexLogInterpolation && index <= CInterpolationAndRenderingSetupBase::IndexEnabledAircraftParts;
        }

        CInterpolationAndRenderingSetupGlobal::CInterpolationAndRenderingSetupGlobal()
        { }

        int CInterpolationAndRenderingSetupGlobal::InfiniteAircraft()
        {
            return 100;
        }

        bool CInterpolationAndRenderingSetupGlobal::isRenderingEnabled() const
        {
            if (m_maxRenderedAircraft < 1)  { return false; }
            if (!isMaxDistanceRestricted()) { return true; }
            return m_maxRenderedDistance.isPositiveWithEpsilonConsidered();
        }

        bool CInterpolationAndRenderingSetupGlobal::isRenderingRestricted() const
        {
            return isRenderingEnabled() && (isMaxAircraftRestricted() || isMaxDistanceRestricted());
        }

        int CInterpolationAndRenderingSetupGlobal::getMaxRenderedAircraft() const
        {
            return (m_maxRenderedAircraft <= InfiniteAircraft()) ? m_maxRenderedAircraft : InfiniteAircraft();
        }

        bool CInterpolationAndRenderingSetupGlobal::setMaxRenderedAircraft(int maxRenderedAircraft)
        {
            if (maxRenderedAircraft == m_maxRenderedAircraft) { return false; }
            if (maxRenderedAircraft < 1)
            {
                // disable, we set both values to 0
                this->disableRendering();
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

        bool CInterpolationAndRenderingSetupGlobal::setMaxRenderedDistance(const CLength &distance)
        {

            if (distance == m_maxRenderedDistance) { return false; }
            if (distance.isNull() || distance.isNegativeWithEpsilonConsidered())
            {
                m_maxRenderedDistance = CLength(0.0, nullptr);
            }
            else if (distance.isZeroEpsilonConsidered())
            {
                // zero means disabled, we disable max aircraft too
                this->disableRendering();
            }
            else
            {
                Q_ASSERT(!distance.isNegativeWithEpsilonConsidered());
                m_maxRenderedDistance = distance;
            }
            return true;
        }


        void CInterpolationAndRenderingSetupGlobal::clearMaxRenderedDistance()
        {
            this->setMaxRenderedDistance(CLength(0.0, nullptr));
        }

        bool CInterpolationAndRenderingSetupGlobal::isMaxAircraftRestricted() const
        {
            return m_maxRenderedAircraft < InfiniteAircraft();
        }

        void CInterpolationAndRenderingSetupGlobal::clearAllRenderingRestrictions()
        {
            m_maxRenderedDistance = CLength(0, nullptr);
            m_maxRenderedAircraft = InfiniteAircraft();
        }

        void CInterpolationAndRenderingSetupGlobal::disableRendering()
        {
            m_maxRenderedAircraft = 0;
            m_maxRenderedDistance = CLength(0, CLengthUnit::NM()); // zero distance
        }

        bool CInterpolationAndRenderingSetupGlobal::isMaxDistanceRestricted() const
        {
            return !m_maxRenderedDistance.isNull();
        }

        QString CInterpolationAndRenderingSetupGlobal::getRenderRestrictionText() const
        {
            if (!this->isRenderingRestricted()) { return "none"; }
            QString rt;
            if (this->isMaxAircraftRestricted())
            {
                rt.append(QString::number(this->getMaxRenderedAircraft())).append(" A/C");
            }
            if (this->isMaxDistanceRestricted())
            {
                if (!rt.isEmpty()) { rt.append(" ");}
                rt.append(this->getMaxRenderedDistance().valueRoundedWithUnit(CLengthUnit::NM(), 0));
            }
            return rt;
        }

        void CInterpolationAndRenderingSetupGlobal::setBaseValues(const CInterpolationAndRenderingSetupBase &baseValues)
        {
            m_logInterpolation       = baseValues.logInterpolation();
            m_simulatorDebugMessages = baseValues.showSimulatorDebugMessages();
            m_forceFullInterpolation = baseValues.isForcingFullInterpolation();
            m_enabledAircraftParts   = baseValues.isAircraftPartsEnabled();
            m_enabledGndFlag         = baseValues.isGndFlagEnabled();
            m_sendGndToSim           = baseValues.sendGndFlagToSimulator();
        }

        QString CInterpolationAndRenderingSetupGlobal::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return
                CInterpolationAndRenderingSetupBase::convertToQString(i18n) %
                QStringLiteral(" max.aircraft:") % QString::number(m_maxRenderedAircraft) %
                QStringLiteral(" max.distance:") % m_maxRenderedDistance.valueRoundedWithUnit(CLengthUnit::NM(), 2);
        }

        CVariant CInterpolationAndRenderingSetupGlobal::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMaxRenderedAircraft: return CVariant::fromValue(m_maxRenderedAircraft);
            case IndexMaxRenderedDistance: return CVariant::fromValue(m_maxRenderedDistance);
            default: break;
            }
            if (CInterpolationAndRenderingSetupBase::canHandleIndex(i)) { return CInterpolationAndRenderingSetupBase::propertyByIndex(index); }
            return CValueObject::propertyByIndex(index);
        }

        void CInterpolationAndRenderingSetupGlobal::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                *this = variant.value<CInterpolationAndRenderingSetupGlobal>();
                return;
            }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexMaxRenderedAircraft: m_maxRenderedAircraft = variant.toInt(); return;
            case IndexMaxRenderedDistance: m_maxRenderedDistance = variant.value<CLength>(); return;
            default: break;
            }
            if (CInterpolationAndRenderingSetupBase::canHandleIndex(i))
            {
                CInterpolationAndRenderingSetupBase::setPropertyByIndex(index, variant);
                return;
            }
            CValueObject::setPropertyByIndex(index, variant);
        }

        CInterpolationAndRenderingSetupPerCallsign::CInterpolationAndRenderingSetupPerCallsign()
        { }

        CInterpolationAndRenderingSetupPerCallsign::CInterpolationAndRenderingSetupPerCallsign(const CCallsign &callsign, const CInterpolationAndRenderingSetupGlobal &globalSetup)
            : CInterpolationAndRenderingSetupBase(globalSetup), m_callsign(callsign)
        { }

        CPropertyIndexList CInterpolationAndRenderingSetupPerCallsign::unequalToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const
        {
            CPropertyIndexList diff;
            if (this->logInterpolation() != globalSetup.logInterpolation()) { diff.push_back(IndexLogInterpolation); }
            if (this->showSimulatorDebugMessages() != globalSetup.showSimulatorDebugMessages()) { diff.push_back(IndexSimulatorDebugMessages); }
            if (this->isForcingFullInterpolation() != globalSetup.isForcingFullInterpolation()) { diff.push_back(IndexForceFullInterpolation); }
            if (this->isAircraftPartsEnabled() != globalSetup.isAircraftPartsEnabled()) { diff.push_back(IndexEnabledAircraftParts); }
            if (this->isGndFlagEnabled() != globalSetup.isGndFlagEnabled()) { diff.push_back(IndexEnableGndFlag); }
            if (this->sendGndFlagToSimulator() != globalSetup.sendGndFlagToSimulator()) { diff.push_back(IndexSendGndFlagToSimulator); }
            return diff;
        }

        bool CInterpolationAndRenderingSetupPerCallsign::isEqualToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const
        {
            return this->unequalToGlobal(globalSetup).isEmpty();
        }

        CVariant CInterpolationAndRenderingSetupPerCallsign::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
            default: break;
            }
            return CInterpolationAndRenderingSetupBase::propertyByIndex(index);
        }

        void CInterpolationAndRenderingSetupPerCallsign::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself())
            {
                *this = variant.value<CInterpolationAndRenderingSetupPerCallsign>();
                return;
            }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign: m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); return;
            default: break;
            }
            CInterpolationAndRenderingSetupBase::setPropertyByIndex(index, variant);
        }

        const CInterpolationAndRenderingSetupPerCallsign &CInterpolationAndRenderingSetupPerCallsign::null()
        {
            static const CInterpolationAndRenderingSetupPerCallsign null;
            return null;
        }
    } // ns
} // ns

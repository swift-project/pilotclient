/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/network/client.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"
#include "blackconfig/buildconfig.h"
#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;

namespace BlackMisc::Simulation
{
    CInterpolationAndRenderingSetupBase::CInterpolationAndRenderingSetupBase()
    {
        // Experimental
        // m_fixSceneryOffset = CBuildConfig::isLocalDeveloperDebugBuild();
    }

    bool CInterpolationAndRenderingSetupBase::setSendingGndFlagToSimulator(bool sendFLag)
    {
        if (sendFLag == m_sendGndToSim) { return false; }
        m_sendGndToSim = sendFLag;
        return true;
    }

    bool CInterpolationAndRenderingSetupBase::setPitchOnGround(const CAngle &pitchOnGround)
    {
        if (pitchOnGround == m_pitchOnGround) { return false; }
        m_pitchOnGround = pitchOnGround;
        return true;
    }

    void CInterpolationAndRenderingSetupBase::consolidateWithClient(const CClient &client)
    {
        m_enabledAircraftParts &= client.hasAircraftPartsCapability();
    }

    bool CInterpolationAndRenderingSetupBase::setInterpolatorMode(CInterpolationAndRenderingSetupBase::InterpolatorMode mode)
    {
        const int m = static_cast<int>(mode);
        if (m_interpolatorMode == m) { return false; }
        m_interpolatorMode = m;
        return true;
    }

    bool CInterpolationAndRenderingSetupBase::setInterpolatorMode(const QString &mode)
    {
        if (mode.contains("spline", Qt::CaseInsensitive)) { return this->setInterpolatorMode(Spline); }
        if (mode.contains("linear", Qt::CaseInsensitive)) { return this->setInterpolatorMode(Linear); }
        return false;
    }

    const QString &CInterpolationAndRenderingSetupBase::modeToString(InterpolatorMode mode)
    {
        static const QString l("linear");
        static const QString s("spline");

        switch (mode)
        {
        case Linear: return l;
        case Spline: return s;
        default: return s;
        }
    }

    bool CInterpolationAndRenderingSetupBase::setLogInterpolation(bool log)
    {
        if (m_logInterpolation == log) { return false; }
        m_logInterpolation = log;
        return true;
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

    QVariant CInterpolationAndRenderingSetupBase::propertyByIndex(CPropertyIndexRef index) const
    {
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLogInterpolation:       return QVariant::fromValue(m_logInterpolation);
        case IndexSimulatorDebugMessages: return QVariant::fromValue(m_simulatorDebugMessages);
        case IndexForceFullInterpolation: return QVariant::fromValue(m_forceFullInterpolation);
        case IndexEnabledAircraftParts:   return QVariant::fromValue(m_enabledAircraftParts);
        case IndexSendGndFlagToSimulator: return QVariant::fromValue(m_sendGndToSim);
        case IndexInterpolatorMode:       return QVariant::fromValue(m_interpolatorMode);
        case IndexInterpolatorModeAsString: return QVariant::fromValue(this->getInterpolatorModeAsString());
        case IndexFixSceneryOffset: return QVariant::fromValue(m_fixSceneryOffset);
        case IndexPitchOnGround:    return QVariant::fromValue(m_pitchOnGround);
        default: break;
        }
        BLACK_VERIFY_X(false, Q_FUNC_INFO, "Cannot handle index");
        return QStringLiteral("Wrong index for %1").arg(i);
    }

    void CInterpolationAndRenderingSetupBase::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexLogInterpolation:       m_logInterpolation = variant.toBool(); return;
        case IndexSimulatorDebugMessages: m_simulatorDebugMessages = variant.toBool(); return;
        case IndexForceFullInterpolation: m_forceFullInterpolation = variant.toBool(); return;
        case IndexEnabledAircraftParts:   m_enabledAircraftParts = variant.toBool(); return;
        case IndexSendGndFlagToSimulator: m_sendGndToSim = variant.toBool(); return;
        case IndexInterpolatorMode:       m_interpolatorMode = variant.toInt(); return;
        case IndexInterpolatorModeAsString: this->setInterpolatorMode(variant.toString()); return;
        case IndexFixSceneryOffset: m_fixSceneryOffset = variant.toBool(); return;
        case IndexPitchOnGround:    m_pitchOnGround.setPropertyByIndex(index.copyFrontRemoved(), variant); return;
        default: break;
        }
        BLACK_VERIFY_X(false, Q_FUNC_INFO, "Cannot handle index");
    }

    QString CInterpolationAndRenderingSetupBase::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return
            QStringLiteral("Interpolator: ") % this->getInterpolatorModeAsString() %
            QStringLiteral(" | Dbg.sim.msgs: ") % boolToYesNo(m_simulatorDebugMessages) %
            QStringLiteral(" | log interpolation: ") % boolToYesNo(m_logInterpolation) %
            QStringLiteral(" | force VTOL interpolation: ") % boolToYesNo(m_forceFullInterpolation) %
            QStringLiteral(" | enable parts: ") % boolToYesNo(m_enabledAircraftParts) %
            QStringLiteral(" | send gnd: ") % boolToYesNo(m_sendGndToSim) %
            QStringLiteral(" | fix.scenery offset: ") % boolToYesNo(m_fixSceneryOffset) %
            QStringLiteral(" | pitch on gnd.: ") % m_pitchOnGround.valueRoundedWithUnit(CAngleUnit::deg(), 1, true);
    }

    bool CInterpolationAndRenderingSetupBase::canHandleIndex(int index)
    {
        return index >= CInterpolationAndRenderingSetupBase::IndexLogInterpolation && index <= CInterpolationAndRenderingSetupBase::IndexFixSceneryOffset;
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
        m_sendGndToSim           = baseValues.isSendingGndFlagToSimulator();
        m_fixSceneryOffset       = baseValues.isFixingSceneryOffset();
        m_interpolatorMode       = baseValues.getInterpolatorMode();
        m_pitchOnGround          = baseValues.getPitchOnGround();
    }

    QString CInterpolationAndRenderingSetupGlobal::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)
        return
            CInterpolationAndRenderingSetupBase::convertToQString(i18n) %
            QStringLiteral(" max.aircraft:") % QString::number(m_maxRenderedAircraft) %
            QStringLiteral(" max.distance:") % m_maxRenderedDistance.valueRoundedWithUnit(CLengthUnit::NM(), 2);
    }

    QVariant CInterpolationAndRenderingSetupGlobal::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexMaxRenderedAircraft: return QVariant::fromValue(m_maxRenderedAircraft);
        case IndexMaxRenderedDistance: return QVariant::fromValue(m_maxRenderedDistance);
        default: break;
        }
        if (CInterpolationAndRenderingSetupBase::canHandleIndex(i)) { return CInterpolationAndRenderingSetupBase::propertyByIndex(index); }
        return CValueObject::propertyByIndex(index);
    }

    void CInterpolationAndRenderingSetupGlobal::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
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
        if (this->logInterpolation()            != globalSetup.logInterpolation())            { diff.push_back(IndexLogInterpolation); }
        if (this->showSimulatorDebugMessages()  != globalSetup.showSimulatorDebugMessages())  { diff.push_back(IndexSimulatorDebugMessages); }
        if (this->isForcingFullInterpolation()  != globalSetup.isForcingFullInterpolation())  { diff.push_back(IndexForceFullInterpolation); }
        if (this->isAircraftPartsEnabled()      != globalSetup.isAircraftPartsEnabled())      { diff.push_back(IndexEnabledAircraftParts); }
        if (this->isSendingGndFlagToSimulator() != globalSetup.isSendingGndFlagToSimulator()) { diff.push_back(IndexSendGndFlagToSimulator); }
        if (this->isFixingSceneryOffset()       != globalSetup.isFixingSceneryOffset())       { diff.push_back(IndexFixSceneryOffset); }
        if (this->getPitchOnGround()            != globalSetup.getPitchOnGround())            { diff.push_back(IndexPitchOnGround); }
        return diff;
    }

    bool CInterpolationAndRenderingSetupPerCallsign::isEqualToGlobal(const CInterpolationAndRenderingSetupGlobal &globalSetup) const
    {
        const CPropertyIndexList il = this->unequalToGlobal(globalSetup);
        const bool equal = il.isEmpty();
        return equal;
    }

    QVariant CInterpolationAndRenderingSetupPerCallsign::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
        default: break;
        }
        return CInterpolationAndRenderingSetupBase::propertyByIndex(index);
    }

    void CInterpolationAndRenderingSetupPerCallsign::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
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

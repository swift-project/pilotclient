/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator_common.h"
#include "interpolator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/collection.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    CSimulatorCommon::CSimulatorCommon(const CSimulatorPluginInfo &info,
                                       BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                       QObject *parent)
        : ISimulator(parent),
          COwnAircraftAware(ownAircraftProvider),
          CRemoteAircraftAware(remoteAircraftProvider),
          m_simulatorPluginInfo(info)
    {
        this->setObjectName(info.getIdentifier());
        this->m_oneSecondTimer = new QTimer(this);
        this->m_oneSecondTimer->setObjectName(this->objectName().append(":m_oneSecondTimer"));
        connect(this->m_oneSecondTimer, &QTimer::timeout, this, &CSimulatorCommon::ps_oneSecondTimer);
        this->m_oneSecondTimer->start(1000);

        // provider signals
        bool c = remoteAircraftProvider->connectRemoteAircraftProviderSignals(
                     std::bind(&CSimulatorCommon::ps_remoteProviderAddAircraftSituation, this, std::placeholders::_1),
                     std::bind(&CSimulatorCommon::ps_remoteProviderAddAircraftParts, this, std::placeholders::_1),
                     std::bind(&CSimulatorCommon::ps_remoteProviderRemovedAircraft, this, std::placeholders::_1),
                     std::bind(&CSimulatorCommon::ps_remoteProviderAircraftSnapshot, this, std::placeholders::_1)
                 );
        Q_ASSERT(c);
        Q_UNUSED(c);

        // info
        CLogMessage(this).info("Initialized simulator driver %1") << m_simulatorPluginInfo.toQString();
    }

    bool CSimulatorCommon::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        if (!remoteAircraft.isEnabled()) { return false; }

        // if not restriced, directly change
        if (!isRenderingRestricted()) { return this->physicallyAddRemoteAircraft(remoteAircraft); }

        //! \todo Go thru logic
        return this->physicallyAddRemoteAircraft(remoteAircraft);
    }

    bool CSimulatorCommon::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        // if not restriced, directly change
        if (!isRenderingRestricted()) { return this->physicallyRemoveRemoteAircraft(callsign); }

        //! \todo Go thru logic
        return this->physicallyRemoveRemoteAircraft(callsign);
    }

    void CSimulatorCommon::blinkHighlightedAircraft()
    {
        if (m_highlightedAircraft.isEmpty() || m_highlightEndTimeMsEpoch < 1) { return; }
        m_blinkCycle = !m_blinkCycle;

        if (QDateTime::currentMSecsSinceEpoch() < m_highlightEndTimeMsEpoch)
        {
            // blink mode, toggle aircraft
            for (const CSimulatedAircraft &aircraft : m_highlightedAircraft)
            {
                if (m_blinkCycle)
                {
                    this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
                }
                else
                {
                    this->physicallyAddRemoteAircraft(aircraft);
                }
            }
        }
        else
        {
            // restore
            for (const CSimulatedAircraft &aircraft : m_highlightedAircraft)
            {
                // get the current state for this aircraft
                // it might has been removed in the mean time
                const CCallsign cs(aircraft.getCallsign());
                resetAircraftFromBacked(cs);
            }
            m_highlightedAircraft.clear();
            m_highlightEndTimeMsEpoch = 0;
        }
    }

    void CSimulatorCommon::recalculateRestrictedAircraft()
    {
        if (!isMaxAircraftRestricted()) { return; }
        if (!isRenderingEnabled()) { return; }

        CSimulatedAircraftList newAircraftInRange(getAircraftInRange().getClosestObjects(getMaxRenderedAircraft()));
        CCallsignSet newAircraftCallsigns(newAircraftInRange.getCallsigns());
        CCallsignSet toBeRemovedCallsigns(m_callsignsToBeRendered.difference(newAircraftCallsigns));
        CCallsignSet toBeAddedCallsigns(newAircraftCallsigns.difference(m_callsignsToBeRendered));
        for (const CCallsign &cs : toBeRemovedCallsigns)
        {
            physicallyRemoveRemoteAircraft(cs);
        }
        for (const CCallsign &cs : toBeAddedCallsigns)
        {
            physicallyAddRemoteAircraft(newAircraftInRange.findFirstByCallsign(cs));
        }
        this->m_callsignsToBeRendered = newAircraftCallsigns;
    }

    void CSimulatorCommon::resetAircraftFromBacked(const CCallsign &callsign)
    {
        CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
        bool enabled = aircraft.isEnabled();
        if (enabled)
        {
            // are we already visible?
            if (!isPhysicallyRenderedAircraft(callsign))
            {
                this->physicallyAddRemoteAircraft(aircraft);
            }
        }
        else
        {
            physicallyRemoveRemoteAircraft(callsign);
        }
    }

    void CSimulatorCommon::setInitialAircraftSituationAndParts(CSimulatedAircraft &aircraft) const
    {
        if (!this->m_interpolator) { return; }

        const CCallsign callsign(aircraft.getCallsign());
        if (!(this->remoteAircraftSituationsCount(callsign) < 1)) { return; }

        // with an interpolator the interpolated situation is used
        // to avoid position jittering
        qint64 time = QDateTime::currentMSecsSinceEpoch();
        IInterpolator::InterpolationStatus is;
        CAircraftSituation as(m_interpolator->getInterpolatedSituation(callsign, time, aircraft.isVtol(), is));
        if (is.interpolationSucceeded) { aircraft.setSituation(as); }

    }

    int CSimulatorCommon::getMaxRenderedAircraft() const
    {
        return (m_maxRenderedAircraft <= MaxAircraftInfinite) ? m_maxRenderedAircraft : MaxAircraftInfinite;
    }

    void CSimulatorCommon::setMaxRenderedAircraft(int maxRenderedAircraft)
    {
        if (maxRenderedAircraft == m_maxRenderedAircraft) { return; }
        if (maxRenderedAircraft < 1)
        {
            m_maxRenderedAircraft = 0;
        }
        else if (maxRenderedAircraft >= MaxAircraftInfinite)
        {
            m_maxRenderedAircraft = MaxAircraftInfinite;
        }
        else
        {
            m_maxRenderedAircraft = maxRenderedAircraft;
        }

        bool r = isRenderingRestricted();
        emit restrictedRenderingChanged(r);
    }

    void CSimulatorCommon::setMaxRenderedDistance(CLength &distance)
    {
        if (distance == m_maxRenderedDistance) { return; }
        if (distance.isNull() || distance >= getRenderedDistanceBoundary())
        {
            m_maxRenderedDistance = CLength(0.0, CLengthUnit::nullUnit());
        }
        else
        {
            Q_ASSERT(distance.isPositiveWithEpsilonConsidered());
            m_maxRenderedDistance = distance;
        }

        bool r = isRenderingRestricted();
        emit restrictedRenderingChanged(r);
    }

    CLength CSimulatorCommon::getMaxRenderedDistance() const
    {
        if (m_maxRenderedDistance.isNull()) { return getRenderedDistanceBoundary(); }
        return m_maxRenderedDistance;
    }

    const CSimulatorPluginInfo &CSimulatorCommon::getSimulatorPluginInfo() const
    {
        return m_simulatorPluginInfo;
    }

    const CSimulatorSetup &CSimulatorCommon::getSimulatorSetup() const
    {
        return m_simulatorSetup;
    }

    CLength CSimulatorCommon::getRenderedDistanceBoundary() const
    {
        return CLength(20.0, CLengthUnit::NM());
    }

    bool CSimulatorCommon::isMaxAircraftRestricted() const
    {
        return m_maxRenderedAircraft < MaxAircraftInfinite && isRenderingEnabled();
    }

    bool CSimulatorCommon::isMaxDistanceRestricted() const
    {
        return !m_maxRenderedDistance.isNull();
    }

    void CSimulatorCommon::enableDebugMessages(bool driverMessages, bool interpolatorMessages)
    {
        this->m_debugMessages = driverMessages;
        Q_UNUSED(interpolatorMessages);
    }

    int CSimulatorCommon::getInstalledModelsCount() const
    {
        return getInstalledModels().size();
    }

    void CSimulatorCommon::highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime)
    {
        CCallsign cs(aircraftToHighlight.getCallsign());
        this->m_highlightedAircraft.removeByCallsign(cs);
        if (enableHighlight)
        {
            qint64 deltaT = displayTime.valueRounded(CTimeUnit::ms(), 0);
            this->m_highlightEndTimeMsEpoch = QDateTime::currentMSecsSinceEpoch() + deltaT;
            this->m_highlightedAircraft.push_back(aircraftToHighlight);
        }
    }

    bool CSimulatorCommon::isRenderingEnabled() const
    {
        if (m_maxRenderedAircraft < 1)  { return false; }
        if (!isMaxDistanceRestricted()) { return true; }

        return m_maxRenderedDistance.valueRounded(CLengthUnit::NM(), 2) > 0.1;
    }

    bool CSimulatorCommon::isRenderingRestricted() const
    {
        return this->isMaxDistanceRestricted() || this->isMaxAircraftRestricted();
    }

    void CSimulatorCommon::deleteAllRenderingRestrictions()
    {
        if (!isRenderingEnabled()) { return; }
        this->m_maxRenderedDistance = CLength(0, CLengthUnit::nullUnit());
        this->m_maxRenderedAircraft = MaxAircraftInfinite;
        emit restrictedRenderingChanged(false);
    }

    void CSimulatorCommon::ps_oneSecondTimer()
    {
        m_timerCounter++;
        blinkHighlightedAircraft();

        // any <n> seconds
        if (m_timerCounter % 10 == 0)
        {
            recalculateRestrictedAircraft();
        }
    }

    void CSimulatorCommon::ps_recalculateRenderedAircraft()
    {
        this->ps_recalculateRenderedAircraft(getLatestAirspaceAircraftSnapshot());
    }

    void CSimulatorCommon::ps_recalculateRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!snapshot.isValidSnapshot()) { return;}
        CCallsignSet callsignsInSimuator(physicallyRenderedAircraft());

    }

    void CSimulatorCommon::ps_remoteProviderAddAircraftSituation(const CAircraftSituation &situation)
    {
        Q_UNUSED(situation);
    }

    void CSimulatorCommon::ps_remoteProviderAddAircraftParts(const CAircraftParts &parts)
    {
        Q_UNUSED(parts);
    }

    void CSimulatorCommon::ps_remoteProviderRemovedAircraft(const CCallsign &callsign)
    {
        Q_UNUSED(callsign);
    }

    void CSimulatorCommon::ps_remoteProviderAircraftSnapshot(const CAirspaceAircraftSnapshot &aircraftSnapshot)
    {
        ps_recalculateRenderedAircraft(aircraftSnapshot);
    }

} // namespace

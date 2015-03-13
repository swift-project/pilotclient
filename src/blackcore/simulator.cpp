/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator.h"
#include "interpolator.h"
#include "blackmisc/collection.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackSim;

namespace BlackCore
{
    void ISimulator::emitSimulatorCombinedStatus()
    {
        int status =
            (isConnected() ? Connected : static_cast<ISimulator::SimulatorStatus>(0))
          | (isSimulating() ? Running : static_cast<ISimulator::SimulatorStatus>(0))
          | (isPaused() ? Paused : static_cast<ISimulator::SimulatorStatus>(0))
          ;
        emit simulatorStatusChanged(status);
    }
    
    ISimulatorListener::ISimulatorListener(QObject* parent) : QObject(parent)
    {
    }

    CSimulatorCommon::CSimulatorCommon(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                       QObject *parent)
        : ISimulator(parent),
          COwnAircraftProviderSupport(ownAircraftProvider),
          CRemoteAircraftProviderSupport(remoteAircraftProvider)
    {
        m_oneSecondTimer = new QTimer(this);
        connect(this->m_oneSecondTimer, &QTimer::timeout, this, &CSimulatorCommon::ps_oneSecondTimer);
        m_oneSecondTimer->start(1000);
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
                    this->removeRemoteAircraft(aircraft.getCallsign());
                }
                else
                {
                    this->addRemoteAircraft(aircraft);
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

        //! \todo Simulator, why is there no difference on CSequence?
        CSimulatedAircraftList newAircraftInRange(remoteAircraft().getClosestObjects(getMaxRenderedAircraft()));
        CCallsignList newAircraftCallsigns(newAircraftInRange.getCallsigns());
        CCallsignList toBeRemovedCallsigns(m_callsignsToBeRendered.difference(newAircraftCallsigns));
        CCallsignList toBeAddedCallsigns(newAircraftCallsigns.difference(m_callsignsToBeRendered));
        for (const CCallsign &cs : toBeRemovedCallsigns)
        {
            removeRemoteAircraft(cs);
        }
        for (const CCallsign &cs : toBeAddedCallsigns)
        {
            addRemoteAircraft(newAircraftInRange.findFirstByCallsign(cs));
        }
        this->m_callsignsToBeRendered = newAircraftCallsigns;
    }

    void CSimulatorCommon::resetAircraftFromBacked(const CCallsign &callsign)
    {
        CSimulatedAircraft aircraft(this->remoteAircraft().findFirstByCallsign(callsign));
        bool enabled = aircraft.isEnabled();
        if (enabled)
        {
            // are we already visible?
            if (!isRenderedAircraft(callsign))
            {
                this->addRemoteAircraft(aircraft);
            }
        }
        else
        {
            removeRemoteAircraft(callsign);
        }
    }

    void CSimulatorCommon::setInitialAircraftSituationAndParts(CSimulatedAircraft &aircraft) const
    {
        if (!this->m_interpolator) { return; }

        const CCallsign callsign(aircraft.getCallsign());
        if (!this->m_interpolator->hasDataForCallsign(callsign)) { return; }

        // with an interpolator the interpolated situation is used
        // to avoid position jittering
        qint64 time = QDateTime::currentMSecsSinceEpoch();
        IInterpolator::InterpolationStatus is;
        CAircraftSituation as(m_interpolator->getInterpolatedSituation(callsign, time, is));
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
    
    void CSimulatorCommon::enableDebugMessages(bool driver, bool interpolator)
    {
        this->m_debugMessages = driver;
        Q_UNUSED(interpolator);
    
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

} // namespace

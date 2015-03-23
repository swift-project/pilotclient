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

using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackSim;

namespace BlackCore
{
    void ISimulator::emitSimulatorCombinedStatus()
    {
        emit simulatorStatusChanged(isConnected(), isSimulating(), isPaused());
    }

    CSimulatorCommon::CSimulatorCommon(const BlackSim::CSimulatorInfo &simInfo, BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider, BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider, QObject *parent)
        : ISimulator(parent), COwnAircraftProviderSupport(ownAircraftProvider), CRemoteAircraftProviderSupport(remoteAircraftProvider), m_simulatorInfo(simInfo)
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
        return m_maxRenderedAircraft;
    }

    void CSimulatorCommon::setMaxRenderedAircraft(int maxRenderedAircraft, const BlackMisc::Aviation::CCallsignList &callsigns)
    {
        m_maxRenderedAircraft = maxRenderedAircraft;
        m_callsignsToBeRendered = callsigns;
    }

    CSimulatorInfo CSimulatorCommon::getSimulatorInfo() const
    {
        return m_simulatorInfo;
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
        return m_maxRenderedAircraft < 1;
    }

    void CSimulatorCommon::ps_oneSecondTimer()
    {
        blinkHighlightedAircraft();
    }

} // namespace

/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airspace_analyzer.h"
#include "blackmisc/logmessage.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{

    CAirspaceAnalyzer::CAirspaceAnalyzer(const IOwnAircraftProviderReadOnly *ownAircraftProvider, IRemoteAircraftProvider *remoteAircraftProvider, INetwork *network, QObject *parent) :
        CContinuousWorker(parent, "CAirspaceAnalyzer"),
        COwnAircraftAwareReadOnly(ownAircraftProvider),
        CRemoteAircraftAware(remoteAircraftProvider)
    {
        Q_ASSERT_X(network, Q_FUNC_INFO, "Network object required to connect");

        // disconnect
        bool c = connect(network, &INetwork::pilotDisconnected, this, &CAirspaceAnalyzer::ps_watchdogRemoveAircraftCallsign);
        Q_ASSERT(c);
        c = connect(network, &INetwork::atcDisconnected, this, &CAirspaceAnalyzer::ps_watchdogRemoveAtcCallsign);
        Q_ASSERT(c);

        // update
        c = connect(network, &INetwork::aircraftPositionUpdate, this, &CAirspaceAnalyzer::ps_watchdogTouchAircraftCallsign);
        Q_ASSERT(c);
        c = connect(network, &INetwork::atcPositionUpdate, this, &CAirspaceAnalyzer::ps_watchdogTouchAtcCallsign);
        Q_ASSERT(c);

        // network
        c = connect(network, &INetwork::connectionStatusChanged, this, &CAirspaceAnalyzer::ps_onConnectionStatusChanged);
        Q_ASSERT(c);
        Q_UNUSED(c);
    }

    void CAirspaceAnalyzer::ps_watchdogTouchAircraftCallsign(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_ASSERT_X(!situation.getCallsign().isEmpty(), Q_FUNC_INFO, "No callsign in situaton");
        Q_UNUSED(transponder);
        m_aircraftCallsignTimestamps[situation.getCallsign()] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::ps_watchdogTouchAtcCallsign(const CCallsign &callsign, const CFrequency &freq, const Geo::CCoordinateGeodetic &pos, const CLength &range)
    {
        Q_UNUSED(freq);
        Q_UNUSED(pos);
        Q_UNUSED(range);
        m_atcCallsignTimestamps[callsign] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::ps_onConnectionStatusChanged(INetwork::ConnectionStatus oldStatus, INetwork::ConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus);
        if (newStatus == INetwork::Disconnected)
        {
            this->clear();
        }
    }

    void CAirspaceAnalyzer::clear()
    {
        m_aircraftCallsignTimestamps.clear();
        m_atcCallsignTimestamps.clear();
    }

    void CAirspaceAnalyzer::ps_watchdogRemoveAircraftCallsign(const CCallsign &callsign)
    {
        m_aircraftCallsignTimestamps.remove(callsign);
    }

    void CAirspaceAnalyzer::ps_watchdogRemoveAtcCallsign(const CCallsign &callsign)
    {
        m_atcCallsignTimestamps.remove(callsign);
    }

    void CAirspaceAnalyzer::watchdogCheckTimeouts()
    {
        qint64 aircraftTimeoutMs = m_timeoutAircraft.valueInteger(CTimeUnit::ms());
        qint64 atcTimeoutMs = m_timeoutAtc.valueInteger(CTimeUnit::ms());
        qint64 currentTimeMsEpoch = QDateTime::currentMSecsSinceEpoch();
        qint64 timeoutAircraftEpochMs = currentTimeMsEpoch - aircraftTimeoutMs;
        qint64 timeoutAtcEpochMs = currentTimeMsEpoch - atcTimeoutMs;

        for (const CCallsign &callsign : m_aircraftCallsignTimestamps.keys())
        {
            if (m_aircraftCallsignTimestamps.value(callsign) > timeoutAircraftEpochMs) { continue; }
            CLogMessage(this).debug() << "Aircraft " << callsign.toQString() << "timed out!";
            m_aircraftCallsignTimestamps.remove(callsign);
            emit timeoutAircraft(callsign);
        }

        for (const CCallsign &callsign : m_atcCallsignTimestamps.keys())
        {
            if (m_atcCallsignTimestamps.value(callsign) > timeoutAtcEpochMs) { continue; }
            CLogMessage(this).debug() << "ATC " << callsign.toQString() << "timed out!";
            m_atcCallsignTimestamps.remove(callsign);
            emit timeoutAtc(callsign);
        }
    }
} // ns

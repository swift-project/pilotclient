/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/airspaceanalyzer.h"
#include "airspacemonitor.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/threadutils.h"

#include <QDateTime>
#include <QMetaObject>
#include <QReadLocker>
#include <QString>
#include <QThread>
#include <QWriteLocker>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackCore
{
    CAirspaceAnalyzer::CAirspaceAnalyzer(IOwnAircraftProvider *ownAircraftProvider, INetwork *network, CAirspaceMonitor *airspaceMonitorParent) :
        CContinuousWorker(airspaceMonitorParent, "CAirspaceAnalyzer"),
        COwnAircraftAware(ownAircraftProvider),
        CRemoteAircraftAware(airspaceMonitorParent)
    {
        Q_ASSERT_X(network, Q_FUNC_INFO, "Network object required to connect");

        // all in new thread from here on
        this->setObjectName(getName());
        m_updateTimer.start(7500);
        m_lastWatchdogCallMsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
        bool c = connect(&m_updateTimer, &QTimer::timeout, this, &CAirspaceAnalyzer::onTimeout);
        Q_ASSERT(c);

        // network connected
        c = connect(network, &INetwork::pilotDisconnected, this, &CAirspaceAnalyzer::watchdogRemoveAircraftCallsign);
        Q_ASSERT(c);
        c = connect(network, &INetwork::atcDisconnected, this, &CAirspaceAnalyzer::watchdogRemoveAtcCallsign);
        Q_ASSERT(c);
        c = connect(network, &INetwork::connectionStatusChanged, this, &CAirspaceAnalyzer::onConnectionStatusChanged);
        Q_ASSERT(c);

        // network situations
        c = connect(network, &INetwork::aircraftPositionUpdate, this, &CAirspaceAnalyzer::onNetworkPositionUpdate);
        Q_ASSERT(c);
        c = connect(network, &INetwork::atcPositionUpdate, this, &CAirspaceAnalyzer::watchdogTouchAtcCallsign);
        Q_ASSERT(c);

        // Monitor
        c = connect(airspaceMonitorParent, &CAirspaceMonitor::addedAircraftSituation, this, &CAirspaceAnalyzer::watchdogTouchAircraftCallsign);
        Q_ASSERT(c);
        Q_UNUSED(c);

        // start in own thread
        this->start(QThread::LowestPriority);
    }

    CAirspaceAircraftSnapshot CAirspaceAnalyzer::getLatestAirspaceAircraftSnapshot() const
    {
        QReadLocker l(&m_lockSnapshot);
        return m_latestAircraftSnapshot;
    }

    void CAirspaceAnalyzer::setSimulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const CLength &maxRenderedDistance)
    {
        QWriteLocker l(&m_lockRestrictions);
        m_simulatorRenderedAircraftRestricted = restricted;
        m_simulatorRenderingEnabled = enabled;
        m_simulatorMaxRenderedAircraft = maxAircraft;
        m_simulatorMaxRenderedDistance = maxRenderedDistance;
    }

    CAirspaceAnalyzer::~CAirspaceAnalyzer()
    { }

    void CAirspaceAnalyzer::onNetworkPositionUpdate(const CAircraftSituation &situation, const CTransponder &transponder)
    {
        Q_UNUSED(transponder);
        this->watchdogTouchAircraftCallsign(situation);
    }

    void CAirspaceAnalyzer::watchdogTouchAircraftCallsign(const CAircraftSituation &situation)
    {
        Q_ASSERT_X(!situation.getCallsign().isEmpty(), Q_FUNC_INFO, "No callsign in situaton");
        m_aircraftCallsignTimestamps[situation.getCallsign()] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::watchdogTouchAtcCallsign(const CCallsign &callsign, const CFrequency &frequency, const Geo::CCoordinateGeodetic &position, const CLength &range)
    {
        Q_UNUSED(frequency);
        Q_UNUSED(position);
        Q_UNUSED(range);
        m_atcCallsignTimestamps[callsign] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus);
        if (newStatus == INetwork::Disconnected)
        {
            this->clear();
            m_updateTimer.stop();
        }
        else if (newStatus == INetwork::Connected)
        {
            m_updateTimer.start();
        }
    }

    void CAirspaceAnalyzer::onTimeout()
    {
        if (!this->isEnabled()) { return; }
        this->analyzeAirspace();
        this->watchdogCheckTimeouts();
    }

    void CAirspaceAnalyzer::clear()
    {
        m_aircraftCallsignTimestamps.clear();
        m_atcCallsignTimestamps.clear();

        QWriteLocker l(&m_lockSnapshot);
        m_latestAircraftSnapshot = CAirspaceAircraftSnapshot();
    }

    void CAirspaceAnalyzer::watchdogRemoveAircraftCallsign(const CCallsign &callsign)
    {
        m_aircraftCallsignTimestamps.remove(callsign);
    }

    void CAirspaceAnalyzer::watchdogRemoveAtcCallsign(const CCallsign &callsign)
    {
        m_atcCallsignTimestamps.remove(callsign);
    }

    void CAirspaceAnalyzer::watchdogCheckTimeouts()
    {
        // this is a trick to not remove everything while debugging
        const qint64 currentTimeMsEpoch = QDateTime::currentMSecsSinceEpoch();
        const qint64 callDiffMs = currentTimeMsEpoch - m_lastWatchdogCallMsSinceEpoch;
        const qint64 callThresholdMs = static_cast<qint64>(m_updateTimer.interval() * 1.5);
        m_lastWatchdogCallMsSinceEpoch = currentTimeMsEpoch;
        if (callDiffMs > callThresholdMs)
        {
            // allow some time to normalize before checking again
            m_doNotRunAgainBefore = currentTimeMsEpoch + 2 * callThresholdMs;
            return;
        }
        if (m_doNotRunAgainBefore > currentTimeMsEpoch) { return; }
        m_doNotRunAgainBefore = -1;

        // checks
        const qint64 aircraftTimeoutMs = m_timeoutAircraft.valueInteger(CTimeUnit::ms());
        const qint64 atcTimeoutMs = m_timeoutAtc.valueInteger(CTimeUnit::ms());
        const qint64 timeoutAircraftEpochMs = currentTimeMsEpoch - aircraftTimeoutMs;
        const qint64 timeoutAtcEpochMs = currentTimeMsEpoch - atcTimeoutMs;
        const bool enabled = m_enabledWatchdog;

        const QList<CCallsign> callsignsAircraft = m_aircraftCallsignTimestamps.keys();
        for (const CCallsign &callsign : callsignsAircraft) // clazy:exclude=container-anti-pattern,range-loop
        {
            if (!enabled) { m_aircraftCallsignTimestamps[callsign] = timeoutAircraftEpochMs + 1000; } // fake value so it can be re-enabled
            const qint64 tsv = m_aircraftCallsignTimestamps.value(callsign);
            if (tsv > timeoutAircraftEpochMs) { continue; }
            CLogMessage(this).debug() << "Aircraft " << callsign.toQString() << "timed out! " << (currentTimeMsEpoch - tsv) << "ms";
            m_aircraftCallsignTimestamps.remove(callsign);
            emit this->timeoutAircraft(callsign);
        }

        const QList<CCallsign> callsignsAtc = m_atcCallsignTimestamps.keys();
        for (const CCallsign &callsign : callsignsAtc) // clazy:exclude=container-anti-pattern,range-loop
        {
            if (!enabled) { m_aircraftCallsignTimestamps[callsign] = timeoutAtcEpochMs + 1000; } // fake value so it can be re-enabled
            const qint64 tsv = m_aircraftCallsignTimestamps.value(callsign);
            if (m_atcCallsignTimestamps.value(callsign) > timeoutAtcEpochMs) { continue; }
            CLogMessage(this).debug() << "ATC " << callsign.toQString() << "timed out! " << (currentTimeMsEpoch - tsv) << "ms";
            m_atcCallsignTimestamps.remove(callsign);
            emit this->timeoutAtc(callsign);
        }
    }

    void CAirspaceAnalyzer::analyzeAirspace()
    {
        Q_ASSERT_X(!CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Expect to run in background thread");
        Q_ASSERT_X(!CThreadUtils::isApplicationThreadObjectThread(this), Q_FUNC_INFO, "Expect to run in background thread affinity");

        bool restricted, enabled;
        int maxAircraft;
        CLength maxRenderedDistance;
        {
            QReadLocker l(&m_lockRestrictions);
            restricted = m_simulatorRenderedAircraftRestricted;
            enabled = m_simulatorRenderingEnabled;
            maxAircraft = m_simulatorMaxRenderedAircraft;
            maxRenderedDistance = m_simulatorMaxRenderedDistance;
        }

        // remark for simulation snapshot is used when there are restrictions
        // nevertheless we calculate all the time as the snapshot could be used in other scenarios

        CSimulatedAircraftList aircraftInRange(this->getAircraftInRange()); // thread safe copy from provider
        CAirspaceAircraftSnapshot snapshot(
            aircraftInRange,
            restricted, enabled,
            maxAircraft, maxRenderedDistance
        );

        // lock block
        {
            QWriteLocker l(&m_lockSnapshot);
            bool wasValid = m_latestAircraftSnapshot.isValidSnapshot();
            if (wasValid)
            {
                snapshot.setRestrictionChanged(m_latestAircraftSnapshot);
            }
            m_latestAircraftSnapshot = snapshot;
            if (!wasValid) { return; } // ignore the 1st snapshot
        }

        emit this->airspaceAircraftSnapshot(snapshot);
    }
} // ns

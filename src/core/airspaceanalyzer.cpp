// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/airspaceanalyzer.h"

#include <QDateTime>
#include <QMetaObject>
#include <QReadLocker>
#include <QString>
#include <QThread>
#include <QWriteLocker>

#include "core/airspacemonitor.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/transponder.h"
#include "misc/logmessage.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"
#include "misc/threadutils.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;
using namespace swift::core::fsd;

static constexpr std::chrono::milliseconds updateInterval { 7500 };

namespace swift::core
{
    CAirspaceAnalyzer::CAirspaceAnalyzer(IOwnAircraftProvider *ownAircraftProvider, CFSDClient *fsdClient,
                                         CAirspaceMonitor *airspaceMonitorParent)
        : CContinuousWorker(airspaceMonitorParent, "CAirspaceAnalyzer"), COwnAircraftAware(ownAircraftProvider),
          CRemoteAircraftAware(airspaceMonitorParent), m_updateTimer(this, "CAirspaceAnalyzer")
    {
        Q_ASSERT_X(fsdClient, Q_FUNC_INFO, "Network object required to connect");

        // all in new thread from here on
        this->setObjectName(this->getName());
        m_updateTimer.startTimer(updateInterval);
        m_lastWatchdogCallMsSinceEpoch = QDateTime::currentMSecsSinceEpoch();
        bool c = connect(&m_updateTimer, &CThreadedTimer::timeout, this, &CAirspaceAnalyzer::onTimeout);
        Q_ASSERT(c);

        // network connected

        // those are CID and not callsign related
        // c = connect(fsdClient, &CFSDClient::deletePilotReceived, this,
        // &CAirspaceAnalyzer::watchdogRemoveAircraftCallsign, Qt::QueuedConnection); Q_ASSERT(c); c =
        // connect(fsdClient, &CFSDClient::deleteAtcReceived, this, &CAirspaceAnalyzer::watchdogRemoveAtcCallsign,
        // Qt::QueuedConnection); Q_ASSERT(c);

        c = connect(fsdClient, &CFSDClient::connectionStatusChanged, this,
                    &CAirspaceAnalyzer::onConnectionStatusChanged, Qt::QueuedConnection);
        Q_ASSERT(c);

        // network situations
        c = connect(fsdClient, &CFSDClient::pilotDataUpdateReceived, this, &CAirspaceAnalyzer::onNetworkPositionUpdate,
                    Qt::QueuedConnection);
        Q_ASSERT(c);
        c = connect(fsdClient, &CFSDClient::atcDataUpdateReceived, this, &CAirspaceAnalyzer::watchdogTouchAtcCallsign,
                    Qt::QueuedConnection);
        Q_ASSERT(c);

        // Monitor
        c = connect(airspaceMonitorParent, &CAirspaceMonitor::addedAircraftSituation, this,
                    &CAirspaceAnalyzer::watchdogTouchAircraftCallsign);
        Q_ASSERT(c);
        c = connect(airspaceMonitorParent, &CAirspaceMonitor::removedAircraft, this,
                    &CAirspaceAnalyzer::watchdogRemoveAircraftCallsign);
        Q_ASSERT(c);
        c = connect(airspaceMonitorParent, &CAirspaceMonitor::atcStationDisconnected, this,
                    &CAirspaceAnalyzer::onAtcStationDisconnected);
        Q_ASSERT(c);

        // --------------------
        Q_UNUSED(c)

        // start in own thread
        this->start(QThread::LowestPriority);
    }

    CAirspaceAircraftSnapshot CAirspaceAnalyzer::getLatestAirspaceAircraftSnapshot() const
    {
        QReadLocker l(&m_lockSnapshot);
        return m_latestAircraftSnapshot;
    }

    void CAirspaceAnalyzer::setSimulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft,
                                                                  const CLength &maxRenderedDistance)
    {
        QWriteLocker l(&m_lockRestrictions);
        m_simulatorRenderedAircraftRestricted = restricted;
        m_simulatorRenderingEnabled = enabled;
        m_simulatorMaxRenderedAircraft = maxAircraft;
        m_simulatorMaxRenderedDistance = maxRenderedDistance;
    }

    void CAirspaceAnalyzer::onNetworkPositionUpdate(const CAircraftSituation &situation,
                                                    const CTransponder &transponder)
    {
        Q_UNUSED(transponder)
        this->watchdogTouchAircraftCallsign(situation);
    }

    void CAirspaceAnalyzer::onAtcStationDisconnected(const CAtcStation &station)
    {
        const CCallsign cs = station.getCallsign();
        this->watchdogRemoveAtcCallsign(cs);
    }

    void CAirspaceAnalyzer::watchdogTouchAircraftCallsign(const CAircraftSituation &situation)
    {
        const CCallsign cs = situation.getCallsign();
        Q_ASSERT_X(!cs.isEmpty(), Q_FUNC_INFO, "No callsign in situaton");
        m_aircraftCallsignTimestamps[cs] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::watchdogTouchAtcCallsign(const CCallsign &callsign, const CFrequency &frequency,
                                                     const CCoordinateGeodetic &position, const CLength &range)
    {
        Q_UNUSED(frequency)
        Q_UNUSED(position)
        Q_UNUSED(range)
        m_atcCallsignTimestamps[callsign] = QDateTime::currentMSecsSinceEpoch();
    }

    void CAirspaceAnalyzer::onConnectionStatusChanged(CConnectionStatus oldStatus, CConnectionStatus newStatus)
    {
        Q_UNUSED(oldStatus)
        if (newStatus.isDisconnected())
        {
            this->clear();
            m_updateTimer.stopTimer();
        }
        else if (newStatus.isConnected()) { m_updateTimer.startTimer(updateInterval); }
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
        constexpr auto callThresholdMs = static_cast<qint64>(updateInterval.count() * 1.5);
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
        const std::chrono::milliseconds aircraftTimeoutMs = m_timeoutAircraft;
        const std::chrono::milliseconds atcTimeoutMs = m_timeoutAtc;
        const qint64 timeoutAircraftEpochMs = currentTimeMsEpoch - aircraftTimeoutMs.count();
        const qint64 timeoutAtcEpochMs = currentTimeMsEpoch - atcTimeoutMs.count();
        const bool enabled = m_enabledWatchdog;

        const QList<CCallsign> callsignsAircraft = m_aircraftCallsignTimestamps.keys();
        for (const CCallsign &callsign : callsignsAircraft) // clazy:exclude=container-anti-pattern,range-loop
        {
            if (!enabled)
            {
                m_aircraftCallsignTimestamps[callsign] = timeoutAircraftEpochMs + 1000;
            } // fake value so it can be re-enabled
            const qint64 tsv = m_aircraftCallsignTimestamps.value(callsign);
            if (tsv > timeoutAircraftEpochMs) { continue; }
            CLogMessage(this).debug() << QStringLiteral("Aircraft '%1' timed out after %2ms")
                                             .arg(callsign.toQString())
                                             .arg(currentTimeMsEpoch - tsv);
            m_aircraftCallsignTimestamps.remove(callsign);
            emit this->timeoutAircraft(callsign);
        }

        const QList<CCallsign> callsignsAtc = m_atcCallsignTimestamps.keys();
        for (const CCallsign &callsign : callsignsAtc) // clazy:exclude=container-anti-pattern,range-loop
        {
            if (!enabled)
            {
                m_aircraftCallsignTimestamps[callsign] = timeoutAtcEpochMs + 1000;
            } // fake value so it can be re-enabled
            const qint64 tsv = m_atcCallsignTimestamps.value(callsign);
            if (tsv > timeoutAtcEpochMs) { continue; }
            CLogMessage(this).debug() << QStringLiteral("ATC '%1' timed out after %2ms")
                                             .arg(callsign.toQString())
                                             .arg(currentTimeMsEpoch - tsv);
            m_atcCallsignTimestamps.remove(callsign);
            emit this->timeoutAtc(callsign);
        }
    }

    void CAirspaceAnalyzer::analyzeAirspace()
    {
        Q_ASSERT_X(!CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Expect to run in background thread");
        Q_ASSERT_X(thread() != qApp->thread(), Q_FUNC_INFO, "Expect to run in background thread affinity");

        bool restricted {};
        bool enabled {};
        int maxAircraft {};
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
        CAirspaceAircraftSnapshot snapshot(aircraftInRange, restricted, enabled, maxAircraft, maxRenderedDistance);

        // lock block
        {
            QWriteLocker l(&m_lockSnapshot);
            bool wasValid = m_latestAircraftSnapshot.isValidSnapshot();
            if (wasValid) { snapshot.setRestrictionChanged(m_latestAircraftSnapshot); }
            m_latestAircraftSnapshot = snapshot;
            if (!wasValid) { return; } // ignore the 1st snapshot
        }

        emit this->airspaceAircraftSnapshot(snapshot);
    }
} // namespace swift::core

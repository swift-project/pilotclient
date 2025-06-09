// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AIRSPACE_ANALYZER_H
#define SWIFT_CORE_AIRSPACE_ANALYZER_H

#include <atomic>
#include <chrono>

#include <QHash>
#include <QReadWriteLock>
#include <QtGlobal>

#include "core/fsd/fsdclient.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/atcstation.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/network/connectionstatus.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/units.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/threadedtimer.h"
#include "misc/worker.h"

namespace swift::misc::aviation
{
    class CAircraftSituation;
    class CCallsign;
    class CTransponder;
} // namespace swift::misc::aviation

namespace swift::core
{
    class CAirspaceMonitor;

    //! Class monitoring and analyzing (closest aircraft, outdated aircraft / watchdog) airspace
    //! in background.
    //!
    //! \details Watchdog functionality: This class was introduced due to a flaw in the VATSIM server implementation:
    //! Every client needs to send an add/delete packet on its own to inform other
    //!          clients nearby. The server does not take care of that. When a client crashes, no delete packet is ever
    //!          sent. This class therefore monitors callsigns and emits a timeout signal if it wasn't resetted during
    //!          the specified timeout value.
    //!
    class SWIFT_CORE_EXPORT CAirspaceAnalyzer :
        public swift::misc::CContinuousWorker,
        public swift::misc::simulation::COwnAircraftAware,
        public swift::misc::simulation::CRemoteAircraftAware
    {
        Q_OBJECT

    public:
        //! List of callsigns and their last activity
        using CCallsignTimestampSet = QHash<swift::misc::aviation::CCallsign, qint64>;

        //! Constructor
        CAirspaceAnalyzer(swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                          fsd::CFSDClient *fsdClient, CAirspaceMonitor *airspaceMonitorParent);

        //! Destructor
        virtual ~CAirspaceAnalyzer() override;

        //! Get the latest snapshot
        //! \threadsafe
        swift::misc::simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

        //! Render restrictions in simulator
        void
        setSimulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft,
                                              const swift::misc::physical_quantities::CLength &maxRenderedDistance);

        //! Enable/disable watchdog
        //! \remark primarily for debugging, where stopping at a breakpoint can cause multiple timeouts
        void setEnabledWatchdog(bool enabled) { m_enabledWatchdog = enabled; }

        //! Clear
        void clear();

    signals:
        //! Callsign has timed out
        void timeoutAircraft(const swift::misc::aviation::CCallsign &callsign);

        //! Callsign has timed out
        void timeoutAtc(const swift::misc::aviation::CCallsign &callsign);

        //! New aircraft snapshot
        void airspaceAircraftSnapshot(const swift::misc::simulation::CAirspaceAircraftSnapshot &snapshot);

    private:
        //! Remove callsign from watch list
        void watchdogRemoveAircraftCallsign(const swift::misc::aviation::CCallsign &callsign);

        //! Remove callsign from watch list
        void watchdogRemoveAtcCallsign(const swift::misc::aviation::CCallsign &callsign);

        //! Reset timestamp for callsign
        void watchdogTouchAircraftCallsign(const swift::misc::aviation::CAircraftSituation &situation);

        //! Reset timestamp for callsign
        void watchdogTouchAtcCallsign(const swift::misc::aviation::CCallsign &callsign,
                                      const swift::misc::physical_quantities::CFrequency &frequency,
                                      const swift::misc::geo::CCoordinateGeodetic &position,
                                      const swift::misc::physical_quantities::CLength &range);

        //! Connection status of network changed
        void onConnectionStatusChanged(swift::misc::network::CConnectionStatus oldStatus,
                                       swift::misc::network::CConnectionStatus newStatus);

        //! Network position update
        void onNetworkPositionUpdate(const swift::misc::aviation::CAircraftSituation &situation,
                                     const swift::misc::aviation::CTransponder &transponder);

        //! ATC station disconnected
        void onAtcStationDisconnected(const swift::misc::aviation::CAtcStation &station);

        //! Run a check
        void onTimeout();

        //! Check for time outs
        void watchdogCheckTimeouts();

        //! Analyze the airspace
        void analyzeAirspace();

        // watchdog
        CCallsignTimestampSet m_aircraftCallsignTimestamps; //!< for watchdog (pilots)
        CCallsignTimestampSet m_atcCallsignTimestamps; //!< for watchdog (ATC)
        std::chrono::seconds m_timeoutAircraft { 15 }; //!< Timeout value for watchdog functionality
        std::chrono::seconds m_timeoutAtc { 50 }; //!< Timeout value for watchdog functionality
        qint64 m_lastWatchdogCallMsSinceEpoch; //!< when last called
        qint64 m_doNotRunAgainBefore = -1; //!< do not run again before, also used to detect debugging
        std::atomic_bool m_enabledWatchdog { true }; //!< watchdog enabled
        misc::CThreadedTimer m_updateTimer; //!< Thread safe timer for update timeout

        // snapshot
        swift::misc::simulation::CAirspaceAircraftSnapshot m_latestAircraftSnapshot;
        bool m_simulatorRenderedAircraftRestricted = false;
        bool m_simulatorRenderingEnabled = true;
        int m_simulatorMaxRenderedAircraft = -1;
        swift::misc::physical_quantities::CLength m_simulatorMaxRenderedDistance { 0.0, nullptr };
        mutable QReadWriteLock m_lockSnapshot; //!< lock snapshot
        mutable QReadWriteLock m_lockRestrictions; //!< lock simulator restrictions
    };
} // namespace swift::core

#endif // SWIFT_CORE_AIRSPACE_ANALYZER_H

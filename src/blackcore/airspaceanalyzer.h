/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRSPACE_ANALYZER_H
#define BLACKCORE_AIRSPACE_ANALYZER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/fsd/fsdclient.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/worker.h"

#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QTimer>
#include <QtGlobal>
#include <atomic>

namespace BlackMisc::Aviation
{
    class CAircraftSituation;
    class CCallsign;
    class CTransponder;
}

namespace BlackCore
{
    class CAirspaceMonitor;

    //! Class monitoring and analyzing (closest aircraft, outdated aircraft / watchdog) airspace
    //! in background.
    //!
    //! \details Watchdog functionality: This class was introduced due to a flaw in the VATSIM server implementation: Every client needs to send an add/delete packet on its own to inform other
    //!          clients nearby. The server does not take care of that. When a client crashes, no delete packet is ever sent. This class therefore monitors callsigns and emits a timeout signal if it
    //!          wasn't resetted during the specified timeout value.
    //!
    class BLACKCORE_EXPORT CAirspaceAnalyzer :
        public BlackMisc::CContinuousWorker,
        public BlackMisc::Simulation::COwnAircraftAware,
        public BlackMisc::Simulation::CRemoteAircraftAware
    {
        Q_OBJECT

    public:
        //! List of callsigns and their last activity
        using CCallsignTimestampSet = QHash<BlackMisc::Aviation::CCallsign, qint64>;

        //! Constructor
        CAirspaceAnalyzer(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                          Fsd::CFSDClient *fsdClient,
                          CAirspaceMonitor *airspaceMonitorParent);

        //! Destructor
        virtual ~CAirspaceAnalyzer() override;

        //! Get the latest snapshot
        //! \threadsafe
        BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const;

        //! Render restrictions in simulator
        void setSimulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const BlackMisc::PhysicalQuantities::CLength &maxRenderedDistance);

        //! Enable/disable watchdog
        //! \remark primarily for debugging, where stopping at a breakpoint can cause multiple timeouts
        void setEnabledWatchdog(bool enabled) { m_enabledWatchdog = enabled; }

        //! Clear
        void clear();

    signals:
        //! Callsign has timed out
        void timeoutAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Callsign has timed out
        void timeoutAtc(const BlackMisc::Aviation::CCallsign &callsign);

        //! New aircraft snapshot
        void airspaceAircraftSnapshot(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

    private:
        //! Remove callsign from watch list
        void watchdogRemoveAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Remove callsign from watch list
        void watchdogRemoveAtcCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Reset timestamp for callsign
        void watchdogTouchAircraftCallsign(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Reset timestamp for callsign
        void watchdogTouchAtcCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency,
                                      const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        //! Connection status of network changed
        void onConnectionStatusChanged(BlackMisc::Network::CConnectionStatus oldStatus, BlackMisc::Network::CConnectionStatus newStatus);

        //! Network position update
        void onNetworkPositionUpdate(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! ATC stations online
        void onChangedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool isConnected);

        //! Run a check
        void onTimeout();

        //! Check for time outs
        void watchdogCheckTimeouts();

        //! Analyze the airspace
        void analyzeAirspace();

        // watchdog
        CCallsignTimestampSet m_aircraftCallsignTimestamps; //!< for watchdog (pilots)
        CCallsignTimestampSet m_atcCallsignTimestamps; //!< for watchdog (ATC)
        BlackMisc::PhysicalQuantities::CTime m_timeoutAircraft = { 15, BlackMisc::PhysicalQuantities::CTimeUnit::s() }; //!< Timeout value for watchdog functionality
        BlackMisc::PhysicalQuantities::CTime m_timeoutAtc = { 50, BlackMisc::PhysicalQuantities::CTimeUnit::s() }; //!< Timeout value for watchdog functionality
        qint64 m_lastWatchdogCallMsSinceEpoch; //!< when last called
        qint64 m_doNotRunAgainBefore = -1; //!< do not run again before, also used to detect debugging
        std::atomic_bool m_enabledWatchdog { true }; //!< watchdog enabled

        // snapshot
        BlackMisc::Simulation::CAirspaceAircraftSnapshot m_latestAircraftSnapshot;
        bool m_simulatorRenderedAircraftRestricted = false;
        bool m_simulatorRenderingEnabled = true;
        int m_simulatorMaxRenderedAircraft = -1;
        BlackMisc::PhysicalQuantities::CLength m_simulatorMaxRenderedDistance { 0.0, nullptr };
        mutable QReadWriteLock m_lockSnapshot; //!< lock snapshot
        mutable QReadWriteLock m_lockRestrictions; //!< lock simulator restrictions
    };
} // namespace

#endif

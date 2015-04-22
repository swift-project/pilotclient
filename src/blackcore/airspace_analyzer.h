/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRSPACE_ANALYZER_H
#define BLACKCORE_AIRSPACE_ANALYZER_H

#include "blackcore/network.h"
#include "blackmisc/worker.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/aviation/callsign.h"
#include <QObject>
#include <QHash>

namespace BlackCore
{
    //! Class monitoring and analyzing (closests aircraft, outdated aircraft / watchdog) airspace
    //! in background.
    //!
    //! \details Watchdog functionality: This class was introduced due to a flaw in the VATSIM server implementation: Every client needs to send an add/delete packet on its own to inform other
    //!          clients nearby. The server does not take care of that. When a client crashes, no delete packet is ever sent. This class therefore monitors callsigns and emits a timeout signal if it
    //!          wasn't resetted during the specified timeout value.
    //!
    class CAirspaceAnalyzer : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! List of callsigns and their last activity
        typedef QHash<BlackMisc::Aviation::CCallsign, qint64> CCallsignTimestampSet;

        //! Constructor
        CAirspaceAnalyzer(INetwork *network, QObject *parent);

    public slots:
        //! Clear
        void clear();

    signals:
        //! Callsign has timed out
        void timeoutAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Callsign has timed out
        void timeoutAtc(const BlackMisc::Aviation::CCallsign &callsign);

    private slots:
        //! Remove callsign from watch list
        void ps_watchdogRemoveAircraftCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Remove callsign from watch list
        void ps_watchdogRemoveAtcCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Reset timestamp for callsign
        void ps_watchdogTouchAircraftCallsign(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! Reset timestamp for callsign
        void ps_watchdogTouchAtcCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                                         const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);

        //! Connection status of network changed
        void ps_onConnectionStatusChanged(INetwork::ConnectionStatus oldStatus, INetwork::ConnectionStatus newStatus);

    private:
        //! Check for time outs
        void watchdogCheckTimeouts();

        CCallsignTimestampSet m_aircraftCallsignTimestamps;
        CCallsignTimestampSet m_atcCallsignTimestamps;
        BlackMisc::PhysicalQuantities::CTime m_timeoutAircraft = {15, BlackMisc::PhysicalQuantities::CTimeUnit::s() }; //!< Timeout value for watchdog functionality
        BlackMisc::PhysicalQuantities::CTime m_timeoutAtc = {50, BlackMisc::PhysicalQuantities::CTimeUnit::s() }; //!< Timeout value for watchdog functionality

    };

} // namespace

#endif

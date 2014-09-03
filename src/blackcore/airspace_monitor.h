/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRSPACE_MONITOR_H
#define BLACKCORE_AIRSPACE_MONITOR_H

#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/avflightplan.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/avcallsignlist.h"
#include "network.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"

namespace BlackCore
{

    /*!
     * Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
     */
    class CAirspaceMonitor : public QObject
    {
        Q_OBJECT

    public:
        //! Constructor
        CAirspaceMonitor(QObject *parent, INetwork *network, CVatsimBookingReader *bookings, CVatsimDataFileReader *dataFile);

        //! Returns the list of users we know about
        BlackMisc::Network::CUserList getUsers() const;

        //! Returns a list of the users corresponding to the given callsigns
        BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const;

        //! Returns the flightplan for the given callsign
        BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign);

        //! Returns this list of other clients we know about
        BlackMisc::Network::CClientList getOtherClients() const { return m_otherClients; }

        //! Returns a list of other clients corresponding to the given callsigns
        BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const;

        //! Returns a METAR for the given airport, if available
        BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode);

        //! Returns the current online ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const { return m_atcStationsOnline; }

        //! Returns the current booked ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const { return m_atcStationsBooked; }

        //! Returns the current aircraft in range
        BlackMisc::Aviation::CAircraftList getAircraftInRange() const { return m_aircraftsInRange; }

        //! Returns the closest ATC station operating on the given frequency, if any
        BlackMisc::Aviation::CAtcStation getAtcStationForComUnit(const BlackMisc::Aviation::CComSystem &comSystem);

        //! Request to update other clients' data from the network
        void requestDataUpdates();

        //! Request to update ATC stations' ATIS data from the network
        void requestAtisUpdates();

        //! Create dummy entries for performance tests
        void testCreateDummyOnlineAtcStations(int number);

    signals:
        //! Online ATC stations were changed
        void changedAtcStationsOnline();

        //! Booked ATC stations were changed
        void changedAtcStationsBooked();

        //! Connection status of an ATC station was changed
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool isConnected);

        //! Aircrafts were changed
        void changedAircraftsInRange();

        //! A new aircraft appeared
        void addedAircraft(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation);

        //! An aircraft's situation was changed
        void changedAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation);

        //! An aircraft disappeared
        void removedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Sent a status message
        void statusMessage(const BlackMisc::CStatusMessage &msg);

    public slots:
        //! Own aircraft updated
        void setOwnAircraft(const BlackMisc::Aviation::CAircraft &ownAircraft) { m_ownAircraft = ownAircraft; }

        //! Own aircraft model updated
        void setOwnAircraftModel(const BlackMisc::Network::CAircraftModel &model) { m_ownAircraftModel = model; }
            
    public:
        //! Clear the contents
        void clear()
        {
            m_atcStationsOnline.clear();
            m_atcStationsBooked.clear();
            m_aircraftsInRange.clear();
            m_otherClients.clear();
            m_metarCache.clear();
            m_flightPlanCache.clear();
        }

    private:
        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked;
        BlackMisc::Aviation::CAircraftList m_aircraftsInRange;
        BlackMisc::Network::CClientList m_otherClients;
        QMap<BlackMisc::Aviation::CAirportIcao, BlackMisc::Aviation::CInformationMessage> m_metarCache;
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CFlightPlan> m_flightPlanCache;

        BlackMisc::Aviation::CAircraft m_ownAircraft;
        BlackMisc::Network::CAircraftModel m_ownAircraftModel;

        INetwork *m_network;
        CVatsimBookingReader *m_vatsimBookingReader;
        CVatsimDataFileReader *m_vatsimDataFileReader;

        // FIXME (MS) should be in INetwork
        void sendFsipiCustomPacket(const BlackMisc::Aviation::CCallsign &recipientCallsign) const;
        void sendFsipirCustomPacket(const BlackMisc::Aviation::CCallsign &recipientCallsign) const;
        QStringList createFsipiCustomPacketData() const;

    private slots:
        void ps_realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void ps_fsipirCustomPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &, const QString &, const QString &, const QString &model);
        void ps_serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void ps_metarReceived(const QString &metarMessage);
        void ps_flightplanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);
        void ps_atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);
        void ps_atcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_atisReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);
        void ps_atisVoiceRoomReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void ps_atisLogoffTimeReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);
        void ps_aircraftUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);
        void ps_pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_frequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
    };

} // namespace

#endif // guard

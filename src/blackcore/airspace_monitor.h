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

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "network.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"
#include "airspace_watchdog.h"

namespace BlackCore
{

    /*!
     * Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
     */
    class CAirspaceMonitor :
        public QObject,
        public BlackMisc::Simulation::IRemoteAircraftProvider,  // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Simulation::COwnAircraftAwareReadOnly // used to obtain in memory inofmration about own aircraft
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

    public:
        //! Constructor
        CAirspaceMonitor(QObject *parent, const BlackMisc::Simulation::IOwnAircraftProviderReadOnly *ownAircraft, INetwork *network, CVatsimBookingReader *bookings, CVatsimDataFileReader *dataFile);

        //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraft
        virtual const BlackMisc::Simulation::CSimulatedAircraftList &remoteAircraft() const override;

        //! \copydoc IRemoteAircraftProvider::renderedAircraft
        virtual BlackMisc::Simulation::CSimulatedAircraftList &remoteAircraft() override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
        virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftSituations
        virtual BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
        virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
        virtual BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() override;

        //! \copydoc IRemoteAircraftProvider::remoteAircraftParts
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator) override;

        //! \copydoc IRemoteAircraftProvider::updateAircraftModel
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) override;

        //! \copydoc IRemoteAircraftProvider::updateFastPositionEnabled
        virtual bool updateFastPositionUpdates(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const QString &originator) override;

        //! Returns the list of users we know about
        BlackMisc::Network::CUserList getUsers() const;

        //! Returns a list of the users corresponding to the given callsigns
        BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

        //! Returns the flightplan for the given callsign
        //! \remarks pseudo synchronous, call the async functions and waits for result
        BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign);

        //! Returns this list of other clients we know about
        BlackMisc::Network::CClientList getOtherClients() const;

        //! Returns a list of other clients corresponding to the given callsigns
        BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

        //! Returns a METAR for the given airport, if available
        BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode);

        //! Returns the current online ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const { return m_atcStationsOnline; }

        //! Returns the current booked ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const { return m_atcStationsBooked; }

        //! Returns the closest ATC station operating on the given frequency, if any
        BlackMisc::Aviation::CAtcStation getAtcStationForComUnit(const BlackMisc::Aviation::CComSystem &comSystem);

        //! Clear the contents
        void clear();

        //! Connection status
        void setConnected(bool connected);

        //! Request to update other clients' data from the network
        void requestDataUpdates();

        //! Request to update ATC stations' ATIS data from the network
        void requestAtisUpdates();

        //! Create dummy entries for performance tests
        void testCreateDummyOnlineAtcStations(int number);

        //! Test injected aircraft parts
        void testAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts, bool incremental);

        //! Aircraft situations
        virtual BlackMisc::Aviation::CAircraftSituationList getRenderedAircraftSituations() const;

        //! Aircraft parts
        virtual BlackMisc::Aviation::CAircraftPartsList getRenderedAircraftParts() const;

        //! \copydoc IRemoteAircraftProviderReadOnly::connectSignals
        //! \copydoc IRemoteAircraftProviderReadOnly::connectSignals
        virtual bool connectRemoteAircraftProviderSignals(
            std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
            std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
        ) override;

        static const qint64 AircraftSituationsRemovedOffsetMs = 30 * 1000; //!< situations older than now - offset will be removed
        static const qint64 AircraftPartsRemoveOffsetMs = 30* 1000;        //!< parts older than now - offset will be removed

    signals:

        //--- signals for the provider, work locally only (not in DBus)

        //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftSituation
        void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftPart
        void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! \copydoc IRemoteAircraftProviderReadOnly::removedAircraft
        void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! Online ATC stations were changed
        void changedAtcStationsOnline();

        //! Booked ATC stations were changed
        void changedAtcStationsBooked();

        //! Connection status of an ATC station was changed
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool isConnected);

        //! Aircraft were changed
        void changedAircraftInRange();

        //! A new aircraft appeared
        void addedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! Read for model matching
        void readyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

    private:
        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked;
        BlackMisc::Network::CClientList      m_otherClients;
        BlackMisc::Simulation::CSimulatedAircraftList  m_aircraftInRange;
        BlackMisc::Aviation::CAircraftSituationList    m_aircraftSituations;
        BlackMisc::Aviation::CAircraftPartsList        m_aircraftParts;

        QMap<BlackMisc::Aviation::CAirportIcao, BlackMisc::Aviation::CInformationMessage> m_metarCache;
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CFlightPlan>            m_flightPlanCache;
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CAircraftIcao>          m_icaoCodeCache;

        INetwork              *m_network               = nullptr;
        CVatsimBookingReader  *m_vatsimBookingReader   = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader  = nullptr;
        CAirspaceWatchdog      m_atcWatchdog;
        CAirspaceWatchdog      m_aircraftWatchdog;
        bool                   m_serverSupportsNameQuery = false; //!< not all servers support name query
        bool                   m_connected = false;               //!< retrieve data

        //! Remove ATC online stations
        void removeAllOnlineAtcStations();

        //! Remove all aircraft in range
        void removeAllAircraft();

        //! Remove all other clients
        void removeAllOtherClients();

        //! Remove data from caches
        void removeFromAircraftCaches(const BlackMisc::Aviation::CCallsign &callsign);

        //! Schedule a ready for model matching
        void fireDelayedReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, int trial = 1, int delayMs = 2500);

    private slots:
        //! Create aircraft in range, this is the only place where a new aircraft should be added
        void ps_aircraftUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);
        void ps_aircraftInterimUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
        void ps_realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void ps_customFSinnPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &p1, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &model);
        void ps_serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void ps_metarReceived(const QString &metarMessage);
        void ps_flightPlanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void ps_atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);
        void ps_atcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_atisReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);
        void ps_atisVoiceRoomReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void ps_atisLogoffTimeReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icaoData);
        void ps_pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_frequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);
        void ps_receivedDataFile();
        void ps_aircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, bool isFull);

        //!  Send the information if aircraft and(!) client are vailable
        void ps_sendReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, int trial);
    };

} // namespace

#endif // guard

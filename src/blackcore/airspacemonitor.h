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

#include "blackcore/blackcoreexport.h"
#include "blackcore/network.h"
#include "blackcore/vatsimmetarreader.h"
#include "airspaceanalyzer.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/weather/metarset.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/callsignset.h"

namespace BlackCore
{
    class CWebDataServices;

    //! Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
    //! Central instance of data for \sa IRemoteAircraftProvider.
    class BLACKCORE_EXPORT CAirspaceMonitor :
        public QObject,
        public BlackMisc::Simulation::IRemoteAircraftProvider,  // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Simulation::COwnAircraftAware // used to obtain in memory information about own aircraft
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

    public:
        //! Constructor
        CAirspaceMonitor(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraft, INetwork *network, QObject *parent);

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::getAircraftInRange
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::getAircraftInRangeForCallsign
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::getAircraftInRangeForCallsign
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::getAircraftInRangeCount
        //! \ingroup remoteaircraftprovider
        virtual int getAircraftInRangeCount() const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::getLatestAirspaceAircraftSnapshot
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::remoteAircraftSituations
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::remoteAircraftSituationsCount
        //! \ingroup remoteaircraftprovider
        virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::remoteAircraftParts
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore = -1) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::isRemoteAircraftSupportingParts
        //! \ingroup remoteaircraftprovider
        virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::remoteAircraftSupportingParts
        //! \ingroup remoteaircraftprovider
        virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::updateAircraftEnabled
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::updateAircraftModel
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::updateFastPositionEnabled
        //! \ingroup remoteaircraftprovider
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::updateAircraftRendered
        //! \ingroup remoteaircraftprovider
        virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::updateMarkAllAsNotRendered
        //! \ingroup remoteaircraftprovider
        virtual void updateMarkAllAsNotRendered(const BlackMisc::CIdentifier &originator) override;

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

        //! Returns the current online ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const { return m_atcStationsOnline; }

        //! Returns the current booked ATC stations
        BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const { return m_atcStationsBooked; }

        //! Returns the closest ATC station operating on the given frequency, if any
        BlackMisc::Aviation::CAtcStation getAtcStationForComUnit(const BlackMisc::Aviation::CComSystem &comSystem);

        //! Logging for matching process (see why a model is matched like it is)
        void logMatchingProcess(bool log) { this->m_logMatchingProcess = log; }

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
        void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental);

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::connectRemoteAircraftProviderSignals
        virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void(const BlackMisc::Aviation::CAircraftSituation &)>          addedSituationSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)>   addedPartsSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &)>                   removedAircraftSlot,
            std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot
        ) override;

        //! Is interim position sending enabled?
        bool isFastPositionSendingEnabled() const;

        //! Enable interim position sending
        void enableFastPositionSending(bool enable);

        //! Analyzer
        CAirspaceAnalyzer *analyzer() const { return m_analyzer; }

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

        static const qint64 AircraftSituationsRemovedOffsetMs = 30 * 1000; //!< situations older than now - offset will be removed
        static const qint64 AircraftPartsRemoveOffsetMs = 30 * 1000;       //!< parts older than now - offset will be removed

    signals:
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

        //! Parts added
        void addedAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

        //! Situation added
        void addedAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Read for model matching
        void readyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! An aircraft disappeared
        void removedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! \copydoc CAirspaceAnalyzer::airspaceAircraftSnapshot
        void airspaceAircraftSnapshot(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

    private:
        BlackMisc::Aviation::CAtcStationList           m_atcStationsOnline;
        BlackMisc::Aviation::CAtcStationList           m_atcStationsBooked;
        BlackMisc::Network::CClientList                m_otherClients;
        BlackMisc::Simulation::CSimulatedAircraftList  m_aircraftInRange; //!< aircraft, thread safe access required

        // hashs, because not sorted by key but keeping order
        CSituationsPerCallsign m_situationsByCallsign; //!< situations, for performance reasons per callsign, thread safe access required
        CPartsPerCallsign      m_partsByCallsign;      //!< parts, for performance reasons per callsign, thread safe access required
        BlackMisc::Aviation::CCallsignSet m_aircraftSupportingParts; //!< aircraft supporting parts, thread safe access required

        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CFlightPlan>       m_flightPlanCache; //!< flight plan information retrieved any cached
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Simulation::CAircraftModel>  m_modelCache;      //!< any model information recevived from network temporarily stored until it is "completed". Will be removed when aircraft is moved to aircraft in range

        INetwork              *m_network                 = nullptr;
        CAirspaceAnalyzer     *m_analyzer                = nullptr; //!< owned analyzer
        bool                   m_serverSupportsNameQuery = false;   //!< not all servers support name query
        bool                   m_connected               = false;   //!< retrieve data
        bool                   m_sendInterimPositions    = false;   //!< send interim positions to other clients
        bool                   m_logMatchingProcess      = false;   //!< shall we log. information about the matching process
        QTimer                 m_interimPositionUpdateTimer;

        // locks
        mutable QReadWriteLock m_lockSituations; //!< lock for situations
        mutable QReadWriteLock m_lockParts;      //!< lock for parts
        mutable QReadWriteLock m_lockAircraft;   //!< lock aircraft

        //! Remove ATC online stations
        void removeAllOnlineAtcStations();

        //! Remove all aircraft in range
        void removeAllAircraft();

        //! Remove all other clients
        void removeAllOtherClients();

        //! Remove data from caches
        void removeFromAircraftCaches(const BlackMisc::Aviation::CCallsign &callsign);

        //! Schedule a "ready for model matching"
        void fireDelayedReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, int trial = 1, int delayMs = 2500);

        //! FSD or icao query received. Here we also replace the model with a model from DB if possible (reverse lookup)
        void icaoOrFsdDataReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery, const QString &modelString, BlackMisc::Simulation::CAircraftModel::ModelType type);

        //! Log.matching
        void logMatching(const QString &text) const;

        //! Store an aircraft situation
        //! \threadsafe
        void storeAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Store an aircraft part
        //! \threadsafe
        void storeAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

    private slots:
        //! Create aircraft in range, this is the only place where a new aircraft should be added
        void ps_aircraftUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! Create ATC station, this is the only place where an online ATC station should be added
        void ps_atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        //! Send the information if aircraft and(!) client are available
        //! \note it can take some time to obtain all data for model matching, so function recursively calls itself if something is still missing (trial)
        void ps_sendReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, int trial);

        void ps_realNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void ps_capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void ps_customFSinnPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &model);
        void ps_serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void ps_metarReceived(const QString &metarMessage);
        void ps_flightPlanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void ps_atcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_atisReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);
        void ps_atisVoiceRoomReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void ps_atisLogoffTimeReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void ps_icaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);
        void ps_pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void ps_frequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);
        void ps_receivedDataFile();
        void ps_aircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, bool isFull);
        void ps_aircraftInterimUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
        void ps_sendInterimPositions();
    };

} // namespace

#endif // guard

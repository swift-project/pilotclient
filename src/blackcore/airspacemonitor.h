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

#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <functional>

#include "blackcore/blackcoreexport.h"
#include "blackcore/network.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/clientprovider.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/identifiable.h"
#include "blackmisc/identifier.h"

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CComSystem;
        class CInformationMessage;
        class CTransponder;
    }
}

namespace BlackCore
{
    class CAirspaceAnalyzer;
    class INetwork;

    //! Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
    //! Central instance of data for \sa IRemoteAircraftProvider.
    class BLACKCORE_EXPORT CAirspaceMonitor :
        public QObject,
        public BlackMisc::Network::IClientProvider,            // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Simulation::IRemoteAircraftProvider, // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Simulation::COwnAircraftAware,       // used to obtain in memory information about own aircraft
        public BlackMisc::Simulation::CSimulationEnvironmentAware, // elevation info etc.
        public BlackMisc::CIdentifiable
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Network::IClientProvider)
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Constructor
        CAirspaceMonitor(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraft, INetwork *network, QObject *parent);

        //! \ingroup remoteaircraftprovider
        //! @{
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;
        virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual BlackMisc::Simulation::CAircraftModel getAircraftInRangeModelForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual int getAircraftInRangeCount() const override;
        virtual bool isAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
        virtual BlackMisc::Aviation::CAircraftSituationList remoteAircraftSituations(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual int remoteAircraftSituationsCount(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual BlackMisc::Aviation::CAircraftPartsList remoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore = -1) const override;
        virtual bool isRemoteAircraftSupportingParts(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual int getRemoteAircraftSupportingPartsCount() const override;
        virtual BlackMisc::Aviation::CCallsignSet remoteAircraftSupportingParts() const override;
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering) override;
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
        virtual bool updateAircraftNetworkModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) override;
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
        virtual bool updateAircraftRendered(const BlackMisc::Aviation::CCallsign &callsign, bool rendered) override;
        virtual bool updateAircraftGroundElevation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Geo::CElevationPlane &elevation) override;
        virtual void updateMarkAllAsNotRendered() override;
        virtual BlackMisc::CStatusMessageList getAircraftPartsHistory(const BlackMisc::Aviation::CCallsign &callsign) const override;
        virtual bool isAircraftPartsHistoryEnabled() const override;
        virtual void enableAircraftPartsHistory(bool enabled) override;
        //! @}

        //! \ingroup remoteaircraftprovider
        //! \ingroup reverselookup
        //! @{
        virtual void enableReverseLookupMessages(bool enabled) override;
        virtual bool isReverseLookupMessagesEnabled() const override;
        virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
        //! @}

        //! Returns the list of users we know about
        BlackMisc::Network::CUserList getUsers() const;

        //! Returns a list of the users corresponding to the given callsigns
        BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

        //! Returns the loaded flight plan for the given callsign
        //! \remarks only use this if a network loaded flight plan is directly needed
        //! \remarks pseudo synchronous, call the async functions and waits for result
        BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign);

        //! Try to get flight plan remarks
        //! \remarks returns a value only if the flight plan is already cached or can be obtained from VATSIM reader
        //! \threadsafe
        BlackMisc::Aviation::CFlightPlanRemarks tryToGetFlightPlanRemarks(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Returns the current online ATC stations (consolidated with booked stations)
        BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const { return m_atcStationsOnline; }

        //! Returns the current booked ATC stations (consolidated with online stations)
        BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const { return m_atcStationsBooked; }

        //! Returns the closest ATC station operating on the given frequency, if any
        BlackMisc::Aviation::CAtcStation getAtcStationForComUnit(const BlackMisc::Aviation::CComSystem &comSystem);

        //! Clear the contents
        void clear();

        //! Request to update other clients' data from the network
        void requestDataUpdates();

        //! Request to update ATC stations' ATIS data from the network
        void requestAtisUpdates();

        //! Request updates of bookings
        void requestAtcBookingsUpdate();

        //! Create dummy entries for performance tests
        void testCreateDummyOnlineAtcStations(int number);

        //! Test injected aircraft parts
        void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental);

        //! \copydoc BlackMisc::Simulation::IRemoteAircraftProvider::connectRemoteAircraftProviderSignals
        virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationFunction,
            std::function<void(const BlackMisc::Aviation::CCallsign &, const BlackMisc::Aviation::CAircraftParts &)> partsFunction,
            std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftFunction,
            std::function<void(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotFunction
        ) override;

        //! Analyzer
        CAirspaceAnalyzer *analyzer() const { return m_analyzer; }

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

    signals:
        //! Online ATC stations were changed
        void changedAtcStationsOnline();

        //! Booked ATC stations were changed
        void changedAtcStationsBooked();

        //! Connection status of an ATC station was changed
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool isConnected);

        //! Aircraft were changed
        void changedAircraftInRange();

        //! Raw data as received from network
        void requestedNewAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftDesignator, const QString &airlineDesignator, const QString &livery);

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
        //! Used to temporary store FsInn data
        struct FsInnPacket
        {
            //! Default ctor
            FsInnPacket() {}

            //! Constructor
            FsInnPacket(const QString &aircraftIcaoDesignator, const QString &airlineDesignator, const QString &combinedCode, const QString &modelString);

            QString aircraftIcaoDesignator;
            QString airlineIcaoDesignator;
            QString combinedCode;
            QString modelString;
        };

        BlackMisc::Aviation::CAtcStationList          m_atcStationsOnline; //!< online ATC stations
        BlackMisc::Aviation::CAtcStationList          m_atcStationsBooked; //!< booked ATC stations
        BlackMisc::Simulation::CSimulatedAircraftList m_aircraftInRange;   //!< aircraft, thread safe access required
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::CStatusMessageList> m_reverseLookupMessages;
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::CStatusMessageList> m_aircraftPartsHistory;
        QMap<BlackMisc::Aviation::CCallsign, FsInnPacket> m_tempFsInnPackets;

        // hashs, because not sorted by key but keeping order
        CSituationsPerCallsign            m_situationsByCallsign;    //!< situations, for performance reasons per callsign, thread safe access required
        CPartsPerCallsign                 m_partsByCallsign;         //!< parts, for performance reasons per callsign, thread safe access required
        BlackMisc::Aviation::CCallsignSet m_aircraftSupportingParts; //!< aircraft supporting parts, thread safe access required

        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CFlightPlan> m_flightPlanCache; //!< flight plan information retrieved from network and cached

        INetwork          *m_network  = nullptr;  //!< corresponding network interface
        CAirspaceAnalyzer *m_analyzer = nullptr;  //!< owned analyzer
        bool m_enableReverseLookupMsgs = false;   //!< shall we log. information about the matching process
        bool m_enableAircraftPartsHistory = true; //!< shall we keep a history of aircraft parts
        bool m_bookingsRequested = false;         //!< bookings have been requested, it can happen we receive an BlackCore::Vatsim::CVatsimBookingReader::atcBookingsReadUnchanged signal

        // locks
        mutable QReadWriteLock m_lockSituations;   //!< lock for situations: m_situationsByCallsign
        mutable QReadWriteLock m_lockParts;        //!< lock for parts: m_partsByCallsign, m_aircraftSupportingParts
        mutable QReadWriteLock m_lockAircraft;     //!< lock aircraft: m_aircraftInRange
        mutable QReadWriteLock m_lockMessages;     //!< lock for messages
        mutable QReadWriteLock m_lockPartsHistory; //!< lock for aircraft parts

        //! Remove ATC online stations
        void removeAllOnlineAtcStations();

        //! Remove all aircraft in range
        //! \threadsafe
        void removeAllAircraft();

        //! Remove data from caches and logs
        //! \threadsafe
        void removeFromAircraftCachesAndLogs(const BlackMisc::Aviation::CCallsign &callsign);

        //! Network queries for ATC
        void sendInitialAtcQueries(const BlackMisc::Aviation::CCallsign &callsign);

        //! Network queries for pilots
        void sendInitialPilotQueries(const BlackMisc::Aviation::CCallsign &callsign, bool withIcaoQuery, bool withFsInn);

        //! Connected with network?
        bool isConnected() const;

        //! Get the currently connected server
        const BlackMisc::Network::CServer getConnectedServer() const;

        //! Supports VATSIM data file
        bool supportsVatsimDataFile() const;

        //! Distance calculation
        BlackMisc::PhysicalQuantities::CLength calculateDistanceToOwnAircraft(const BlackMisc::Aviation::CAircraftSituation &situation) const;

        //! Angle calculation
        BlackMisc::PhysicalQuantities::CAngle calculateBearingToOwnAircraft(const BlackMisc::Aviation::CAircraftSituation &situation) const;

        //! Store an aircraft situation
        //! \threadsafe
        //! \remark sets gnd flag from parts if parts are available
        void storeAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Store an aircraft part
        //! \threadsafe
        void storeAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

        //! Add new aircraft, ignored if aircraft already exists
        //! \remark position to own aircraft set, VATSIM data file data considered
        //! \threadsafe
        bool addNewAircraftInRange(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Init a new aircraft and add it or update model of existing aircraft
        //! \threadsafe
        BlackMisc::Simulation::CSimulatedAircraft addOrUpdateAircraftInRange(
            const BlackMisc::Aviation::CCallsign &callsign,
            const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString,
            BlackMisc::Simulation::CAircraftModel::ModelType modelType,
            BlackMisc::CStatusMessageList *log = nullptr);

        //! Update aircraft
        //! \threadsafe
        int updateAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true);

        //! Update online stations by callsign
        int updateOnlineStation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true, bool sendSignal = true);

        //! Update booked station by callsign
        int updateBookedStation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true, bool sendSignal = true);

        //! Call ps_customFSInnPacketReceived with stored packet
        void recallFsInnPacket(const BlackMisc::Aviation::CCallsign &callsign);

        //! Send the information if aircraft and(!) client are available
        //! \note it can take some time to obtain all data for model matching, so function recursively calls itself if something is still missing (trial)
        //! \sa reverseLookupModelWithFlightplanData
        void sendReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, int trial = 1);

        //! Reverse lookup messages
        //! \threadsafe
        //! \ingroup reverselookup
        //! @{
        void addReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CStatusMessageList &messages);
        void addReverseLookupMessage(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CStatusMessage &message);
        void addReverseLookupMessage(
            const BlackMisc::Aviation::CCallsign &callsign, const QString &message,
            BlackMisc::CStatusMessage::StatusSeverity severity = BlackMisc::CStatusMessage::SeverityInfo);
        //! @}

        //! Reverse lookup, if available flight plan data are considered
        //! \remark this is where a model is created based on network data
        //! \ingroup reverselookup
        BlackMisc::Simulation::CAircraftModel reverseLookupModelWithFlightplanData(
            const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcao,
            const QString &airlineIcao, const QString &livery, const QString &modelString,
            BlackMisc::Simulation::CAircraftModel::ModelType type, BlackMisc::CStatusMessageList *log);

        //! Create aircraft in range, this is the only place where a new aircraft should be added
        void onAircraftUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! Create ATC station, this is the only place where an online ATC station should be added
        void onAtcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        //! Receive FSInn packet
        //! \remark This can happen even without a query before
        void onCustomFSInnPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &modelString);

        void onRealNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void onCapabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void onServerReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void onFlightPlanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void onAtcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void onAtisReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);
        void onAtisVoiceRoomReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void onAtisLogoffTimeReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void onIcaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);
        void onPilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void onFrequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
        void onReceivedAtcBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);
        void onReadUnchangedAtcBookings();
        void onReceivedVatsimDataFile();
        void onAircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, int currentOffset);
        void onAircraftInterimUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
        void onConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus oldStatus, BlackCore::INetwork::ConnectionStatus newStatus);
    };
} // namespace

#endif // guard

// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_AIRSPACE_MONITOR_H
#define SWIFT_CORE_AIRSPACE_MONITOR_H

#include "core/swiftcoreexport.h"
#include "misc/simulation/settings/modelmatchersettings.h"
#include "misc/simulation/aircraftmodelsetprovider.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/matchinglog.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulationenvironmentprovider.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/network/server.h"
#include "misc/network/ecosystem.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/clientprovider.h"
#include "misc/network/userlist.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/angle.h"
#include "misc/simplecommandparser.h"
#include "misc/identifier.h"

#include <QJsonObject>
#include <QList>
#include <QHash>
#include <QMetaObject>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <QQueue>
#include <functional>

namespace swift::core
{
    namespace fsd
    {
        class CFSDClient;
    }
    class CAirspaceAnalyzer;

    //! Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
    //! Central instance of data for \sa IRemoteAircraftProvider.
    class SWIFT_CORE_EXPORT CAirspaceMonitor :
        public swift::misc::simulation::CRemoteAircraftProvider, // those data will be provided from the class CAirspaceMonitor
        public swift::misc::network::CClientProvider, // those data will be provided from the class CAirspaceMonitor
        public swift::misc::simulation::COwnAircraftAware, // used to obtain in memory information about own aircraft
        public swift::misc::simulation::CSimulationEnvironmentAware, // elevation info etc. from simulator
        public swift::misc::simulation::CAircraftModelSetAware // model set for reverse lookup
    {
        // CRemoteAircraftProvider is QObject
        Q_OBJECT
        Q_INTERFACES(swift::misc::network::IClientProvider)
        Q_INTERFACES(swift::misc::simulation::IRemoteAircraftProvider)

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CAirspaceMonitor(swift::misc::simulation::IOwnAircraftProvider *ownAircraft,
                         swift::misc::simulation::IAircraftModelSetProvider *modelSetProvider,
                         fsd::CFSDClient *fsdClient,
                         QObject *parent);

        //! @{
        //! Members not implenented or fully implenented by CRemoteAircraftProvider
        //! \ingroup remoteaircraftprovider
        virtual QObject *asQObject() override { return this; }
        virtual swift::misc::simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
        virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
        //! @}

        //! Returns the list of users we know about
        swift::misc::network::CUserList getUsers() const;

        //! Returns a list of the users corresponding to the given callsigns
        swift::misc::network::CUserList getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const;

        //! Returns the loaded flight plan for the given callsign
        //! \remarks only use this if a network loaded flight plan is directly needed
        //! \remarks pseudo synchronous, call the async functions and waits for result
        swift::misc::aviation::CFlightPlan loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign);

        //! Try to get flight plan remarks
        //! \remarks returns a value only if the flight plan is already cached or can be obtained from VATSIM reader
        //! \threadsafe
        swift::misc::aviation::CFlightPlanRemarks tryToGetFlightPlanRemarks(const swift::misc::aviation::CCallsign &callsign) const;

        //! Returns the current online ATC stations
        swift::misc::aviation::CAtcStationList getAtcStationsOnline() const { return m_atcStationsOnline; }

        //! Recalculate distance to own aircraft
        swift::misc::aviation::CAtcStationList getAtcStationsOnlineRecalculated();

        //! Returns the closest ATC station operating on the given frequency, if any
        swift::misc::aviation::CAtcStation getAtcStationForComUnit(const swift::misc::aviation::CComSystem &comSystem) const;

        //! Clear the contents
        void clear();

        //! Request to update other clients' data from the network
        void requestAircraftDataUpdates();

        //! Request to update ATC stations' ATIS data from the network
        void requestAtisUpdates();

        //! Analyzer
        CAirspaceAnalyzer *analyzer() const { return m_analyzer; }

        //! \copydoc CAirspaceAnalyzer::setEnabled
        bool enableAnalyzer(bool enable);

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

        //! Re-init all aircrft
        int reInitializeAllAircraft();

        //! Max (FSD) range
        void setMaxRange(const swift::misc::physical_quantities::CLength &range);

        //! Create dummy entries for performance tests
        //! \private for testing purposes
        void testCreateDummyOnlineAtcStations(int number);

        //! Test injected aircraft parts
        //! \private for testing purposes
        void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CAircraftParts &parts, bool incremental);

        //! Matching readiness
        enum MatchingReadinessFlag
        {
            NotReady = 0, //!< no data at all
            ReceivedIcaoCodes = 1 << 0, //!< ICAO codes received
            ReceivedFsInnPacket = 1 << 1, //!< FsInn pcket received
            ReadyForMatchingSent = 1 << 2, //!< Read for matching sending
            RecursiveCall = 1 << 3, //!< recursion
            ReceivedAll = ReceivedIcaoCodes | ReceivedFsInnPacket,
            Verified = 1 << 4, //!< verified already
        };
        Q_DECLARE_FLAGS(MatchingReadiness, MatchingReadinessFlag)

        //! @{
        //! As string
        static const QString &enumFlagToString(MatchingReadinessFlag r);
        static QString enumToString(MatchingReadiness r);
        //! @}

        //! \ingroup swiftdotcommands
        //! <pre>
        //! .fsd range distance        max.range e.g. ".fsd range 100NM"
        //! </pre>
        //! \copydoc swift::core::context::IContextNetwork::parseCommandLine
        bool parseCommandLine(const QString &commandLine, const swift::misc::CIdentifier &originator);

        //! Register help
        static void registerHelp()
        {
            if (swift::misc::CSimpleCommandParser::registered("swift::core::fsd::CFSDClient")) { return; }
            swift::misc::CSimpleCommandParser::registerCommand({ ".fsd range distance", "FSD max. range" });
        }

    signals:
        //! Online ATC stations were changed
        void changedAtcStationsOnline();

        //! ATC station disconnected
        void atcStationDisconnected(const swift::misc::aviation::CAtcStation &station);

        //! Raw data as received from network
        //! \remark used for statistics
        //! \private
        void requestedNewAircraft(const swift::misc::aviation::CCallsign &callsign, const QString &aircraftDesignator, const QString &airlineDesignator, const QString &livery);

        //! Ready for model matching
        void readyForModelMatching(const swift::misc::simulation::CSimulatedAircraft &remoteAircraft);

        //! An ATIS has been received
        void changedAtisReceived(const swift::misc::aviation::CCallsign &callsign);

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

        //! Combined MatchingReadiness/timestamp
        struct Readiness
        {
        private:
            qint64 ts = -1;
            MatchingReadiness readyness = NotReady;

        public:
            //! Add flag and set timestamp if required
            Readiness &addFlag(MatchingReadinessFlag flag)
            {
                if (flag == RecursiveCall) { return *this; }
                if (ts < 0) { ts = QDateTime::currentMSecsSinceEpoch(); }
                readyness.setFlag(flag, true);
                return *this;
            }

            //! Reset timestamp to now
            void resetTimestampToNow() { ts = QDateTime::currentMSecsSinceEpoch(); }

            //! Set the flag
            Readiness &setFlag(MatchingReadinessFlag flag)
            {
                if (flag == RecursiveCall) { return *this; }
                readyness = NotReady;
                this->addFlag(flag);
                return *this;
            }

            //! Received all data?
            bool receivedAll() const { return readyness.testFlag(ReceivedIcaoCodes) && readyness.testFlag(ReceivedFsInnPacket); }

            //! Received ICAO codes?
            bool receivedIcaoCodes() const { return readyness.testFlag(ReceivedIcaoCodes); }

            //! Was matching already sent?
            bool wasMatchingSent() const { return readyness.testFlag(ReadyForMatchingSent); }

            //! Was verifed?
            bool wasVerified() const { return readyness.testFlag(Verified); }

            //! currnt age in ms
            qint64 getAgeMs() const
            {
                if (ts < 0) { return -1; }
                return QDateTime::currentMSecsSinceEpoch() - ts;
            }

            //! As string
            QString toQString() const
            {
                return QStringLiteral("ts: %1 ready: %2").arg(ts).arg(CAirspaceMonitor::enumToString(readyness));
            }
        };

        swift::misc::aviation::CAtcStationList m_atcStationsOnline; //!< online ATC stations
        QHash<swift::misc::aviation::CCallsign, FsInnPacket> m_tempFsInnPackets; //!< unhandled FsInn packets
        QHash<swift::misc::aviation::CCallsign, swift::misc::aviation::CFlightPlan> m_flightPlanCache; //!< flight plan information retrieved from network and cached
        QHash<swift::misc::aviation::CCallsign, Readiness> m_readiness; //!< readiness
        swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TModelMatching> m_matchingSettings { this }; //!< settings
        QQueue<swift::misc::aviation::CCallsign> m_queryAtis; //!< query the ATIS
        QQueue<swift::misc::aviation::CCallsign> m_queryPilot; //!< query the pilot data
        fsd::CFSDClient *m_fsdClient = nullptr; //!< corresponding network interface
        CAirspaceAnalyzer *m_analyzer = nullptr; //!< owned analyzer
        int m_maxDistanceNM = 125; //!< position range / FSD range
        int m_maxDistanceNMHysteresis = qRound(1.1 * m_maxDistanceNM);
        int m_foundInNonMovingAircraft = 0;
        int m_foundInElevationsOnGnd = 0;

        // Processing for queries etc. (fast)
        static constexpr int FastProcessIntervalMs = 50; //!< interval in ms
        QTimer m_fastProcessTimer; //!< process timer for fast updates

        //! Fast processing by timer
        void fastProcessing();

        // Processing for validations etc. (slow)
        static constexpr int SlowProcessIntervalMs = 125 * 1000; //!< interval in ms
        QTimer m_slowProcessTimer; //!< process timer for slow updates

        //! Slow processing by timer
        void slowProcessing();

        // model matching times
        static constexpr qint64 MMCheckAgainMs = 2000;
        static constexpr qint64 MMMaxAgeMs = MMCheckAgainMs * 3;
        static constexpr qint64 MMMaxAgeThresholdMs = MMCheckAgainMs * 10;
        static constexpr qint64 MMVerifyMs = MMCheckAgainMs * 12;

        //! Remove ATC online stations
        void removeAllOnlineAtcStations();

        //! Remove all aircraft in range
        //! \threadsafe
        void removeAllAircraft();

        //! Remove data from caches and logs
        //! \threadsafe
        void removeFromAircraftCachesAndLogs(const swift::misc::aviation::CCallsign &callsign);

        //! Network queries for ATC
        void sendInitialAtcQueries(const swift::misc::aviation::CCallsign &callsign);

        //! Query all online ATC stations
        void queryAllOnlineAtcStations();

        //! Network queries for ATIS
        bool sendNextStaggeredAtisQuery();

        //! Network queries for pilots
        void sendInitialPilotQueries(const swift::misc::aviation::CCallsign &callsign, bool withIcaoQuery, bool withFsInn);

        //! Network queries for pilot
        bool sendNextStaggeredPilotDataQuery();

        //! Connected with network?
        bool isConnected() const;

        //! Connected with server and not shutting down
        bool isConnectedAndNotShuttingDown() const;

        //! Get the currently connected server
        const swift::misc::network::CServer &getConnectedServer() const;

        //! Current ECO system of connected server
        const swift::misc::network::CEcosystem &getCurrentEcosystem() const;

        //! Supports VATSIM data file
        //! \remark depends on currently connected Ecosystem
        bool supportsVatsimDataFile() const;

        //! Distance calculation
        swift::misc::physical_quantities::CLength calculateDistanceToOwnAircraft(const swift::misc::aviation::CAircraftSituation &situation) const;

        //! Angle calculation
        swift::misc::physical_quantities::CAngle calculateBearingToOwnAircraft(const swift::misc::aviation::CAircraftSituation &situation) const;

        //! Store an aircraft situation under consideration of gnd.flags/CG and elevation
        //! \threadsafe
        //! \remark sets gnd.flag from parts if parts are available
        //! \remark uses gnd.elevation if found
        virtual swift::misc::aviation::CAircraftSituation storeAircraftSituation(const swift::misc::aviation::CAircraftSituation &situation, bool allowTestOffset = true) override;

        //! Add or update aircraft
        swift::misc::simulation::CSimulatedAircraft addOrUpdateAircraftInRange(const swift::misc::aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString, swift::misc::simulation::CAircraftModel::ModelType modelType, swift::misc::CStatusMessageList *log);

        //! Add new aircraft, ignored if aircraft already exists
        //! \remark position to own aircraft set, VATSIM data file data considered
        //! \threadsafe
        bool addNewAircraftInRange(const swift::misc::simulation::CSimulatedAircraft &aircraft);

        //! Asynchronously add aircraft
        //! \threadsafe
        //! \sa addNewAircraftInRange
        void asyncReInitializeAllAircraft(const swift::misc::simulation::CSimulatedAircraftList &aircraft, bool readyForModelMatching);

        //! Update online stations by callsign
        int updateOnlineStation(const swift::misc::aviation::CCallsign &callsign, const swift::misc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true, bool sendSignal = true);

        //! Handle max.range
        bool handleMaxRange(const swift::misc::aviation::CAircraftSituation &situation);

        //! Call CAirspaceMonitor::onCustomFSInnPacketReceived with stored packet
        bool recallFsInnPacket(const swift::misc::aviation::CCallsign &callsign);

        //! Send the information if aircraft and(!) client are available
        //! \note it can take some time to obtain all data for model matching, so function recursively calls itself if something is still missing (trial)
        //! \sa reverseLookupModelWithFlightplanData
        void sendReadyForModelMatching(const swift::misc::aviation::CCallsign &callsign, MatchingReadinessFlag rf);

        //! Make sure we got ICAO data
        //! \remark did we get any responses for ICAO data (standard, FsInn)
        void verifyReceivedIcaoData(const swift::misc::aviation::CCallsign &callsign);

        //! Reverse lookup, if available flight plan data are considered
        //! \remark this is where a model is created based on network data
        //! \ingroup reverselookup
        swift::misc::simulation::CAircraftModel reverseLookupModelWithFlightplanData(
            const swift::misc::aviation::CCallsign &callsign, const QString &aircraftIcao,
            const QString &airlineIcao, const QString &liveryString, const QString &modelString,
            swift::misc::simulation::CAircraftModel::ModelType type, swift::misc::CStatusMessageList *log, bool runMatchinScript = true);

        //! Does this look like a co-pilot callsign
        bool isCopilotAircraft(const swift::misc::aviation::CCallsign &callsign) const;

        //! Set matching readiness flag
        Readiness &addMatchingReadinessFlag(const swift::misc::aviation::CCallsign &callsign, MatchingReadinessFlag mrf);

        //! Extrapolates elevation into front (first) element from 2nd and 3rd element
        //! \pre the list must be sorted latest first and containt at least 3 elements
        static bool extrapolateElevation(swift::misc::aviation::CAircraftSituationList &situations, const swift::misc::aviation::CAircraftSituationChange &change);

        //! Extrapolated between the 2 situations for situation
        //! \remark normally used if situationToBeUpdated is not between oldSituation and olderSituation (that would be interpolation)
        //! \return false if there are no two elevations, there is already an elevation, or no extrapolation is possible (too much deviation)
        static bool extrapolateElevation(swift::misc::aviation::CAircraftSituation &situationToBeUpdated, const swift::misc::aviation::CAircraftSituation &oldSituation,
                                         const swift::misc::aviation::CAircraftSituation &olderSituation, const swift::misc::aviation::CAircraftSituationChange &oldChange);

        //! Create aircraft in range, this is the only place where a new aircraft should be added
        void onAircraftUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CTransponder &transponder);

        //! Create ATC station, this is the only place where an online ATC station should be added
        void onAtcPositionUpdate(const swift::misc::aviation::CCallsign &callsign, const swift::misc::physical_quantities::CFrequency &frequency, const swift::misc::geo::CCoordinateGeodetic &position, const swift::misc::physical_quantities::CLength &range);

        //! Receive FSInn packet
        //! \remark This can happen even without a query before
        void onCustomFSInnPacketReceived(const swift::misc::aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &modelString);

        void onRealNameReplyReceived(const swift::misc::aviation::CCallsign &callsign, const QString &realname);
        void onCapabilitiesReplyReceived(const swift::misc::aviation::CCallsign &callsign, swift::misc::network::CClient::Capabilities clientCaps);
        void onServerReplyReceived(const swift::misc::aviation::CCallsign &callsign, const QString &server);
        void onFlightPlanReceived(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CFlightPlan &flightPlan);
        void onAtcControllerDisconnected(const swift::misc::aviation::CCallsign &callsign);
        void onAtisReceived(const swift::misc::aviation::CCallsign &callsign, const swift::misc::aviation::CInformationMessage &atisMessage);
        void onAtisLogoffTimeReceived(const swift::misc::aviation::CCallsign &callsign, const QString &zuluTime);
        void onIcaoCodesReceived(const swift::misc::aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);
        void onPilotDisconnected(const swift::misc::aviation::CCallsign &callsign);
        void onFrequencyReceived(const swift::misc::aviation::CCallsign &callsign, const swift::misc::physical_quantities::CFrequency &frequency);
        void onReceivedVatsimDataFile();
        void onAircraftConfigReceived(const swift::misc::aviation::CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs);
        void onAircraftInterimUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation);
        void onAircraftVisualUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation);
        void onAircraftSimDataUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation, const swift::misc::aviation::CAircraftParts &parts, qint64 currentOffsetMs, const QString &aircraftIcao, const QString &airlineIcao);
        void onConnectionStatusChanged(swift::misc::network::CConnectionStatus oldStatus, swift::misc::network::CConnectionStatus newStatus);
        void onRevBAircraftConfigReceived(const swift::misc::aviation::CCallsign &callsign, const QString &config, qint64 currentOffsetMs);
    };
} // namespace

Q_DECLARE_METATYPE(swift::core::CAirspaceMonitor::MatchingReadinessFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(swift::core::CAirspaceMonitor::MatchingReadiness)

#endif // guard

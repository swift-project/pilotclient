/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AIRSPACE_MONITOR_H
#define BLACKCORE_AIRSPACE_MONITOR_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/simulation/settings/modelmatchersettings.h"
#include "blackmisc/simulation/aircraftmodelsetprovider.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/matchinglog.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/ecosystem.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/clientprovider.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/identifier.h"

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

namespace BlackCore
{
    namespace Fsd { class CFSDClient; }
    class CAirspaceAnalyzer;

    //! Keeps track of other entities in the airspace: aircraft, ATC stations, etc.
    //! Central instance of data for \sa IRemoteAircraftProvider.
    class BLACKCORE_EXPORT CAirspaceMonitor :
        public BlackMisc::Simulation::CRemoteAircraftProvider,     // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Network::CClientProvider,                // those data will be provided from the class CAirspaceMonitor
        public BlackMisc::Simulation::COwnAircraftAware,           // used to obtain in memory information about own aircraft
        public BlackMisc::Simulation::CSimulationEnvironmentAware, // elevation info etc. from simulator
        public BlackMisc::Simulation::CAircraftModelSetAware       // model set for reverse lookup
    {
        // CRemoteAircraftProvider is QObject
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Network::IClientProvider)
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        CAirspaceMonitor(BlackMisc::Simulation::IOwnAircraftProvider      *ownAircraft,
                         BlackMisc::Simulation::IAircraftModelSetProvider *modelSetProvider,
                         Fsd::CFSDClient *fsdClient,
                         QObject         *parent);

        //! Members not implenented or fully implenented by CRemoteAircraftProvider
        //! \ingroup remoteaircraftprovider
        //! @{
        virtual QObject *asQObject() override { return this; }
        virtual BlackMisc::Simulation::CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates) override;
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

        //! Recalculate distance to own aircraft
        BlackMisc::Aviation::CAtcStationList getAtcStationsOnlineRecalculated();

        //! Returns the current booked ATC stations (consolidated with online stations)
        BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const { return m_atcStationsBooked; }

        //! Recalculate distance to own aircraft
        BlackMisc::Aviation::CAtcStationList getAtcStationsBookedRecalculated();

        //! Returns the closest ATC station operating on the given frequency, if any
        BlackMisc::Aviation::CAtcStation getAtcStationForComUnit(const BlackMisc::Aviation::CComSystem &comSystem) const;

        //! Clear the contents
        void clear();

        //! Request to update other clients' data from the network
        void requestAircraftDataUpdates();

        //! Request to update ATC stations' ATIS data from the network
        void requestAtisUpdates();

        //! Request updates of bookings
        void requestAtcBookingsUpdate();

        //! Analyzer
        CAirspaceAnalyzer *analyzer() const { return m_analyzer; }

        //! \copydoc CAirspaceAnalyzer::setEnabled
        bool enableAnalyzer(bool enable);

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

        //! Re-init all aircrft
        int reInitializeAllAircraft();

        //! Max (FSD) range
        void setMaxRange(const BlackMisc::PhysicalQuantities::CLength &range);

        //! Create dummy entries for performance tests
        //! \private for testing purposes
        void testCreateDummyOnlineAtcStations(int number);

        //! Test injected aircraft parts
        //! \private for testing purposes
        void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental);

        //! Matching readiness
        enum MatchingReadinessFlag
        {
            NotReady             = 0,       //!< no data at all
            ReceivedIcaoCodes    = 1 << 0,  //!< ICAO codes received
            ReceivedFsInnPacket  = 1 << 1,  //!< FsInn pcket received
            ReadyForMatchingSent = 1 << 2,  //!< Read for matching sending
            RecursiveCall        = 1 << 3,  //!< recursion
            ReceivedAll          = ReceivedIcaoCodes | ReceivedFsInnPacket,
            Verified             = 1 << 4,  //!< verified already
        };
        Q_DECLARE_FLAGS(MatchingReadiness, MatchingReadinessFlag)

        //! As string
        //! @{
        static const QString &enumFlagToString(MatchingReadinessFlag r);
        static QString enumToString(MatchingReadiness r);
        //! @}

        //! \addtogroup swiftdotcommands
        //! @{
        //! <pre>
        //! .fsd range distance        max.range e.g. ".fsd range 100NM"
        //! </pre>
        //! @}
        //! \copydoc BlackCore::Context::IContextNetwork::parseCommandLine
        bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator);

        //! Register help
        static void registerHelp()
        {
            if (BlackMisc::CSimpleCommandParser::registered("BlackCore::Fsd::CFSDClient")) { return; }
            BlackMisc::CSimpleCommandParser::registerCommand({".fsd range distance", "FSD max. range"});
        }

    signals:
        //! Online ATC stations were changed
        void changedAtcStationsOnline();

        //! Booked ATC stations were changed
        void changedAtcStationsBooked();

        //! Connection status of an ATC station was changed
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &station, bool isConnected);

        //! Raw data as received from network
        //! \remark used for statistics
        //! \private
        void requestedNewAircraft(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftDesignator, const QString &airlineDesignator, const QString &livery);

        //! Ready for model matching
        void readyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! An ATIS has been received
        void changedAtisReceived(const BlackMisc::Aviation::CCallsign &callsign);

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

        BlackMisc::Aviation::CAtcStationList m_atcStationsOnline; //!< online ATC stations
        BlackMisc::Aviation::CAtcStationList m_atcStationsBooked; //!< booked ATC stations
        QHash<BlackMisc::Aviation::CCallsign, FsInnPacket>                      m_tempFsInnPackets; //!< unhandled FsInn packets
        QHash<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CFlightPlan> m_flightPlanCache;  //!< flight plan information retrieved from network and cached
        QHash<BlackMisc::Aviation::CCallsign, Readiness>                        m_readiness;        //!< readiness
        BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TModelMatching> m_matchingSettings { this }; //!< settings
        QQueue<BlackMisc::Aviation::CCallsign> m_queryAtis;  //!< query the ATIS
        QQueue<BlackMisc::Aviation::CCallsign> m_queryPilot; //!< query the pilot data
        Fsd::CFSDClient   *m_fsdClient = nullptr;            //!< corresponding network interface
        CAirspaceAnalyzer *m_analyzer  = nullptr;            //!< owned analyzer
        bool m_bookingsRequested       = false;              //!< bookings have been requested, it can happen we receive an BlackCore::Vatsim::CVatsimBookingReader::atcBookingsReadUnchanged signal
        int m_maxDistanceNM            = 125;                //!< position range / FSD range
        int m_maxDistanceNMHysteresis  = qRound(1.1 * m_maxDistanceNM);
        int m_foundInNonMovingAircraft = 0;
        int m_foundInElevationsOnGnd   = 0;

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
        static constexpr qint64 MMCheckAgainMs      = 2000;
        static constexpr qint64 MMMaxAgeMs          = MMCheckAgainMs * 3;
        static constexpr qint64 MMMaxAgeThresholdMs = MMCheckAgainMs * 10;
        static constexpr qint64 MMVerifyMs          = MMCheckAgainMs * 12;

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

        //! Query all online ATC stations
        void queryAllOnlineAtcStations();

        //! Network queries for ATIS
        bool sendNextStaggeredAtisQuery();

        //! Network queries for pilots
        void sendInitialPilotQueries(const BlackMisc::Aviation::CCallsign &callsign, bool withIcaoQuery, bool withFsInn);

        //! Network queries for pilot
        bool sendNextStaggeredPilotDataQuery();

        //! Connected with network?
        bool isConnected() const;

        //! Connected with server and not shutting down
        bool isConnectedAndNotShuttingDown() const;

        //! Get the currently connected server
        const BlackMisc::Network::CServer &getConnectedServer() const;

        //! Current ECO system of connected server
        const BlackMisc::Network::CEcosystem &getCurrentEcosystem() const;

        //! Supports VATSIM data file
        //! \remark depends on currently connected Ecosystem
        bool supportsVatsimDataFile() const;

        //! Distance calculation
        BlackMisc::PhysicalQuantities::CLength calculateDistanceToOwnAircraft(const BlackMisc::Aviation::CAircraftSituation &situation) const;

        //! Angle calculation
        BlackMisc::PhysicalQuantities::CAngle calculateBearingToOwnAircraft(const BlackMisc::Aviation::CAircraftSituation &situation) const;

        //! Store an aircraft situation under consideration of gnd.flags/CG and elevation
        //! \threadsafe
        //! \remark sets gnd.flag from parts if parts are available
        //! \remark uses gnd.elevation if found
        virtual BlackMisc::Aviation::CAircraftSituation storeAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation, bool allowTestOffset = true) override;

        //! Add or update aircraft
        BlackMisc::Simulation::CSimulatedAircraft addOrUpdateAircraftInRange(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcao, const QString &airlineIcao, const QString &livery, const QString &modelString, BlackMisc::Simulation::CAircraftModel::ModelType modelType, BlackMisc::CStatusMessageList *log);

        //! Add new aircraft, ignored if aircraft already exists
        //! \remark position to own aircraft set, VATSIM data file data considered
        //! \threadsafe
        bool addNewAircraftInRange(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

        //! Asynchronously add aircraft
        //! \threadsafe
        //! \sa addNewAircraftInRange
        void asyncReInitializeAllAircraft(const BlackMisc::Simulation::CSimulatedAircraftList &aircraft, bool readyForModelMatching);

        //! Update online stations by callsign
        int updateOnlineStation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true, bool sendSignal = true);

        //! Update booked station by callsign
        int updateBookedStation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &vm, bool skipEqualValues = true, bool sendSignal = true);

        //! Handle max.range
        bool handleMaxRange(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Call CAirspaceMonitor::onCustomFSInnPacketReceived with stored packet
        bool recallFsInnPacket(const BlackMisc::Aviation::CCallsign &callsign);

        //! Send the information if aircraft and(!) client are available
        //! \note it can take some time to obtain all data for model matching, so function recursively calls itself if something is still missing (trial)
        //! \sa reverseLookupModelWithFlightplanData
        void sendReadyForModelMatching(const BlackMisc::Aviation::CCallsign &callsign, MatchingReadinessFlag rf);

        //! Make sure we got ICAO data
        //! \remark did we get any responses for ICAO data (standard, FsInn)
        void verifyReceivedIcaoData(const BlackMisc::Aviation::CCallsign &callsign);

        //! Reverse lookup, if available flight plan data are considered
        //! \remark this is where a model is created based on network data
        //! \ingroup reverselookup
        BlackMisc::Simulation::CAircraftModel reverseLookupModelWithFlightplanData(
            const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcao,
            const QString &airlineIcao, const QString &liveryString, const QString &modelString,
            BlackMisc::Simulation::CAircraftModel::ModelType type, BlackMisc::CStatusMessageList *log, bool runMatchinScript = true);

        //! Does this look like a co-pilot callsign
        bool isCopilotAircraft(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Set matching readiness flag
        Readiness &addMatchingReadinessFlag(const BlackMisc::Aviation::CCallsign &callsign, MatchingReadinessFlag mrf);

        //! Extrapolates elevation into front (first) element from 2nd and 3rd element
        //! \pre the list must be sorted latest first and containt at least 3 elements
        static bool extrapolateElevation(BlackMisc::Aviation::CAircraftSituationList &situations, const BlackMisc::Aviation::CAircraftSituationChange &change);

        //! Extrapolated between the 2 situations for situation
        //! \remark normally used if situationToBeUpdated is not between oldSituation and olderSituation (that would be interpolation)
        //! \return false if there are no two elevations, there is already an elevation, or no extrapolation is possible (too much deviation)
        static bool extrapolateElevation(BlackMisc::Aviation::CAircraftSituation &situationToBeUpdated, const BlackMisc::Aviation::CAircraftSituation &oldSituation,
            const BlackMisc::Aviation::CAircraftSituation &olderSituation, const BlackMisc::Aviation::CAircraftSituationChange &oldChange);

        //! Create aircraft in range, this is the only place where a new aircraft should be added
        void onAircraftUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);

        //! Create ATC station, this is the only place where an online ATC station should be added
        void onAtcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency, const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::PhysicalQuantities::CLength &range);

        //! Receive FSInn packet
        //! \remark This can happen even without a query before
        void onCustomFSInnPacketReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &modelString);

        void onRealNameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);
        void onCapabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, BlackMisc::Network::CClient::Capabilities clientCaps);
        void onServerReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &server);
        void onFlightPlanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
        void onAtcControllerDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void onAtisReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atisMessage);
        void onAtisLogoffTimeReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void onIcaoCodesReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &aircraftIcaoDesignator, const QString &airlineIcaoDesignator, const QString &livery);
        void onPilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void onFrequencyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
        void onReceivedAtcBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);
        void onReadUnchangedAtcBookings();
        void onReceivedVatsimDataFile();
        void onAircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QJsonObject &jsonObject, qint64 currentOffsetMs);
        void onAircraftInterimUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
        void onAircraftVisualUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
        void onConnectionStatusChanged(BlackMisc::Network::CConnectionStatus oldStatus, BlackMisc::Network::CConnectionStatus newStatus);
        void onRevBAircraftConfigReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &config, qint64 currentOffsetMs);

    };
} // namespace

Q_DECLARE_METATYPE(BlackCore::CAirspaceMonitor::MatchingReadinessFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CAirspaceMonitor::MatchingReadiness)

#endif // guard

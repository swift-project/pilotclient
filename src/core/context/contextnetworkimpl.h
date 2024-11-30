// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_CONTEXT_CONTEXTNETWORK_IMPL_H
#define SWIFT_CORE_CONTEXT_CONTEXTNETWORK_IMPL_H

#include <functional>

#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QtGlobal>

#include "core/context/contextnetwork.h"
#include "core/corefacadeconfig.h"
#include "core/swiftcoreexport.h"
#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/airporticaocode.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/digestsignal.h"
#include "misc/identifier.h"
#include "misc/network/clientlist.h"
#include "misc/network/clientprovider.h"
#include "misc/network/server.h"
#include "misc/network/serverlist.h"
#include "misc/network/textmessagelist.h"
#include "misc/network/user.h"
#include "misc/network/userlist.h"
#include "misc/pq/length.h"
#include "misc/simplecommandparser.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/simulation/simulationenvironmentprovider.h"
#include "misc/statusmessage.h"
#include "misc/weather/metar.h"
#include "misc/weather/metarlist.h"

// clazy:excludeall=const-signal-or-slot

class QTimer;

namespace swift::misc
{
    class CDBusServer;
    namespace aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    } // namespace aviation
} // namespace swift::misc

namespace swift::core
{
    class CAirspaceMonitor;
    class CCoreFacade;

    namespace fsd
    {
        class CFSDClient;
    }

    namespace context
    {
        //! Network context implementation
        class SWIFT_CORE_EXPORT CContextNetwork :
            public IContextNetwork,
            public swift::misc::simulation::IRemoteAircraftProvider,
            public swift::misc::network::IClientProvider
        {
            Q_OBJECT
            Q_INTERFACES(swift::misc::simulation::IRemoteAircraftProvider)
            Q_INTERFACES(swift::misc::network::IClientProvider)
            Q_CLASSINFO("D-Bus Interface", SWIFT_CORE_CONTEXTNETWORK_INTERFACENAME)

            friend class IContextNetwork;
            friend class swift::core::CCoreFacade;

        public:
            //! Destructor
            virtual ~CContextNetwork() override;

            // remoteaircraftprovider
            virtual swift::misc::aviation::CAircraftSituationList
            remoteAircraftSituations(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual swift::misc::aviation::CAircraftSituation
            remoteAircraftSituation(const swift::misc::aviation::CCallsign &callsign, int index) const override;
            virtual swift::misc::MillisecondsMinMaxMean remoteAircraftSituationsTimestampDifferenceMinMaxMean(
                const swift::misc::aviation::CCallsign &callsign) const override;
            virtual swift::misc::aviation::CAircraftSituationList latestRemoteAircraftSituations() const override;
            virtual swift::misc::aviation::CAircraftSituationList latestOnGroundProviderElevations() const override;
            virtual int remoteAircraftSituationsCount(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual swift::misc::aviation::CAircraftPartsList
            remoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual int remoteAircraftPartsCount(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual swift::misc::aviation::CCallsignSet remoteAircraftSupportingParts() const override;
            virtual swift::misc::aviation::CAircraftSituationChangeList
            remoteAircraftSituationChanges(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual int
            remoteAircraftSituationChangesCount(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual bool updateAircraftRendered(const swift::misc::aviation::CCallsign &callsign,
                                                bool rendered) override;
            virtual int updateMultipleAircraftRendered(const swift::misc::aviation::CCallsignSet &callsigns,
                                                       bool rendered) override;
            virtual int updateMultipleAircraftEnabled(const swift::misc::aviation::CCallsignSet &callsigns,
                                                      bool enabled) override;
            virtual int updateAircraftGroundElevation(const swift::misc::aviation::CCallsign &callsign,
                                                      const swift::misc::geo::CElevationPlane &elevation,
                                                      swift::misc::aviation::CAircraftSituation::GndElevationInfo info,
                                                      bool *setForOnGroundPosition) override;
            virtual void updateMarkAllAsNotRendered() override;
            virtual swift::misc::physical_quantities::CLength
            getCGFromDB(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual swift::misc::physical_quantities::CLength getCGFromDB(const QString &modelString) const override;
            virtual void rememberCGFromDB(const swift::misc::physical_quantities::CLength &cgFromDB,
                                          const swift::misc::aviation::CCallsign &callsign) override;
            virtual void rememberCGFromDB(const swift::misc::physical_quantities::CLength &cgFromDB,
                                          const QString &modelString) override;
            virtual swift::misc::simulation::CAirspaceAircraftSnapshot
            getLatestAirspaceAircraftSnapshot() const override;
            virtual swift::misc::geo::CElevationPlane
            averageElevationOfNonMovingAircraft(const swift::misc::aviation::CAircraftSituation &reference,
                                                const swift::misc::physical_quantities::CLength &range,
                                                int minValues = 1, int sufficientValues = 2) const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::connectRemoteAircraftProviderSignals
            virtual QList<QMetaObject::Connection> connectRemoteAircraftProviderSignals(
                QObject *receiver,
                std::function<void(const swift::misc::aviation::CAircraftSituation &)> addedSituationSlot,
                std::function<void(const swift::misc::aviation::CCallsign &,
                                   const swift::misc::aviation::CAircraftParts &)>
                    addedPartsSlot,
                std::function<void(const swift::misc::aviation::CCallsign &)> removedAircraftSlot,
                std::function<void(const swift::misc::simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot)
                override;

            // clientprovider
            virtual void setClients(const swift::misc::network::CClientList &clients) override;
            virtual void clearClients() override;
            virtual swift::misc::network::CClient
            getClientOrDefaultForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual bool hasClientInfo(const swift::misc::aviation::CCallsign &callsign) const override;
            virtual bool addNewClient(const swift::misc::network::CClient &client) override;
            virtual int updateOrAddClient(const swift::misc::aviation::CCallsign &callsign,
                                          const swift::misc::CPropertyIndexVariantMap &vm,
                                          bool skipEqualValues = true) override;
            virtual int removeClient(const swift::misc::aviation::CCallsign &callsign) override;
            virtual bool
            autoAdjustCientGndCapability(const swift::misc::aviation::CAircraftSituation &situation) override;
            virtual bool addClientGndCapability(const swift::misc::aviation::CCallsign &callsign) override;
            virtual bool setClientGndCapability(const swift::misc::aviation::CCallsign &callsign,
                                                bool supportGndFlag) override;
            virtual void markAsSwiftClient(const swift::misc::aviation::CCallsign &callsign) override;

            //! \copydoc IContextNetwork::connectRawFsdMessageSignal
            virtual QMetaObject::Connection
            connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot) override;

            //! Gracefully shut down, e.g. for thread safety
            void gracefulShutdown();

            //! Network library
            //! \protected
            //! \remarks normally only for core facade internal usage
            //! \remarks public so values can be logged/monitored
            fsd::CFSDClient *fsdClient() const { return m_fsdClient; }

            //! Airspace
            //! \protected
            //! \remarks normally only for core facade internal usage
            //! \remarks public so values can be logged/monitored
            CAirspaceMonitor *airspace() const { return m_airspace; }

        public slots:
            // remoteaircraftprovider

            // emit signal when changed
            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const swift::misc::aviation::CCallsign &callsign,
                                               bool enabledForRendering) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateAircraftModel
            virtual bool updateAircraftModel(const swift::misc::aviation::CCallsign &callsign,
                                             const swift::misc::simulation::CAircraftModel &model,
                                             const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateAircraftNetworkModel
            virtual bool updateAircraftNetworkModel(const swift::misc::aviation::CCallsign &callsign,
                                                    const swift::misc::simulation::CAircraftModel &model,
                                                    const swift::misc::CIdentifier &originator) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const swift::misc::aviation::CCallsign &callsign,
                                                   bool enableFastPositonUpdates) override;

            //! \copydoc swift::core::context::IContextNetwork::updateAircraftSupportingGndFLag
            virtual bool updateAircraftSupportingGndFLag(const swift::misc::aviation::CCallsign &callsign,
                                                         bool supportGndFlag) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::enableReverseLookupMessages
            virtual void enableReverseLookupMessages(swift::misc::simulation::ReverseLookupLogging enable) override;

            // plain vanilla passing to airspace monitor
            //! \copydoc swift::core::context::IContextNetwork::setAircraftEnabledFlag
            virtual bool setAircraftEnabledFlag(const swift::misc::aviation::CCallsign &callsign,
                                                bool enabledForRendering) override;

            //! \copydoc swift::core::context::IContextNetwork::reInitializeAllAircraft
            virtual int reInitializeAllAircraft() override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateCG
            virtual bool updateCG(const swift::misc::aviation::CCallsign &callsign,
                                  const swift::misc::physical_quantities::CLength &cg) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateCGForModel
            virtual swift::misc::aviation::CCallsignSet
            updateCGForModel(const QString &modelString, const swift::misc::physical_quantities::CLength &cg) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::updateCGAndModelString
            virtual bool updateCGAndModelString(const swift::misc::aviation::CCallsign &callsign,
                                                const swift::misc::physical_quantities::CLength &cg,
                                                const QString &modelString) override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRange
            virtual swift::misc::simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCallsigns
            virtual swift::misc::aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftInRange
            virtual bool isAircraftInRange(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::isVtolAircraft
            virtual bool isVtolAircraft(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftInRangeForCallsign
            virtual swift::misc::simulation::CSimulatedAircraft
            getAircraftInRangeForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::getAircraftInRangeModelForCallsign
            virtual swift::misc::simulation::CAircraftModel
            getAircraftInRangeModelForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getReverseLookupMessages
            virtual swift::misc::CStatusMessageList
            getReverseLookupMessages(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::isReverseLookupMessagesEnabled
            virtual swift::misc::simulation::ReverseLookupLogging isReverseLookupMessagesEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::getAircraftPartsHistory
            virtual swift::misc::CStatusMessageList
            getAircraftPartsHistory(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getRemoteAircraftParts
            virtual swift::misc::aviation::CAircraftPartsList
            getRemoteAircraftParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::getRemoteAircraftSupportingPartsCount
            virtual int getRemoteAircraftSupportingPartsCount() const override;

            //! \copydoc swift::core::context::IContextNetwork::isAircraftPartsHistoryEnabled
            virtual bool isAircraftPartsHistoryEnabled() const override;

            //! \copydoc swift::core::context::IContextNetwork::isRemoteAircraftSupportingParts
            virtual bool
            isRemoteAircraftSupportingParts(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::enableAircraftPartsHistory
            virtual void enableAircraftPartsHistory(bool enabled) override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::aircraftSituationsAdded
            virtual int aircraftSituationsAdded() const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::aircraftPartsAdded
            virtual int aircraftPartsAdded() const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::situationsLastModified
            virtual qint64 situationsLastModified(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::misc::simulation::IRemoteAircraftProvider::partsLastModified
            virtual qint64 partsLastModified(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkStatistics
            virtual QString getNetworkStatistics(bool reset, const QString &separator) override;

            //! \copydoc swift::core::context::IContextNetwork::setNetworkStatisticsEnable
            virtual bool setNetworkStatisticsEnable(bool enabled) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAltitudeOffset
            virtual bool testAddAltitudeOffset(const swift::misc::aviation::CCallsign &callsign,
                                               const swift::misc::physical_quantities::CLength &offset =
                                                   swift::misc::physical_quantities::CLength::null()) override;

            //! \copydoc swift::core::context::IContextNetwork::getNetworkPresetValues
            virtual QStringList getNetworkPresetValues() const override;

            //! \copydoc swift::core::context::IContextNetwork::getPartnerCallsign
            virtual swift::misc::aviation::CCallsign getPartnerCallsign() const override;

            //! \copydoc swift::misc::network::IClientProvider::getClients
            virtual swift::misc::network::CClientList getClients() const override;

            //! \copydoc swift::misc::network::IClientProvider::getClientsForCallsigns
            virtual swift::misc::network::CClientList
            getClientsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::misc::network::IClientProvider::setOtherClient
            virtual bool setOtherClient(const swift::misc::network::CClient &client) override;

            //! In transition state, e.g. connecting, disconnecting.
            //! \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
            //! \remarks Intentionally only running locally, not in interface
            bool isPendingConnection() const;

            // --------------------- IContextNetwork implementations ---------------------
            //! \ingroup swiftdotcommands
            //! <pre>
            //! .m .msg .chat callsign|freq. message   message text
            //! .altos .altoffset                      altitude offset for testing
            //! .addtimeos .addtimeoffset              additional offset time for testing
            //! .reinit .reinitialize                  re-initialize all aircraft
            //! .watchdog on|off                       watchdog on/off
            //! .wallop message                        send wallop message
            //! </pre>
            //! \copydoc IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine,
                                          const swift::misc::CIdentifier &originator) override;

            //! Register help
            static void registerHelp()
            {
                if (swift::misc::CSimpleCommandParser::registered("swift::core::CContextNetwork")) { return; }
                swift::misc::CSimpleCommandParser::registerCommand({ ".m", "alias: .msg, .chat" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".m message text", "send text message" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".m callsign message text", "send text message" });
                swift::misc::CSimpleCommandParser::registerCommand(
                    { ".altos callsign|? offsetvalue", "set altitude offset value (testing), ?=all" });
                swift::misc::CSimpleCommandParser::registerCommand(
                    { ".addtimeos offsetvalue", "add (delta) time offset (testing), e.g. 50ms" });
                swift::misc::CSimpleCommandParser::registerCommand(
                    { ".watchdog on|off", "enable/disable network watchdog (testing)" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".wallop message", "send a wallop message" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".reinit", "re-initialize all aircraft" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".enable", "alias: .unignore" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".enable callsign", "enable/unignore callsign" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".disable", "alias: .ignore" });
                swift::misc::CSimpleCommandParser::registerCommand({ ".disable callsign", "disable/ignore callsign" });
            }

            //! \copydoc swift::core::context::IContextNetwork::getAtcStationsOnline
            virtual swift::misc::aviation::CAtcStationList
            getAtcStationsOnline(bool recalculateDistance) const override;

            //! \copydoc swift::core::context::IContextNetwork::getClosestAtcStationsOnline
            virtual swift::misc::aviation::CAtcStationList getClosestAtcStationsOnline(int number) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationForCallsign
            virtual swift::misc::aviation::CAtcStation
            getOnlineStationForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getOnlineStationsForFrequency
            virtual swift::misc::aviation::CAtcStationList
            getOnlineStationsForFrequency(const swift::misc::physical_quantities::CFrequency &frequency) const override;

            //! \copydoc swift::core::context::IContextNetwork::isOnlineStation
            virtual bool isOnlineStation(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::connectToNetwork
            virtual swift::misc::CStatusMessage
            connectToNetwork(const swift::misc::network::CServer &server, const QString &extraLiveryString,
                             bool sendLivery, const QString &extraModelString, bool sendModelString,
                             const swift::misc::aviation::CCallsign &partnerCallsign,
                             swift::misc::network::CLoginMode mode) override;

            //! \copydoc swift::core::context::IContextNetwork::getConnectedServer
            virtual swift::misc::network::CServer getConnectedServer() const override;

            //! \copydoc swift::core::context::IContextNetwork::getLoginMode
            virtual swift::misc::network::CLoginMode getLoginMode() const override;

            //! \copydoc swift::core::context::IContextNetwork::disconnectFromNetwork
            virtual swift::misc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc swift::core::context::IContextNetwork::isConnected
            virtual bool isConnected() const override;

            //! \copydoc swift::core::context::IContextNetwork::sendTextMessages
            virtual void sendTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::sendFlightPlan
            virtual void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan) override;

            //! \copydoc swift::core::context::IContextNetwork::loadFlightPlanFromNetwork
            virtual swift::misc::aviation::CFlightPlan
            loadFlightPlanFromNetwork(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getMetarForAirport
            swift::misc::weather::CMetar
            getMetarForAirport(const swift::misc::aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsers
            virtual swift::misc::network::CUserList getUsers() const override;

            //! \copydoc swift::core::context::IContextNetwork::getUsersForCallsigns
            virtual swift::misc::network::CUserList
            getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const override;

            //! \copydoc swift::core::context::IContextNetwork::getUserForCallsign
            virtual swift::misc::network::CUser
            getUserForCallsign(const swift::misc::aviation::CCallsign &callsign) const override;

            //! \copydoc swift::core::context::IContextNetwork::getVatsimFsdServers
            virtual swift::misc::network::CServerList getVatsimFsdServers() const override;

            //! \copydoc swift::core::context::IContextNetwork::requestAircraftDataUpdates
            virtual void requestAircraftDataUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::requestAtisUpdates
            virtual void requestAtisUpdates() override;

            //! \copydoc swift::core::context::IContextNetwork::setFastPositionEnabledCallsigns
            virtual void setFastPositionEnabledCallsigns(swift::misc::aviation::CCallsignSet &callsigns) override;

            //! \copydoc swift::core::context::IContextNetwork::getFastPositionEnabledCallsigns
            virtual swift::misc::aviation::CCallsignSet getFastPositionEnabledCallsigns() const override;

            //! \copydoc swift::core::context::IContextNetwork::getLibraryInfo
            virtual QString getLibraryInfo(bool detailed) const override;

            //! \copydoc swift::core::context::IContextNetwork::testCreateDummyOnlineAtcStations
            virtual void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc swift::core::context::IContextNetwork::testAddAircraftParts
            virtual void testAddAircraftParts(const swift::misc::aviation::CCallsign &callsign,
                                              const swift::misc::aviation::CAircraftParts &parts,
                                              bool incremental) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedAtisMessage
            virtual void testReceivedAtisMessage(const swift::misc::aviation::CCallsign &callsign,
                                                 const swift::misc::aviation::CInformationMessage &msg) override;

            //! \copydoc swift::core::context::IContextNetwork::testReceivedTextMessages
            virtual void testReceivedTextMessages(const swift::misc::network::CTextMessageList &textMessages) override;

            //! \copydoc swift::core::context::IContextNetwork::testRequestAircraftConfig
            virtual void testRequestAircraftConfig(const swift::misc::aviation::CCallsign &callsign) override;

        protected:
            //! Constructor, with link to runtime
            CContextNetwork(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextNetwork *registerWithDBus(swift::misc::CDBusServer *server);

            //! Set the provider
            void setSimulationEnvironmentProvider(swift::misc::simulation::ISimulationEnvironmentProvider *provider);

        private:
            swift::misc::network::CLoginMode m_currentMode = swift::misc::network::CLoginMode::Pilot; //!< current modeM
            CAirspaceMonitor *m_airspace = nullptr;
            fsd::CFSDClient *m_fsdClient = nullptr;
            QTimer *m_requestAircraftDataTimer =
                nullptr; //!< general updates such as frequencies, see requestAircraftDataUpdates()
            QTimer *m_requestAtisTimer = nullptr; //!< general updates such as ATIS
            QTimer *m_staggeredMatchingTimer = nullptr; //!< staggered update
            int m_simulatorConnected = 0; //!< how often a simulator has been connected
            swift::misc::simulation::CSimulatorInfo m_lastConnectedSim; //!< last connected sim.

            // Digest signals, only sending after some time
            misc::CDigestSignal m_dsAtcStationsOnlineChanged { this, &IContextNetwork::changedAtcStationsOnline,
                                                               &IContextNetwork::changedAtcStationsOnlineDigest,
                                                               std::chrono::milliseconds(1000), 4 };
            misc::CDigestSignal m_dsAircraftsInRangeChanged { this, &IContextNetwork::changedAircraftInRange,
                                                              &IContextNetwork::changedAircraftInRangeDigest,
                                                              std::chrono::milliseconds(1000), 4 };

            QQueue<swift::misc::simulation::CSimulatedAircraft> m_readyForModelMatching; //!< ready for matching

            //! Own aircraft from \sa CContextOwnAircraft
            swift::misc::simulation::CSimulatedAircraft ownAircraft() const;

            //! Check if a callsign is a valid partner callsign
            bool isValidPartnerCallsign(const swift::misc::aviation::CCallsign &ownCallsign,
                                        const swift::misc::aviation::CCallsign &partnerCallsign);

            //! Update METAR collection
            void updateMetars(const swift::misc::weather::CMetarList &metars);

            //! Check when a message was received, if the message has to be relayed, is a supervisor message ...
            void onTextMessagesReceived(const swift::misc::network::CTextMessageList &messages);

            //! Text message has been sent
            void onTextMessageSent(const swift::misc::network::CTextMessage &message);

            //! An ATIS has been received
            void onChangedAtisReceived(const swift::misc::aviation::CCallsign &callsign);

            //! Connection status changed
            void onFsdConnectionStatusChanged(const swift::misc::network::CConnectionStatus &from,
                                              const swift::misc::network::CConnectionStatus &to);

            //! Ready for matching
            void onReadyForModelMatching(const swift::misc::simulation::CSimulatedAircraft &aircraft);

            //! Emit ready for matching
            void emitReadyForMatching();

            //! Relay to partner callsign
            void createRelayMessageToPartnerCallsign(const swift::misc::network::CTextMessage &textMessage,
                                                     const swift::misc::aviation::CCallsign &partnerCallsign,
                                                     swift::misc::network::CTextMessageList &relayedMessages);

            //! Render restrictions have been changed, used with analyzer
            //! \sa CAirspaceAnalyzer
            //! \ingroup crosscontextfunction
            void xCtxSimulatorRenderRestrictionsChanged(
                bool restricted, bool enabled, int maxAircraft,
                const swift::misc::physical_quantities::CLength &maxRenderedDistance);

            //! Status of simulator changed
            //! \ingroup crosscontextfunction
            void xCtxSimulatorStatusChanged(int status);

            //! Can FSD be used?
            bool canUseFsd() const;

            //! Can the airspace monitor be used be used?
            bool canUseAirspaceMonitor() const;
        };
    } // namespace context
} // namespace swift::core

#endif // guard

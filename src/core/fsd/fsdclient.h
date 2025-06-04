// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_FSD_CLIENT_H
#define SWIFT_CORE_FSD_CLIENT_H

#include "core/fsd/enums.h"
#include "core/fsd/messagebase.h"
#include "core/swiftcoreexport.h"
#include "core/vatsim/vatsimsettings.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/flightplan.h"
#include "misc/aviation/informationmessage.h"
#include "misc/digestsignal.h"
#include "misc/network/clientprovider.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/ecosystemprovider.h"
#include "misc/network/loginmode.h"
#include "misc/network/rawfsdmessage.h"
#include "misc/network/server.h"
#include "misc/network/textmessagelist.h"
#include "misc/simulation/ownaircraftprovider.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulationenvironmentprovider.h"
#include "misc/tokenbucket.h"
#include "misc/worker.h"

#ifdef SWIFT_VATSIM_SUPPORT
#    include "vatsim/vatsimauth.h"
#endif

#include <atomic>

#include <QCommandLineOption>
#include <QHash>
#include <QObject>
#include <QQueue>
#include <QReadWriteLock>
#include <QString>
#include <QTcpSocket>
#include <QTimer>
#include <QtGlobal>

//! @{
//! Protocol version
constexpr int PROTOCOL_REVISION_CLASSIC = 9;
constexpr int PROTOCOL_REVISION_VATSIM_ATC = 10;
constexpr int PROTOCOL_REVISION_VATSIM_AUTH = 100;
constexpr int PROTOCOL_REVISION_VATSIM_VELOCITY = 101;
//! @}

class QNetworkReply;

namespace SwiftFsdTest
{
    class CTestFSDClient;
}
namespace swift::core::fsd
{
    //! Message groups
    enum class TextMessageGroups
    {
        AllClients,
        AllAtcClients,
        AllPilotClients,
        AllSups
    };

    //! FSD client
    //! Todo: Send (interim) data updates automatically
    //! Todo Check ':' in FSD messages. Disconnect if there is a wrong one
    class SWIFT_CORE_EXPORT CFSDClient :
        public swift::misc::CContinuousWorker,
        public swift::misc::network::IEcosystemProvider, // provide info about used ecosystem
        public swift::misc::network::CClientAware, // network can set client information
        public swift::misc::simulation::COwnAircraftAware, // network vatlib consumes own aircraft data and sets
                                                           // ICAO/callsign data
        public swift::misc::simulation::CRemoteAircraftAware, // check if we really need to process network packets
                                                              // (e.g. parts)
        public swift::misc::simulation::CSimulationEnvironmentAware // allows to consume ground elevations
    {
        Q_OBJECT
        Q_INTERFACES(swift::misc::network::IEcosystemProvider)

    public:
        //! Categories
        static const QStringList &getLogCategories();

        //! Ctor
        CFSDClient(swift::misc::network::IClientProvider *clientProvider,
                   swift::misc::simulation::IOwnAircraftProvider *ownAircraftProvider,
                   swift::misc::simulation::IRemoteAircraftProvider *remoteAircraftProvider, QObject *owner = nullptr);

        //! @{
        //! Preset functions
        //! \remark Necessary functions to setup client. Set them all!
        //! \threadsafe
        void setClientName(const QString &clientName)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_clientName = clientName;
        }
        void setHostApplication(const QString &hostApplication)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_hostApplication = hostApplication;
        }
        void setVersion(int major, int minor)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_versionMajor = major;
            m_versionMinor = minor;
        }

#ifdef SWIFT_VATSIM_SUPPORT
        void setClientIdAndKey(quint16 id, const QByteArray &key);
#endif

        void setClientCapabilities(Capabilities capabilities)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_capabilities = capabilities;
        }
        void setServer(const swift::misc::network::CServer &server);
        void setLoginMode(const swift::misc::network::CLoginMode &mode)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_loginMode = mode;
        }
        void setCallsign(const swift::misc::aviation::CCallsign &callsign);
        void setPartnerCallsign(const swift::misc::aviation::CCallsign &callsign)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_partnerCallsign = callsign;
        }
        void setIcaoCodes(const swift::misc::simulation::CSimulatedAircraft &ownAircraft);
        void setLiveryAndModelString(const QString &livery, bool sendLiveryString, const QString &modelString,
                                     bool sendModelString);
        void setSimType(const swift::misc::simulation::CSimulatorInfo &simInfo);
        void setSimType(swift::misc::simulation::CSimulatorInfo::Simulator simulator);
        void setPilotRating(PilotRating rating)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_pilotRating = rating;
        }
        void setAtcRating(AtcRating rating)
        {
            QWriteLocker l(&m_lockUserClientBuffered);
            m_atcRating = rating;
        }
        //! @}

        // ------ thread safe access to preset values -----

        //! Get the server
        //! \threadsafe
        const swift::misc::network::CServer &getServer() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_server;
        }

        //! List of all preset values
        //! \threadsafe
        QStringList getPresetValues() const;

        //! Callsign if any
        //! \threadsafe
        swift::misc::aviation::CCallsign getPresetCallsign() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_ownCallsign;
        }

        //! Partner callsign if any
        //! \threadsafe
        swift::misc::aviation::CCallsign getPresetPartnerCallsign() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_partnerCallsign;
        }

        //! Mode
        //! \threadsafe
        swift::misc::network::CLoginMode getLoginMode() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_loginMode;
        }

        //! Rating
        //! \threadsafe
        PilotRating getPilotRating() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_pilotRating;
        }

        //! @{
        //! Connect/disconnect
        void connectToServer();
        void disconnectFromServer();
        //! @}

        //! @{
        //! Interim positions
        void addInterimPositionReceiver(const swift::misc::aviation::CCallsign &receiver)
        {
            m_interimPositionReceivers.push_back(receiver);
        }
        void removeInterimPositionReceiver(const swift::misc::aviation::CCallsign &receiver)
        {
            m_interimPositionReceivers.remove(receiver);
        }
        //! @}

        //! @{
        //! Convenience functions for sendClientQuery
        //! \remark pseudo private, used in CAirspaceMonitor and network context
        //! \private
        void sendClientQueryCapabilities(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryCom1Freq(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryRealName(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryServer(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryAtis(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryFlightPlan(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQueryAircraftConfig(const swift::misc::aviation::CCallsign &callsign);
        void sendTextMessages(const swift::misc::network::CTextMessageList &messages);
        void sendTextMessage(const swift::misc::network::CTextMessage &message);
        void sendTextMessage(TextMessageGroups receiverGroup, const QString &message);
        void sendRadioMessage(const QVector<int> &frequencieskHz, const QString &message);
        void sendFlightPlan(const swift::misc::aviation::CFlightPlan &flightPlan);
        void sendPlaneInfoRequest(const swift::misc::aviation::CCallsign &receiver);
        void sendPlaneInfoRequestFsinn(const swift::misc::aviation::CCallsign &callsign);
        //! @}

        //! @{
        //! Interim pos.receivers
        swift::misc::aviation::CCallsignSet getInterimPositionReceivers() const;
        void setInterimPositionReceivers(const swift::misc::aviation::CCallsignSet &interimPositionReceivers);
        //! @}

        //! @{
        //! Connection status
        swift::misc::network::CConnectionStatus getConnectionStatus() const
        {
            QReadLocker l(&m_lockConnectionStatus);
            return m_connectionStatus;
        }
        bool isConnected() const { return this->getConnectionStatus().isConnected(); }
        bool isDisconnected() const { return this->getConnectionStatus().isDisconnected(); }
        bool isPendingConnection() const;
        //! @}

        //! @{
        //! Statistics enable functions
        bool setStatisticsEnable(bool enabled)
        {
            m_statistics = enabled;
            return enabled;
        }
        bool isStatisticsEnabled() const { return m_statistics; }
        //! @}

        //! Clear the statistics
        void clearStatistics();

        //! Text statistics
        QString getNetworkStatisticsAsText(bool reset, const QString &separator = "\n");

        //! Debugging and UNIT tests
        void printToConsole(bool on) { m_printToConsole = on; }

        //! Gracefully shut down FSD client
        void gracefulShutdown();

    signals:
        //! @{
        //! Client responses received
        void atcDataUpdateReceived(const swift::misc::aviation::CCallsign &callsign,
                                   const swift::misc::physical_quantities::CFrequency &freq,
                                   const swift::misc::geo::CCoordinateGeodetic &pos,
                                   const swift::misc::physical_quantities::CLength &range);
        void deleteAtcReceived(const QString &cid);
        void deletePilotReceived(const QString &cid);
        void pilotDataUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation,
                                     const swift::misc::aviation::CTransponder &transponder);
        void pongReceived(const QString &sender, double elapsedTimeMs);
        void flightPlanReceived(const swift::misc::aviation::CCallsign &callsign,
                                const swift::misc::aviation::CFlightPlan &flightPlan);
        void textMessagesReceived(const swift::misc::network::CTextMessageList &messages);
        void aircraftConfigReceived(const QString &sender, const QJsonObject &config, qint64 currentOffsetTimeMs);
        void validAtcResponseReceived(const QString &callsign, bool isValidAtc);
        void capabilityResponseReceived(const swift::misc::aviation::CCallsign &sender,
                                        swift::misc::network::CClient::Capabilities capabilities);
        void com1FrequencyResponseReceived(const QString &sender,
                                           const swift::misc::physical_quantities::CFrequency &frequency);
        void realNameResponseReceived(const QString &sender, const QString &realName);
        void serverResponseReceived(const QString &sender, const QString &hostName);
        void planeInformationReceived(const QString &sender, const QString &aircraft, const QString &airline,
                                      const QString &livery);
        void customPilotPacketReceived(const QString &sender, const QStringList &data);
        void interimPilotDataUpdatedReceived(const swift::misc::aviation::CAircraftSituation &situation);
        void visualPilotDataUpdateReceived(const swift::misc::aviation::CAircraftSituation &situation);
        void euroscopeSimDataUpdatedReceived(const swift::misc::aviation::CAircraftSituation &situation,
                                             const swift::misc::aviation::CAircraftParts &parts,
                                             qint64 currentOffsetTimeMs, const QString &model, const QString &livery);
        void rawFsdMessage(const swift::misc::network::CRawFsdMessage &rawFsdMessage);
        void planeInformationFsinnReceived(const swift::misc::aviation::CCallsign &callsign,
                                           const QString &airlineIcaoDesignator, const QString &aircraftDesignator,
                                           const QString &combinedAircraftType, const QString &modelString);
        void revbAircraftConfigReceived(const QString &sender, const QString &config, qint64 currentOffsetTimeMs);
        void muteRequestReceived(bool mute);

        //! @}

        //! We received a reply to one of our ATIS queries.
        void atisReplyReceived(const swift::misc::aviation::CCallsign &callsign,
                               const swift::misc::aviation::CInformationMessage &atis);

        //! We received a reply to one of our ATIS queries, containing the controller's planned logoff time.
        void atisLogoffTimeReplyReceived(const swift::misc::aviation::CCallsign &callsign, const QString &zuluTime);

        //! We have sent a text message.
        void textMessageSent(const swift::misc::network::CTextMessage &sentMessage);

        //! Connection status has been changed
        void connectionStatusChanged(swift::misc::network::CConnectionStatus oldStatus,
                                     swift::misc::network::CConnectionStatus newStatus);

        //! Network error
        void severeNetworkError(const QString &errorMessage);

        //! Kill request (aka kicked)
        void killRequestReceived(const QString &reason);

    private:
        //! \cond
        friend SwiftFsdTest::CTestFSDClient;
        //! \endcond

        //! @{
        //! Convenience functions for sendClientQuery
        //! really private, ONLY used by UNIT test, not CAirspaceMonitor
        void sendLogin(const QString &token = {});
        void sendDeletePilot();
        void sendDeleteAtc();
        void sendPilotDataUpdate();
        void sendInterimPilotDataUpdate();
        void sendVisualPilotDataUpdate(bool slowUpdate = false);
        void sendAtcDataUpdate(double latitude, double longitude);
        void sendPing(const QString &receiver);
        //
        void sendClientQueryIsValidAtc(const swift::misc::aviation::CCallsign &callsign);
        void sendClientQuery(ClientQueryType queryType, const swift::misc::aviation::CCallsign &receiver,
                             const QStringList &queryData = {});
        void sendTextMessage(const QString &receiver, const QString &message);
        void sendPlaneInformation(const QString &receiver, const QString &aircraft, const QString &airline = {},
                                  const QString &livery = {});
        void sendPlaneInformationFsinn(const swift::misc::aviation::CCallsign &callsign);
        void sendAircraftConfiguration(const QString &receiver, const QString &aircraftConfigJson);
        //
        void sendMessageString(const QString &message);
        void sendQueuedMessage();
        //! @}

        //! @{
        //! Increase the statistics value for given identifier
        int increaseStatisticsValue(const QString &identifier, const QString &appendix = {});
        int increaseStatisticsValue(const QString &identifier, int value);
        //! @}

        //! Message send to FSD
        template <class T>
        void sendQueuedMessage(const T &message)
        {
            if (!message.isValid()) { return; }
            if (m_unitTestMode)
            {
                this->sendDirectMessage(message);
                return;
            }
            m_queuedFsdMessages.enqueue(messageToFSDString(message));
        }

        //! Message send to FSD
        template <class T>
        void sendDirectMessage(const T &message)
        {
            if (!message.isValid()) { return; }
            this->sendMessageString(messageToFSDString(message));
        }

        //! @{
        //! Unit test/debug functions
        void sendFsdMessage(const QString &message);
        void setUnitTestMode(bool on) { m_unitTestMode = on; }
        //! @}

        //! Default model string
        static const QString &defaultModelString()
        {
            static const QString dm("Cessna Skyhawk 172SP");
            return dm;
        }

        //! Send if no model string is available
        static const QString &noModelString()
        {
            static const QString noms("swift empty string");
            return noms;
        }

        //! Model string as configured, also dependent from simulator
        QString getConfiguredModelString(const swift::misc::simulation::CSimulatedAircraft &myAircraft) const;

        //! Livery string, also dependent from simulator
        QString getConfiguredLiveryString(const swift::misc::simulation::CSimulatedAircraft &myAircraft) const;

        //! JSON packets
        struct JsonPackets
        {
            static const QJsonObject &aircraftConfigRequest();
        };

        void sendAuthChallenge(const QString &challenge);
        void sendAuthResponse(const QString &response);
        void sendPong(const QString &receiver, const QString &timestamp);
        void sendClientResponse(ClientQueryType queryType, const QString &receiver);
#ifdef SWIFT_VATSIM_SUPPORT
        void sendClientIdentification(const QString &fsdChallenge);
#endif
        void sendIncrementalAircraftConfig();

        void readDataFromSocket() { this->readDataFromSocketMaxLines(); }
        void readDataFromSocketMaxLines(int maxLines = -1);
        void parseMessage(const QString &lineRaw);

        QString socketErrorString(QAbstractSocket::SocketError error) const;
        static QString socketErrorToQString(QAbstractSocket::SocketError error);

        //! Init. the message types
        void initializeMessageTypes();

        // Type to string
        const QString &messageTypeToString(MessageType mt) const;

        //! @{
        //! Handle response tokens
        void handleAtcDataUpdate(const QStringList &tokens);
#ifdef SWIFT_VATSIM_SUPPORT
        void handleAuthChallenge(const QStringList &tokens);
        void handleAuthResponse(const QStringList &tokens);
#endif
        void handleDeleteATC(const QStringList &tokens);
        void handleDeletePilot(const QStringList &tokens);
        void handleTextMessage(const QStringList &tokens);
        void handlePilotDataUpdate(const QStringList &tokens);
        void handleVisualPilotDataUpdate(const QStringList &tokens, MessageType messageType);
        void handleVisualPilotDataToggle(const QStringList &tokens);
        void handleEuroscopeSimData(const QStringList &tokens);
        void handlePing(const QStringList &tokens);
        void handlePong(const QStringList &tokens);
        void handleKillRequest(const QStringList &tokens);
        void handleFlightPlan(const QStringList &tokens);
        void handleClientQuery(const QStringList &tokens);
        void handleClientResponse(const QStringList &tokens);
        void handleServerError(const QStringList &tokens);
        void handleCustomPilotPacket(const QStringList &tokens);
#ifdef SWIFT_VATSIM_SUPPORT
        void handleFsdIdentification(const QStringList &tokens);
#endif
        void handleRevBClientPartsPacket(const QStringList &tokens);
        void handleRehost(const QStringList &tokens);
        void handleMute(const QStringList &tokens);

        //
        void handleUnknownPacket(const QString &line);
        void handleUnknownPacket(const QStringList &tokens);
        //! @}

        void printSocketError(QAbstractSocket::SocketError socketError);
        void handleSocketError(QAbstractSocket::SocketError socketError);
        void handleSocketConnected();

        void updateConnectionStatus(swift::misc::network::CConnectionStatus newStatus);

        //! Consolidate text messages if we receive multiple messages which belong together
        //! \remark causes a slight delay
        void consolidateTextMessage(const swift::misc::network::CTextMessage &textMessage);

        //! Send the consolidatedTextMessages
        void emitConsolidatedTextMessages();

        //! Remember when last position was received
        qint64 receivedPositionFixTsAndGetOffsetTime(const swift::misc::aviation::CCallsign &callsign,
                                                     qint64 markerTs = -1);

        //! Current offset time
        qint64 currentOffsetTime(const swift::misc::aviation::CCallsign &callsign) const;

        //! Clear state when connection is terminated
        void clearState();

        //! Clear state for callsign
        void clearState(const swift::misc::aviation::CCallsign &callsign);

        //! Insert as first value
        void insertLatestOffsetTime(const swift::misc::aviation::CCallsign &callsign, qint64 offsetMs);

        //! Average offset time in ms
        qint64 averageOffsetTimeMs(const swift::misc::aviation::CCallsign &callsign, int &count,
                                   int maxLastValues = c_maxOffsetTimes) const;

        bool isInterimPositionSendingEnabledForServer() const;
        bool isInterimPositionReceivingEnabledForServer() const;
        bool isVisualPositionSendingEnabledForServer() const;
        const swift::misc::network::CFsdSetup &getSetupForServer() const;

        //! Handles ATIS replies from non-VATSIM servers. If the conditions are not met,
        //! the message is released as normal text message.
        void maybeHandleAtisReply(const swift::misc::aviation::CCallsign &sender,
                                  const swift::misc::aviation::CCallsign &receiver, const QString &message);

        //! Settings have been changed
        void fsdMessageSettingsChanged();

        //! Emit raw FSD message (mostly for debugging)
        void emitRawFsdMessage(const QString &fsdMessage, bool isSent);

        //! Save the statistics
        bool saveNetworkStatistics(const QString &server);

        //! @{
        //! Timers
        void startPositionTimers();
        void stopPositionTimers();
        //! @}

        //! Update the ATIS map
        void updateAtisMap(const QString &callsign, AtisLineType type, const QString &line);

        //! Check if there is a pending logon attempt which "hangs"
        void pendingTimeoutCheck();

        //! Fix ATC station range
        static swift::misc::physical_quantities::CLength
        fixAtcRange(const swift::misc::physical_quantities::CLength &networkRange,
                    const swift::misc::aviation::CCallsign &cs);

        //! Max or 1st non-null value
        static swift::misc::physical_quantities::CLength
        maxOrNotNull(const swift::misc::physical_quantities::CLength &l1,
                     const swift::misc::physical_quantities::CLength &l2);

        //! String without colons
        static QString noColons(const QString &input);

        //! Get a short-lived, one-time-use token from Vatsim web service, to avoid sending plaintext password to FSD
        void getVatsimAuthToken(const QString &cid, const QString &password,
                                const swift::misc::CSlot<void(const QString &)> &callback);

        //! Convert FlightRules to FlightType
        static FlightType getFlightType(swift::misc::aviation::CFlightPlan::FlightRules flightRule);

#ifdef SWIFT_VATSIM_SUPPORT
        vatsim_auth *m_clientAuth = nullptr;
        vatsim_auth *m_serverAuth = nullptr;
#endif
        QString m_lastServerAuthChallenge;
        qint64 m_loginSince = -1; //!< when login was triggered
        static constexpr qint64 PendingConnectionTimeoutMs = 7500;

        // Parser
        QHash<QString, MessageType> m_messageTypeMapping;

        std::shared_ptr<QTcpSocket> m_socket =
            std::make_shared<QTcpSocket>(this); //!< used TCP socket, parent needed as it runs in worker thread
        void connectSocketSignals();
        void initiateConnection(std::shared_ptr<QTcpSocket> rehostingSocket = {}, const QString &rehostingHost = {});
        void resolveLoadBalancing(const QString &host, std::function<void(const QString &)> callback);
        bool m_rehosting = false;

        std::atomic_bool m_unitTestMode { false };
        std::atomic_bool m_printToConsole { false };

        swift::misc::network::CConnectionStatus m_connectionStatus;
        mutable QReadWriteLock m_lockConnectionStatus { QReadWriteLock::Recursive };

        swift::misc::aviation::CAircraftParts m_sentAircraftConfig; //!< aircraft parts sent
        swift::misc::CTokenBucket m_tokenBucket; //!< used with aircraft parts messages
        swift::misc::aviation::CCallsignSet m_interimPositionReceivers; //!< all aircraft receiving interim positions
        swift::misc::network::CTextMessageList m_textMessagesToConsolidate; //!< waiting for new messages
        misc::CDigestSignal m_dsSendTextMessage { this, &CFSDClient::emitConsolidatedTextMessages,
                                                  std::chrono::milliseconds(250), 10 };

        //! ATIS message
        struct AtisMessage
        {
            QString m_voiceRoom;
            QStringList m_textLines;
            QString m_zuluLogoff;
            int m_lineCount = 0;
        };

        QMap<QString, AtisMessage> m_mapAtisMessages;

        //! Pending ATIS query since
        struct PendingAtisQuery
        {
            QDateTime m_queryTime = QDateTime::currentDateTimeUtc();
            QStringList m_atisMessage;
        };

        QHash<swift::misc::aviation::CCallsign, PendingAtisQuery> m_pendingAtisQueries;
        QHash<swift::misc::aviation::CCallsign, qint64> m_lastPositionUpdate;
        QHash<swift::misc::aviation::CCallsign, QList<qint64>> m_lastOffsetTimes; //!< latest offset first

        swift::misc::aviation::CAtcStationList m_atcStations;

        swift::misc::CSettingReadOnly<swift::core::vatsim::TRawFsdMessageSetting> m_fsdMessageSetting {
            this, &CFSDClient::fsdMessageSettingsChanged
        };
        QFile m_rawFsdMessageLogFile;
        std::atomic_bool m_rawFsdMessagesEnabled { false };
        std::atomic_bool m_filterPasswordFromLogin { false };

        // timer parents are needed as we move to thread
        QTimer m_scheduledConfigUpdate { this }; //!< config updates
        QTimer m_positionUpdateTimer { this }; //!< sending positions
        QTimer m_interimPositionUpdateTimer { this }; //!< sending interim positions
        QTimer m_visualPositionUpdateTimer { this }; //!< sending visual positions
        QTimer m_fsdSendMessageTimer { this }; //!< FSD message sending

        qint64 m_additionalOffsetTime = 0; //!< additional offset time

        std::atomic_bool m_statistics { false };
        QMap<QString, int> m_callStatistics; //!< how many calls?
        QVector<QPair<qint64, QString>> m_callByTime; //!< "last call vs. ms"
        mutable QReadWriteLock m_lockStatistics { QReadWriteLock::Recursive }; //!< for user, client and buffered data

        // User data
        swift::misc::network::CServer m_server;
        swift::misc::network::CLoginMode m_loginMode;
        QStringEncoder m_encoder;
        QStringDecoder m_decoder;
        SimType m_simType = SimType::Unknown;
        PilotRating m_pilotRating = PilotRating::Unknown;
        AtcRating m_atcRating = AtcRating::Unknown;
        swift::misc::simulation::CSimulatorInfo m_simTypeInfo; // same as m_simType

        // Client data
        QString m_clientName;
        QString m_hostApplication;
        int m_versionMajor = 0;
        int m_versionMinor = 0;
        int m_protocolRevision = 0;
        ServerType m_serverType = ServerType::LegacyFsd;
        Capabilities m_capabilities = Capabilities::None;

        // buffered data for FSD
        swift::misc::aviation::CCallsign m_ownCallsign; //!< "buffered callsign", as this must not change when connected
        swift::misc::aviation::CCallsign
            m_partnerCallsign; //!< "buffered"callsign", of partner flying in shared cockpit
        swift::misc::aviation::CAircraftIcaoCode
            m_ownAircraftIcaoCode; //!< "buffered icao", as this must not change when connected
        swift::misc::aviation::CAirlineIcaoCode
            m_ownAirlineIcaoCode; //!< "buffered icao", as this must not change when connected
        QString m_ownLivery; //!< "buffered livery", as this must not change when connected
        QString m_ownModelString; //!< "buffered model string", as this must not change when connected
        std::atomic_bool m_sendLiveryString { true };
        std::atomic_bool m_sendModelString { true };

        mutable QReadWriteLock m_lockUserClientBuffered {
            QReadWriteLock::Recursive
        }; //!< for user, client and buffered data
        QString getOwnCallsignAsString() const
        {
            QReadLocker l(&m_lockUserClientBuffered);
            return m_ownCallsign.asString();
        }

        QQueue<QString> m_queuedFsdMessages;

        //! An illegal FSD state has been detected
        void handleIllegalFsdState(const QString &message);

        static int constexpr c_maxOffsetTimes = 6; //!< Max offset times kept
        static int constexpr c_processingIntervalMsec = 100; //!< interval for the processing timer
        static int constexpr c_updatePositionIntervalMsec =
            5000; //!< interval for the position update timer (send our position to network)
        static int constexpr c_updateInterimPositionIntervalMsec =
            1000; //!< interval for interim position updates (send our position as interim position)
        static int constexpr c_updateVisualPositionIntervalMsec =
            200; //!< interval for the VATSIM visual position updates (send our position and 6DOF velocity)
        static int constexpr c_sendFsdMsgIntervalMsec = 10; //!< interval for FSD send messages
        bool m_stoppedSendingVisualPositions = false; //!< for when velocity drops to zero
        bool m_serverWantsVisualPositions = false; //!< there are interested clients in range
        unsigned m_visualPositionUpdateSentCount = 0; //!< for choosing when to send a periodic (slowfast) packet
    };
} // namespace swift::core::fsd

#endif // SWIFT_CORE_FSD_CLIENT_H

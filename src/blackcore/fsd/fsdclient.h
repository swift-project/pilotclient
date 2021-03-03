/* Copyright (C) 2019
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_FSD_CLIENT_H
#define BLACKCORE_FSD_CLIENT_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/fsd/enums.h"
#include "blackcore/fsd/messagebase.h"

#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/network/rawfsdmessage.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/ecosystemprovider.h"
#include "blackmisc/network/clientprovider.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/worker.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/tokenbucket.h"

#include "vatsim/vatsimauth.h"

#include <QtGlobal>
#include <QHash>
#include <QString>
#include <QObject>
#include <QTcpSocket>
#include <QCommandLineOption>
#include <QTimer>
#include <QTextCodec>
#include <QReadWriteLock>
#include <QQueue>

#include <atomic>

//! Protocol version
//! @{
#define PROTOCOL_REVISION_CLASSIC   9
#define PROTOCOL_REVISION_VATSIM_ATC 10
#define PROTOCOL_REVISION_VATSIM_AUTH 100
//! @}

namespace BlackFsdTest { class CTestFSDClient; }
namespace BlackCore
{
    namespace Fsd
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
        class BLACKCORE_EXPORT CFSDClient :
            public BlackMisc::CContinuousWorker,
            public BlackMisc::Network::IEcosystemProvider,            // provide info about used ecosystem
            public BlackMisc::Network::CClientAware,                  // network can set client information
            public BlackMisc::Simulation::COwnAircraftAware,          // network vatlib consumes own aircraft data and sets ICAO/callsign data
            public BlackMisc::Simulation::CRemoteAircraftAware,       // check if we really need to process network packets (e.g. parts)
            public BlackMisc::Simulation::CSimulationEnvironmentAware // allows to consume ground elevations
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Network::IEcosystemProvider)

        public:
            //! Categories
            static const QStringList &getLogCategories();

            //! Ctor
            CFSDClient(BlackMisc::Network::IClientProvider            *clientProvider,
                       BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                       QObject *owner = nullptr);

            //! Preset functions
            //! \remark Necessary functions to setup client. Set them all!
            //! \threadsafe
            //! @{
            void setClientName(const QString &clientName) { QWriteLocker l(&m_lockUserClientBuffered); m_clientName = clientName; }
            void setHostApplication(const QString &hostApplication) { QWriteLocker l(&m_lockUserClientBuffered); m_hostApplication = hostApplication; }
            void setVersion(int major, int minor) { QWriteLocker l(&m_lockUserClientBuffered); m_versionMajor = major; m_versionMinor = minor; }
            void setClientIdAndKey(quint16 id, const QByteArray &key);
            void setClientCapabilities(Capabilities capabilities) { QWriteLocker l(&m_lockUserClientBuffered); m_capabilities = capabilities; }
            void setServer(const BlackMisc::Network::CServer &server);
            void setLoginMode(const BlackMisc::Network::CLoginMode &mode) { QWriteLocker l(&m_lockUserClientBuffered); m_loginMode = mode; }
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
            void setPartnerCallsign(const BlackMisc::Aviation::CCallsign &callsign) { QWriteLocker l(&m_lockUserClientBuffered); m_partnerCallsign = callsign; }
            void setIcaoCodes(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);
            void setLiveryAndModelString(const QString &livery, bool sendLiveryString, const QString &modelString, bool sendModelString);
            void setSimType(const BlackMisc::Simulation::CSimulatorInfo &simInfo);
            void setSimType(BlackMisc::Simulation::CSimulatorInfo::Simulator simulator);
            void setPilotRating(PilotRating rating) { QWriteLocker l(&m_lockUserClientBuffered); m_pilotRating = rating; }
            void setAtcRating(AtcRating rating)     { QWriteLocker l(&m_lockUserClientBuffered); m_atcRating = rating; }
            //! @}

            // ------ thread safe access to preset values -----

            //! Get the server
            //! \threadsafe
            const BlackMisc::Network::CServer &getServer() const { QReadLocker l(&m_lockUserClientBuffered); return m_server; }

            //! List of all preset values
            //! \threadsafe
            QStringList getPresetValues() const;

            //! Callsign if any
            //! \threadsafe
            BlackMisc::Aviation::CCallsign getPresetCallsign() const { QReadLocker l(&m_lockUserClientBuffered); return m_ownCallsign; }

            //! Partner callsign if any
            //! \threadsafe
            BlackMisc::Aviation::CCallsign getPresetPartnerCallsign() const { QReadLocker l(&m_lockUserClientBuffered); return m_partnerCallsign; }

            //! Mode
            //! \threadsafe
            BlackMisc::Network::CLoginMode getLoginMode() const { QReadLocker l(&m_lockUserClientBuffered); return m_loginMode; }

            //! Rating
            //! \threadsafe
            PilotRating getPilotRating() const { QReadLocker l(&m_lockUserClientBuffered); return m_pilotRating; }

            //! Connenct/disconnect
            //! @{
            void connectToServer();
            void disconnectFromServer();
            //! @}

            //! Interim positions
            //! @{
            void addInterimPositionReceiver(const BlackMisc::Aviation::CCallsign &receiver) { m_interimPositionReceivers.push_back(receiver); }
            void removeInterimPositionReceiver(const BlackMisc::Aviation::CCallsign &receiver) { m_interimPositionReceivers.remove(receiver); }
            //! @}

            //! Convenience functions for sendClientQuery
            //! \remark pseudo private, used in CAirspaceMonitor and network context
            //! \private
            //!  @{
            void sendClientQueryCapabilities(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryCom1Freq(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryRealName(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryServer(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryAtis(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryFlightPlan(const BlackMisc::Aviation::CCallsign callsign);
            void sendClientQueryAircraftConfig(const BlackMisc::Aviation::CCallsign callsign);
            void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages);
            void sendTextMessage(const BlackMisc::Network::CTextMessage &message);
            void sendTextMessage(TextMessageGroups receiverGroup, const QString &message);
            void sendRadioMessage(const QVector<int> &frequencieskHz, const QString &message);
            void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan);
            void sendPlaneInfoRequest(const BlackMisc::Aviation::CCallsign &receiver);
            void sendPlaneInfoRequestFsinn(const BlackMisc::Aviation::CCallsign &callsign);
            //! @}

            //! Interim pos.receivers
            //! @{
            BlackMisc::Aviation::CCallsignSet getInterimPositionReceivers() const;
            void setInterimPositionReceivers(const BlackMisc::Aviation::CCallsignSet &interimPositionReceivers);
            //! @}

            //! Connection status
            //! @{
            BlackMisc::Network::CConnectionStatus getConnectionStatus() const { QReadLocker l(&m_lockConnectionStatus); return m_connectionStatus; }
            bool isConnected()    const { return this->getConnectionStatus().isConnected(); }
            bool isDisconnected() const { return this->getConnectionStatus().isDisconnected(); }
            bool isPendingConnection() const;
            //! @}

            //! Statistics enable functions
            //! @{
            bool setStatisticsEnable(bool enabled) { m_statistics = enabled; return enabled; }
            bool isStatisticsEnabled() const { return m_statistics; }
            //! @}

            //! Clear the statistics
            void clearStatistics();

            //! Text statistics
            QString getNetworkStatisticsAsText(bool reset, const QString &separator = "\n");

            //! Debugging and UNIT tests
            void printToConsole(bool on)  { m_printToConsole = on; }

            //! Gracefully shut down FSD client
            void gracefulShutdown();

        signals:
            //! Client responses received
            //! @{
            void atcDataUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                                       const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength    &range);
            void deleteAtcReceived(const QString &cid);
            void deletePilotReceived(const QString &cid);
            void pilotDataUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation, const BlackMisc::Aviation::CTransponder &transponder);
            void pongReceived(const QString &sender, double elapsedTimeMs);
            void flightPlanReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CFlightPlan &flightPlan);
            void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);
            void aircraftConfigReceived(const QString &sender, const QJsonObject &config, qint64 currentOffsetTimeMs);
            void validAtcResponseReceived(const QString &callsign, bool isValidAtc);
            void capabilityResponseReceived(const BlackMisc::Aviation::CCallsign &sender, BlackMisc::Network::CClient::Capabilities capabilities);
            void com1FrequencyResponseReceived(const QString &sender, const BlackMisc::PhysicalQuantities::CFrequency &frequency);
            void realNameResponseReceived(const QString &sender, const QString &realName);
            void serverResponseReceived(const QString &sender, const QString &hostName);
            void planeInformationReceived(const QString &sender, const QString &aircraft, const QString &airline, const QString &livery);
            void customPilotPacketReceived(const QString &sender, const QStringList &data);
            void interimPilotDataUpdatedReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
            void visualPilotDataUpdateReceived(const BlackMisc::Aviation::CAircraftSituation &situation);
            void rawFsdMessage(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);
            void planeInformationFsinnReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &modelString);
            void revbAircraftConfigReceived(const QString &sender, const QString &config, qint64 currentOffsetTimeMs);

            //! @}

            //! We received a reply to one of our ATIS queries.
            void atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);

            //! We received a reply to one of our ATIS queries, containing the controller's planned logoff time.
            void atisLogoffTimeReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);

            //! We have sent a text message.
            void textMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

            //! Connection status has been changed
            void connectionStatusChanged(BlackMisc::Network::CConnectionStatus oldStatus, BlackMisc::Network::CConnectionStatus newStatus);

            //! Network error
            void severeNetworkError(const QString &errorMessage);

            //! Kill request (aka kicked)
            void killRequestReceived(const QString &reason);

        private:
            //! \cond
            friend BlackFsdTest::CTestFSDClient;
            //! \endcond

            //! Convenience functions for sendClientQuery
            //! \remark really private, ONLY used by UNIT test, not CAirspaceMonitor
            //!  @{
            void sendLogin();
            void sendDeletePilot();
            void sendDeleteAtc();
            void sendPilotDataUpdate();
            void sendInterimPilotDataUpdate();
            void sendVisualPilotDataUpdate();
            void sendAtcDataUpdate(double latitude, double longitude);
            void sendPing(const QString &receiver);
            //
            void sendClientQueryIsValidAtc(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQuery(ClientQueryType queryType, const BlackMisc::Aviation::CCallsign &receiver, const QStringList &queryData = {});
            void sendTextMessage(const QString &receiver, const QString &message);
            void sendPlaneInformation(const QString &receiver, const QString &aircraft, const QString &airline = {}, const QString &livery = {});
            void sendPlaneInformationFsinn(const BlackMisc::Aviation::CCallsign &callsign);
            void sendAircraftConfiguration(const QString &receiver, const QString &aircraftConfigJson);
            //
            void sendMessageString(const QString &message);
            void sendQueuedMessage();
            //! @}

            //! Increase the statistics value for given identifier
            //! @{
            int increaseStatisticsValue(const QString &identifier, const QString &appendix = {});
            int increaseStatisticsValue(const QString &identifier, int value);
            //! @}

            //! Message send to FSD
            template <class T>
            void sendQueudedMessage(const T &message)
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

            //! Unit test/debug functions
            //! @{
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
            QString getConfiguredModelString(const BlackMisc::Simulation::CSimulatedAircraft &myAircraft) const;

            //! Livery string, also dependent from simulator
            QString getConfiguredLiveryString(const BlackMisc::Simulation::CSimulatedAircraft &myAircraft) const;

            //! JSON packets
            struct JsonPackets
            {
                static const QJsonObject &aircraftConfigRequest();
            };

            void sendAuthChallenge(const QString &challenge);
            void sendAuthResponse(const QString &response);
            void sendPong(const QString &receiver, const QString &timestamp);
            void sendClientResponse(ClientQueryType queryType, const QString &receiver);
            void sendClientIdentification(const QString &fsdChallenge);
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

            //! Handle response tokens
            //! @{
            void handleAtcDataUpdate(const QStringList &tokens);
            void handleAuthChallenge(const QStringList &tokens);
            void handleAuthResponse(const QStringList &tokens);
            void handleDeleteATC(const QStringList &tokens);
            void handleDeletePilot(const QStringList &tokens);
            void handleTextMessage(const QStringList &tokens);
            void handlePilotDataUpdate(const QStringList &tokens);
            void handleVisualPilotDataUpdate(const QStringList &tokens);
            void handlePing(const QStringList &tokens);
            void handlePong(const QStringList &tokens);
            void handleKillRequest(const QStringList &tokens);
            void handleFlightPlan(const QStringList &tokens);
            void handleClientQuery(const QStringList &tokens);
            void handleClientReponse(const QStringList &tokens);
            void handleServerError(const QStringList &tokens);
            void handleCustomPilotPacket(const QStringList &tokens);
            void handleFsdIdentification(const QStringList &tokens);
            void handleRevBClientPartsPacket(const QStringList &tokens);

            //
            void handleUnknownPacket(const QString &line);
            void handleUnknownPacket(const QStringList &tokens);
            //! @}

            void printSocketError(QAbstractSocket::SocketError socketError);
            void handleSocketError(QAbstractSocket::SocketError socketError);
            void handleSocketConnected();

            void updateConnectionStatus(BlackMisc::Network::CConnectionStatus newStatus);

            //! Consolidate text messages if we receive multiple messages which belong together
            //! \remark causes a slight delay
            void consolidateTextMessage(const BlackMisc::Network::CTextMessage &textMessage);

            //! Send the consolidatedTextMessages
            void emitConsolidatedTextMessages();

            //! Remember when last position was received
            qint64 receivedPositionFixTsAndGetOffsetTime(const BlackMisc::Aviation::CCallsign &callsign, qint64 markerTs = -1);

            //! Current offset time
            qint64 currentOffsetTime(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Clear state when connection is terminated
            void clearState();

            //! Clear state for callsign
            void clearState(const BlackMisc::Aviation::CCallsign &callsign);

            //! Insert as first value
            void insertLatestOffsetTime(const BlackMisc::Aviation::CCallsign &callsign, qint64 offsetMs);

            //! Average offset time in ms
            qint64 averageOffsetTimeMs(const BlackMisc::Aviation::CCallsign &callsign, int &count, int maxLastValues = MaxOffseTimes) const;

            //! Average offset time in ms
            qint64 averageOffsetTimeMs(const BlackMisc::Aviation::CCallsign &callsign, int maxLastValues = MaxOffseTimes) const;

            bool isInterimPositionSendingEnabledForServer() const;
            bool isInterimPositionReceivingEnabledForServer() const;
            bool isVisualPositionSendingEnabledForServer() const;
            const BlackMisc::Network::CFsdSetup &getSetupForServer() const;

            //! Handles ATIS replies from non-VATSIM servers. If the conditions are not met,
            //! the message is released as normal text message.
            void maybeHandleAtisReply(const BlackMisc::Aviation::CCallsign &sender, const BlackMisc::Aviation::CCallsign &receiver, const QString &message);

            //! Settings have been changed
            void fsdMessageSettingsChanged();

            //! Emit raw FSD message (mostly for debugging)
            void emitRawFsdMessage(const QString &fsdMessage, bool isSent);

            //! Save the statistics
            bool saveNetworkStatistics(const QString &server);

            //! Timers
            //! @{
            void startPositionTimers();
            void stopPositionTimers();
            //! @}

            //! Update the ATIS map
            void updateAtisMap(const QString &callsign, AtisLineType type, const QString &line);

            //! Check if there is a pending logon attempt which "hangs"
            void pendingTimeoutCheck();

            //! Fix ATC station range
            static const BlackMisc::PhysicalQuantities::CLength &fixAtcRange(const BlackMisc::PhysicalQuantities::CLength &networkRange, const BlackMisc::Aviation::CCallsign &cs);

            //! Max or 1st non-null value
            static const BlackMisc::PhysicalQuantities::CLength &maxOrNotNull(const BlackMisc::PhysicalQuantities::CLength &l1, const BlackMisc::PhysicalQuantities::CLength &l2);

            //! String withou colons
            static QString noColons(const QString &input);

            vatsim_auth *m_clientAuth = nullptr;
            vatsim_auth *m_serverAuth = nullptr;
            QString      m_lastServerAuthChallenge;
            qint64       m_loginSince = -1; //!< when login was triggered
            static constexpr qint64 PendingConnectionTimeoutMs = 7500;

            // Parser
            QHash<QString, MessageType> m_messageTypeMapping;

            QTcpSocket m_socket { this }; //!< used TCP socket, parent needed as it runs in worker thread

            std::atomic_bool m_unitTestMode   { false };
            std::atomic_bool m_printToConsole { false };

            BlackMisc::Network::CConnectionStatus m_connectionStatus;
            mutable QReadWriteLock m_lockConnectionStatus { QReadWriteLock::Recursive };

            BlackMisc::Aviation::CAircraftParts   m_sentAircraftConfig;        //!< aircraft parts sent
            BlackMisc::CTokenBucket               m_tokenBucket;               //!< used with aircraft parts messages
            BlackMisc::Aviation::CCallsignSet     m_interimPositionReceivers;  //!< all aircraft receiving interim positions
            BlackMisc::Network::CTextMessageList  m_textMessagesToConsolidate; //!< waiting for new messages
            BlackMisc::CDigestSignal              m_dsSendTextMessage  { this, &CFSDClient::emitConsolidatedTextMessages, 250, 10 };

            //! ATIS message
            struct AtisMessage
            {
                QString voiceRoom;
                QStringList textLines;
                QString zuluLogoff;
                int lineCount = 0;
            };

            QMap<QString, AtisMessage> m_mapAtisMessages;

            //! Pending ATIS query since
            struct PendingAtisQuery
            {
                QDateTime   m_queryTime = QDateTime::currentDateTimeUtc();
                QStringList m_atisMessage;
            };

            QHash<BlackMisc::Aviation::CCallsign, PendingAtisQuery> m_pendingAtisQueries;
            QHash<BlackMisc::Aviation::CCallsign, qint64> m_lastPositionUpdate;
            QHash<BlackMisc::Aviation::CCallsign, QList<qint64>> m_lastOffsetTimes; //!< latest offset first

            BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TRawFsdMessageSetting> m_fsdMessageSetting { this, &CFSDClient::fsdMessageSettingsChanged };
            QFile m_rawFsdMessageLogFile;
            std::atomic_bool m_rawFsdMessagesEnabled   { false };
            std::atomic_bool m_filterPasswordFromLogin { false };

            // timer parents are needed as we move to thread
            QTimer m_scheduledConfigUpdate      { this }; //!< config updates
            QTimer m_positionUpdateTimer        { this }; //!< sending positions
            QTimer m_interimPositionUpdateTimer { this }; //!< sending interim positions
            QTimer m_visualPositionUpdateTimer  { this }; //!< sending visual positions
            QTimer m_fsdSendMessageTimer        { this }; //!< FSD message sending

            qint64 m_additionalOffsetTime = 0; //!< additional offset time

            std::atomic_bool m_statistics { false };
            QMap <QString, int> m_callStatistics;          //!< how many calls?
            QVector <QPair<qint64, QString>> m_callByTime; //!< "last call vs. ms"
            mutable QReadWriteLock m_lockStatistics { QReadWriteLock::Recursive }; //!< for user, client and buffered data

            // User data
            BlackMisc::Network::CServer    m_server;
            BlackMisc::Network::CLoginMode m_loginMode;
            QTextCodec  *m_fsdTextCodec = nullptr;
            SimType      m_simType      = SimType::Unknown;
            PilotRating  m_pilotRating  = PilotRating::Unknown;
            AtcRating    m_atcRating    = AtcRating::Unknown;
            BlackMisc::Simulation::CSimulatorInfo m_simTypeInfo; // same as m_simType

            // Client data
            QString m_clientName;
            QString m_hostApplication;
            int m_versionMajor     = 0;
            int m_versionMinor     = 0;
            int m_protocolRevision = 0;
            ServerType   m_serverType   = ServerType::LegacyFsd;
            Capabilities m_capabilities = Capabilities::None;

            // buffered data for FSD
            BlackMisc::Aviation::CCallsign              m_ownCallsign;          //!< "buffered callsign", as this must not change when connected
            BlackMisc::Aviation::CCallsign              m_partnerCallsign;      //!< "buffered"callsign", of partner flying in shared cockpit
            BlackMisc::Aviation::CAircraftIcaoCode      m_ownAircraftIcaoCode;  //!< "buffered icao", as this must not change when connected
            BlackMisc::Aviation::CAirlineIcaoCode       m_ownAirlineIcaoCode;   //!< "buffered icao", as this must not change when connected
            QString                                     m_ownLivery;            //!< "buffered livery", as this must not change when connected
            QString                                     m_ownModelString;       //!< "buffered model string", as this must not change when connected
            std::atomic_bool m_sendLiveryString { true };
            std::atomic_bool m_sendModelString  { true };

            mutable QReadWriteLock m_lockUserClientBuffered { QReadWriteLock::Recursive }; //!< for user, client and buffered data
            QString getOwnCallsignAsString() const { QReadLocker l(&m_lockUserClientBuffered); return m_ownCallsign.asString(); }

            QQueue<QString> m_queuedFsdMessages;

            //! An illegal FSD state has been detected
            void handleIllegalFsdState(const QString &message);

            static const int MaxOffseTimes = 6; //!< Max offset times kept
            static int constexpr c_processingIntervalMsec           = 100;  //!< interval for the processing timer
            static int constexpr c_updatePostionIntervalMsec        = 5000; //!< interval for the position update timer (send our position to network)
            static int constexpr c_updateInterimPostionIntervalMsec = 1000; //!< interval for iterim position updates (send our position as interim position)
            static int constexpr c_updateVisualPositionIntervalMsec = 200;  //!< interval for the VATSIM visual position updates (send our position and 6DOF velocity)
            static int constexpr c_sendFsdMsgIntervalMsec           = 10;   //!< interval for FSD send messages
        };
    } // ns
} // ns

#endif

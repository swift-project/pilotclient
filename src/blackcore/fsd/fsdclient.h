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

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/informationmessage.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/network/connectionstatus.h"
#include "blackmisc/network/loginmode.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/ecosystemprovider.h"
#include "blackmisc/network/clientprovider.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
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

//! Protocol version @{
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
            public QObject,
            public BlackMisc::Network::IEcosystemProvider,            // provide info about used ecosystem
            public BlackMisc::Network::CClientAware,                  // network can set client information
            public BlackMisc::Simulation::COwnAircraftAware,          // network vatlib consumes own aircraft data and sets ICAO/callsign data
            public BlackMisc::Simulation::CRemoteAircraftAware,       // check if we really need to process network packets (e.g. parts)
            public BlackMisc::Simulation::CSimulationEnvironmentAware // allows to consume ground elevations
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Network::IEcosystemProvider)

        public:
            //! Ctor
            CFSDClient(BlackMisc::Network::IClientProvider *clientProvider,
                       BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                       QObject *parent = nullptr);

            //! Preset functions
            //! \remark Necessary functions to setup client. Set them all!
            //! @{
            void setClientName(const QString &clientName) { m_clientName = clientName; }
            void setHostApplication(const QString &hostApplication) { m_hostApplication = hostApplication; }
            void setVersion(int major, int minor) { m_versionMajor = major; m_versionMinor = minor; }
            void setClientIdAndKey(quint16 id, const QByteArray &key);
            void setClientCapabilities(Capabilities capabilities) { m_capabilities = capabilities; }
            void setServer(const BlackMisc::Network::CServer &server);
            void setSimulatorInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &simInfo);
            void setLoginMode(const BlackMisc::Network::CLoginMode &mode) { m_loginMode = mode; }
            void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
            void setPartnerCallsign(const BlackMisc::Aviation::CCallsign &callsign) { m_partnerCallsign = callsign; }
            void setIcaoCodes(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);
            void setLiveryAndModelString(const QString &livery, bool sendLiveryString, const QString &modelString, bool sendModelString);
            void setSimType(SimType type) { m_simType = type; }
            void setSimType(const BlackMisc::Simulation::CSimulatorPluginInfo &simInfo);
            void setPilotRating(PilotRating rating) { m_pilotRating = rating; }
            void setAtcRating(AtcRating rating) { m_atcRating = rating; }
            //! @}

            //! Get the server
            const BlackMisc::Network::CServer &getServer() const { return m_server; }

            //! List of all preset values
            QStringList getPresetValues() const;

            //! Callsign
            BlackMisc::Aviation::CCallsign getPresetPartnerCallsign() const { return m_partnerCallsign; }

            //! Mode
            BlackMisc::Network::CLoginMode getLoginMode() const;

            //! Connenct/disconnect @{
            void connectToServer();
            void disconnectFromServer();
            //! @}

            //! Interim positions @{
            void addInterimPositionReceiver(const BlackMisc::Aviation::CCallsign &receiver) { m_interimPositionReceivers.push_back(receiver); }
            void removeInterimPositionReceiver(const BlackMisc::Aviation::CCallsign &receiver) { m_interimPositionReceivers.remove(receiver); }
            //! @}

            //! Convenience functions for sendClientQuery
            //! \private
            //!  @{
            void sendLogin();
            void sendDeletePilot();
            void sendDeleteAtc();
            void sendPilotDataUpdate();
            void sendInterimPilotDataUpdate();
            void sendAtcDataUpdate(double latitude, double longitude);
            void sendPing(const QString &receiver);
            //
            void sendClientQueryIsValidAtc(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryCapabilities(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryCom1Freq(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryRealName(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryServer(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryAtis(const BlackMisc::Aviation::CCallsign &callsign);
            void sendClientQueryFlightPlan(const BlackMisc::Aviation::CCallsign callsign);
            void sendClientQueryAircraftConfig(const BlackMisc::Aviation::CCallsign callsign);
            void sendClientQuery(ClientQueryType queryType, const BlackMisc::Aviation::CCallsign &receiver, const QStringList &queryData = {});
            void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages);
            void sendTextMessage(const BlackMisc::Network::CTextMessage &message);
            void sendTextMessage(TextMessageGroups receiverGroup, const QString &message);
            void sendTextMessage(const QString &receiver, const QString &message);
            void sendRadioMessage(const QVector<int> &frequencieskHz, const QString &message);
            void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan);
            void sendPlaneInfoRequest(const BlackMisc::Aviation::CCallsign &receiver);
            void sendPlaneInfoRequestFsinn(const BlackMisc::Aviation::CCallsign &callsign);
            void sendPlaneInformation(const QString &receiver, const QString &aircraft, const QString &airline = {}, const QString &livery = {});
            void sendPlaneInformationFsinn(const BlackMisc::Aviation::CCallsign &callsign);
            void sendCustomPilotPacket(const QString &receiver, const QString &subType, const std::vector<QString> &payload);
            void sendAircraftConfiguration(const QString &receiver, const QString &aircraftConfigJson);
            //! @}

            //! Unit test/debug functions @{
            void sendFsdMessage(const QString &message);
            void setUnitTestMode(bool on) { m_unitTestMode = on; }
            void printToConsole(bool on)  { m_printToConsole = on; }
            //! @}

            //! Interim pos.receivers @{
            BlackMisc::Aviation::CCallsignSet getInterimPositionReceivers() const;
            void setInterimPositionReceivers(const BlackMisc::Aviation::CCallsignSet &interimPositionReceivers);
            //! @}

            //! Connection status @{
            BlackMisc::Network::CConnectionStatus getConnectionStatus() const { return m_connectionStatus; }
            bool isConnected() const { return m_connectionStatus.isConnected(); }
            bool isPendingConnection() const { return m_connectionStatus.isConnecting() || m_connectionStatus.isDisconnecting(); }
            //! @}

            //! Statistics enable functions @{
            bool setStatisticsEnable(bool enabled) { m_statistics = enabled; return enabled; }
            bool isStatisticsEnabled() const { return m_statistics; }
            //! @}

            //! Increase the statistics value for given identifier @{
            int increaseStatisticsValue(const QString &identifier, const QString &appendix = {});
            int increaseStatisticsValue(const QString &identifier, int value);
            //! @}

            //! Clear the statistics
            void clearStatistics();

            //! Text statistics
            QString getNetworkStatisticsAsText(bool reset, const QString &separator = "\n");

        signals:
            //! Client responses received @{
            void atcDataUpdateReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                                       const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
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
            void rawFsdMessage(const BlackMisc::Network::CRawFsdMessage &rawFsdMessage);
            void planeInformationFsinnReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineIcaoDesignator, const QString &aircraftDesignator, const QString &combinedAircraftType, const QString &modelString);
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

            //! Send FSD message
            template <class T>
            void sendMessage(const T &message)
            {
                if (!message.isValid()) return;

                const QString payload = message.toTokens().join(':');
                const QString line    = message.pdu() + payload;
                const QString buffer  = line + "\r\n";
                const QByteArray bufferEncoded = m_fsdTextCodec->fromUnicode(buffer);
                emitRawFsdMessage(buffer.trimmed(), true);
                if (m_printToConsole) { qDebug() << "FSD Sent=>" << bufferEncoded; }
                if (!m_unitTestMode)  { m_socket.write(bufferEncoded); }
            }

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

            void readDataFromSocket();
            QString socketErrorToQString(QAbstractSocket::SocketError error);
            void parseMessage(const QString &lineRaw);

            //! Init. the message types
            void initializeMessageTypes();

            // Type to string
            const QString &messageTypeToString(MessageType mt) const;

            //! Handle response tokens @{
            void handleAtcDataUpdate(const QStringList &tokens);
            void handleAuthChallenge(const QStringList &tokens);
            void handleAuthResponse(const QStringList &tokens);
            void handleDeleteATC(const QStringList &tokens);
            void handleDeletePilot(const QStringList &tokens);
            void handleTextMessage(const QStringList &tokens);
            void handlePilotDataUpdate(const QStringList &tokens);
            void handlePing(const QStringList &tokens);
            void handlePong(const QStringList &tokens);
            void handleKillRequest(const QStringList &tokens);
            void handleFlightPlan(const QStringList &tokens);
            void handleClientQuery(const QStringList &tokens);
            void handleClientReponse(const QStringList &tokens);
            void handleServerError(const QStringList &tokens);
            void handleCustomPilotPacket(const QStringList &tokens);
            void handleFsdIdentification(const QStringList &tokens);
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
            const BlackMisc::Network::CFsdSetup &getSetupForServer() const;

            //! Handles ATIS replies from non-VATSIM servers. If the conditions are not met,
            //! the message is released as normal text message.
            void maybeHandleAtisReply(const BlackMisc::Aviation::CCallsign &sender, const BlackMisc::Aviation::CCallsign &receiver, const QString &message);

            //! Settings have been changed
            void fsdMessageSettingsChanged();

            //! Emit raw FSD message (mostly for debugging)
            void emitRawFsdMessage(const QString &fsdMessage, bool isSent);

            //! Additional offset time @{
            qint64 getAdditionalOffsetTime() const;
            void setAdditionalOffsetTime(qint64 addOffset);
            //! @}

            //! Save the statistics
            bool saveNetworkStatistics(const QString &server);

            //! Timers @{
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

            // Client data
            QString m_clientName;
            QString m_hostApplication;
            int m_versionMajor     = 0;
            int m_versionMinor     = 0;
            int m_protocolRevision = 0;
            ServerType m_serverType = ServerType::LegacyFsd;
            Capabilities m_capabilities = Capabilities::None;

            vatsim_auth *clientAuth = nullptr;
            vatsim_auth *serverAuth = nullptr;
            QString      m_lastServerAuthChallenge;
            qint64       m_loginSince = -1; //!< when login was triggered
            static constexpr qint64 PendingConnectionTimeoutMs = 7500;

            // User data
            BlackMisc::Network::CServer    m_server;
            BlackMisc::Network::CLoginMode m_loginMode;
            SimType     m_simType     = SimType::Unknown;
            PilotRating m_pilotRating = PilotRating::Unknown;
            AtcRating   m_atcRating   = AtcRating::Unknown;
            QString     m_com1Frequency;

            // Parser
            QHash<QString, MessageType> m_messageTypeMapping;

            QTcpSocket m_socket; //!< used TCP socket

            bool m_unitTestMode   = false;
            bool m_printToConsole = false;

            BlackMisc::Network::CConnectionStatus m_connectionStatus;
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
                QDateTime m_queryTime = QDateTime::currentDateTimeUtc();
                QStringList m_atisMessage;
            };

            QHash<BlackMisc::Aviation::CCallsign, PendingAtisQuery> m_pendingAtisQueries;
            QHash<BlackMisc::Aviation::CCallsign, qint64> m_lastPositionUpdate;
            QHash<BlackMisc::Aviation::CCallsign, QList<qint64>> m_lastOffsetTimes; //!< latest offset first

            BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TRawFsdMessageSetting> m_fsdMessageSetting { this, &CFSDClient::fsdMessageSettingsChanged };
            QFile m_rawFsdMessageLogFile;
            bool m_rawFsdMessagesEnabled   = false;
            bool m_filterPasswordFromLogin = false;

            QTimer m_scheduledConfigUpdate;
            QTimer m_positionUpdateTimer;        //!< sending positions
            QTimer m_interimPositionUpdateTimer; //!< sending interim positions

            qint64 m_additionalOffsetTime = 0;   //!< additional offset time

            bool m_statistics = false;
            QMap <QString, int> m_callStatistics;
            QVector <QPair<qint64, QString>> m_callByTime;

            BlackMisc::Simulation::CSimulatorPluginInfo m_simulatorInfo;        //!< used simulator
            BlackMisc::Aviation::CCallsign              m_ownCallsign;          //!< "buffered callsign", as this must not change when connected
            BlackMisc::Aviation::CCallsign              m_partnerCallsign;      //!< callsign of partner flying in shared cockpit
            BlackMisc::Aviation::CAircraftIcaoCode      m_ownAircraftIcaoCode;  //!< "buffered icao", as this must not change when connected
            BlackMisc::Aviation::CAirlineIcaoCode       m_ownAirlineIcaoCode;   //!< "buffered icao", as this must not change when connected
            QString                                     m_ownLivery;            //!< "buffered livery", as this must not change when connected
            QString                                     m_ownModelString;       //!< "buffered model string", as this must not change when connected
            bool m_sendLiveryString = true;
            bool m_sendMModelString = true;

            QTextCodec *m_fsdTextCodec = nullptr;

            //! An illegal FSD state has been detected
            void handleIllegalFsdState(const QString &message);

            static const int MaxOffseTimes = 6; //!< Max offset times kept
            static int constexpr c_processingIntervalMsec           = 100;  //!< interval for the processing timer
            static int constexpr c_updatePostionIntervalMsec        = 5000; //!< interval for the position update timer (send our position to network)
            static int constexpr c_updateInterimPostionIntervalMsec = 1000; //!< interval for iterim position updates (send our position as interim position)
        };
    } // ns
} // ns

#endif

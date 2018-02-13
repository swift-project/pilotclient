/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_NETWORK_VATLIB_H
#define BLACKCORE_VATSIM_NETWORK_VATLIB_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/network.h"
#include "blackcore/vatsim/vatsimsettings.h"
#include "blackmisc/tokenbucket.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/digestsignal.h"

#include <stdbool.h>
#include <vatlib/vatlib.h>
#include <QByteArray>
#include <QFile>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTextCodec>
#include <functional>

class QCommandLineOption;

namespace BlackMisc
{
    namespace Aviation { class CFlightPlan; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    namespace Vatsim
    {
        //! Implementation of INetwork using the vatlib shim
        class BLACKCORE_EXPORT CNetworkVatlib :
            public INetwork,
            public BlackMisc::Simulation::COwnAircraftAware // network vatlib consumes own aircraft data and sets ICAO/callsign data
        {
            Q_OBJECT

        public:
            //! Log. categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Constructor
            CNetworkVatlib(BlackMisc::Simulation::IOwnAircraftProvider *ownAircraft, QObject *parent = nullptr);

            //! Destructor
            virtual ~CNetworkVatlib();

            //! \copydoc INetwork::getLibraryInfo
            const QString &getLibraryInfo(bool detailed) const override;

            //! \name Network functions
            //! @{
            virtual bool isConnected() const override { return m_status == vatStatusConnected; }
            virtual BlackMisc::Network::CServer getPresetServer() const override { return m_server; }
            virtual bool isPendingConnection() const override { return m_status == vatStatusConnecting; }
            virtual void presetLoginMode(LoginMode mode) override;
            virtual void presetServer(const BlackMisc::Network::CServer &server) override;
            virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void presetIcaoCodes(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft) override;
            virtual void presetSimulatorInfo(const BlackMisc::Simulation::CSimulatorPluginInfo &simInfo) override;
            virtual void initiateConnection() override;
            virtual void terminateConnection() override;
            virtual void sendPing(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendIpQuery() override;
            virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendCustomFsinnQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendCustomFsinnReponse(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void broadcastAircraftConfig(const QJsonObject &config) override;
            virtual void sendAircraftConfigQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            //! @}

            //! \name Text message functions
            //! @{
            virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages) override;
            //! @}

            //! \name ATC functions
            //! @{
            virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;
            virtual void sendFlightPlanQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            //! @}

            //! \name Aircraft functions
            //! @{
            virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void sendUserInfoQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void setInterimPositionReceivers(const BlackMisc::Aviation::CCallsignSet &receivers) override;
            virtual const BlackMisc::Aviation::CCallsignSet &getInterimPositionReceivers() const override;
            //! @}

            //! Arguments to be passed to another swift appplication
            static QStringList vatlibArguments();

            //! \name Weather functions
            //! @{
            virtual void sendMetarQuery(const BlackMisc::Aviation::CAirportIcaoCode &airportIcao) override;
            //! @}

            //! Command line options this library can handle
            static const QList<QCommandLineOption> &getCmdLineOptions();

            //! Offset times basically telling when to expect the next value from network plus some reserve
            //! @{
            static int constexpr c_positionTimeOffsetMsec = 6000;           //!< offset time for received position updates
            static int constexpr c_interimPositionTimeOffsetMsec = 2000;    //!< offset time for received interim position updates
            //! @}

        private:
            static int constexpr c_processingIntervalMsec = 100;            //!< interval for the processing timer
            static int constexpr c_updatePostionIntervalMsec = 5000;        //!< interval for the position update timer (send our position to network)
            static int constexpr c_updateInterimPostionIntervalMsec = 1000; //!< interval for iterim position updates (send our position as interim position)

            static bool getCmdLineClientIdAndKey(int &id, QString &key);

            void replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void replyToConfigQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign);
            void sendIncrementalAircraftConfig();

            //! \name VATLIB callbacks
            //! @{
            static void onConnectionStatusChanged(VatFsdClient *, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbvar);
            static void onTextMessageReceived(VatFsdClient *, const char *from, const char *to, const char *msg, void *cbvar);
            static void onRadioMessageReceived(VatFsdClient *, const char *from, int freqCount, int *freqList, const char *message, void *cbvar);
            static void onControllerDisconnected(VatFsdClient *, const char *callsign, void *cbvar);
            static void onInterimPilotPositionUpdate(VatFsdClient *, const char *sender, const VatInterimPilotPosition *position, void *cbvar);
            static void onAtcPositionUpdate(VatFsdClient *, const char *callsign, const VatAtcPosition *pos, void *cbvar);
            static void onKicked(VatFsdClient *, const char *reason, void *cbvar);
            static void onPong(VatFsdClient *, const char *sender, double elapsedTime, void *cbvar);
            static void onMetarReceived(VatFsdClient *, const char *data, void *cbvar);
            static void onInfoQueryRequestReceived(VatFsdClient *, const char *callsign, VatClientQueryType type, const char *data, void *cbvar);
            static void onInfoQueryReplyReceived(VatFsdClient *, const char *callsign, VatClientQueryType type, const char *data, const char *data2, void *cbvar);
            static void onCapabilitiesReplyReceived(VatFsdClient *, const char *callsign, int capabilityFlags, void *cbvar);
            static void onAtisReplyReceived(VatFsdClient *, const char *callsign, const VatControllerAtis *atis, void *cbvar);
            static void onFlightPlanReceived(VatFsdClient *, const char *callsign, const VatFlightPlan *fp, void *cbvar);
            static void onErrorReceived(VatFsdClient *, VatServerError error, const char *msg, const char *data, void *cbvar);
            static void onPilotDisconnected(VatFsdClient *, const char *callsign, void *cbvar);
            static void onPilotInfoRequestReceived(VatFsdClient *, const char *callsign, void *cbvar);
            static void onPilotInfoReceived(VatFsdClient *, const char *callsign, const VatAircraftInfo *aircraftInfo, void *cbvar);
            static void onPilotPositionUpdate(VatFsdClient *, const char *callsign, const VatPilotPosition *position, void *cbvar);
            static void onAircraftConfigReceived(VatFsdClient *, const char *callsign, const char *aircraftConfig, void *cbvar);
            static void onCustomPacketReceived(VatFsdClient *, const char *callsign, const char *packetId, const char **data, int dataSize, void *cbvar);
            static void onRawFsdMessage(VatFsdClient *, const char *message, void *cbvar);
            //! @}

            QByteArray toFSD(const QString &qstr) const;
            QByteArray toFSD(const BlackMisc::Aviation::CCallsign &callsign) const;
            std::function<const char **()> toFSD(const QStringList &qstrList) const;
            QString fromFSD(const char *cstr) const;
            QStringList fromFSD(const char **cstrArray, int size) const;
            bool isInterimPositionUpdateEnabledForServer() const;
            void startPositionTimers();
            void stopPositionTimers();
            void initializeSession();
            void changeConnectionStatus(VatConnectionStatus newStatus);
            bool isDisconnected() const { return m_status != vatStatusConnecting && m_status != vatStatusConnected; }
            static QString convertToUnicodeEscaped(const QString &str);
            static VatSimType convertToSimType(BlackMisc::Simulation::CSimulatorPluginInfo &simInfo);
            static void networkLogHandler(SeverityLevel severity, const char *context, const char *message);
            void sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

            static const QString &defaultModelString()
            {
                static const QString dm("Cessna Skyhawk 172SP");
                return dm;
            }

            struct JsonPackets
            {
                static const QJsonObject &aircraftConfigRequest();
            };

            void process();
            void sendPositionUpdate();
            void sendInterimPositions();
            void customPacketDispatcher(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);
            void handleRawFsdMessage(const QString &fsdMessage);
            void fsdMessageSettingsChanged();

        signals:
            void terminate(); //!< \private

        private:
            //! Consolidate text messages if we receive multiple messages which belong together
            //! \remark causes a slight delay
            void consolidateTextMessage(const BlackMisc::Network::CTextMessage &textMessage);

            //! Send the consolidatedTextMessages
            void emitConsolidatedTextMessages();

            //! Handles ATIS replies from non-VATSIM servers. If the conditions are not met, the message is
            //! released as normal text message.
            void maybeHandleAtisReply(const BlackMisc::Aviation::CCallsign &sender, const BlackMisc::Aviation::CCallsign &receiver, const QString &message);

            //! Remember when last position was received
            int markReceivedPositionAndGetOffsetTime(const BlackMisc::Aviation::CCallsign &callsign, qint64 markerTs = -1);

            //! Current offset time
            int currentOffsetTime(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Clear state when connection is terminated
            void clearState();

            //! Clear state for callsign
            void clearState(const BlackMisc::Aviation::CCallsign &callsign);

            //! Deletion policy for QScopedPointer
            struct VatFsdClientDeleter
            {
                //! Called by QScopedPointer destructor
                static void cleanup(VatFsdClient *session) { if (session) Vat_DestroyNetworkSession(session); }
            };

            QScopedPointer<VatFsdClient, VatFsdClientDeleter> m_net;
            LoginMode                    m_loginMode;
            VatConnectionStatus          m_status;
            BlackMisc::Network::CServer  m_server;
            QTextCodec                  *m_fsdTextCodec = nullptr;
            BlackMisc::Simulation::CSimulatorPluginInfo m_simulatorInfo;             //!< used simulator
            BlackMisc::Aviation::CCallsign              m_ownCallsign;               //!< "buffered callsign", as this must not change when connected
            BlackMisc::Aviation::CAircraftIcaoCode      m_ownAircraftIcaoCode;       //!< "buffered icao", as this must not change when connected
            BlackMisc::Aviation::CAirlineIcaoCode       m_ownAirlineIcaoCode;        //!< "buffered icao", as this must not change when connected
            QString                                     m_ownLiveryDescription;      //!< "buffered livery", as this must not change when connected
            BlackMisc::Aviation::CCallsignSet           m_interimPositionReceivers;  //!< all aircraft receiving interim positions
            BlackMisc::Aviation::CAircraftParts         m_sentAircraftConfig;        //!< aircraft parts sent
            BlackMisc::CTokenBucket                     m_tokenBucket;               //!< used with aircraft parts messages

            BlackMisc::CDigestSignal m_dsSendTextMessage  { this, &CNetworkVatlib::emitConsolidatedTextMessages, 500, 10 };
            BlackMisc::Network::CTextMessageList m_textMessagesToConsolidate;

            QTimer m_scheduledConfigUpdate;
            QTimer m_processingTimer;
            QTimer m_positionUpdateTimer;
            QTimer m_interimPositionUpdateTimer;

            //! Pending ATIS query since
            struct PendingAtisQuery
            {
                QDateTime m_queryTime = QDateTime::currentDateTimeUtc();
                QStringList m_atisMessage;
            };

            QHash<BlackMisc::Aviation::CCallsign, PendingAtisQuery> m_pendingAtisQueries;
            QHash<BlackMisc::Aviation::CCallsign, qint64> m_lastPositionUpdate;
            QHash<BlackMisc::Aviation::CCallsign, qint64> m_lastOffsetTime;

            BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TRawFsdMessageSetting> m_fsdMessageSetting { this, &CNetworkVatlib::fsdMessageSettingsChanged };
            QFile m_rawFsdMessageLogFile;
            bool m_rawFsdMessagesEnabled = false;
            bool m_filterPasswordFromLogin = false;
        };
    } //namespace
} //namespace
#endif // guard

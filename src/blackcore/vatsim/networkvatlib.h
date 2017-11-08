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

#include <stdbool.h>
#include <vatlib/vatlib.h>
#include <QByteArray>
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
            static QStringList inheritedArguments();

            //! \name Weather functions
            //! @{
            virtual void sendMetarQuery(const BlackMisc::Aviation::CAirportIcaoCode &airportIcao) override;
            //! @}

            //! Command line options this library can handle
            static const QList<QCommandLineOption> &getCmdLineOptions();

            static int const c_positionTimeOffsetMsec = 6000;           //!< offset time for received position updates
            static int const c_interimPositionTimeOffsetMsec = 2000;    //!< offset time for received interim position updates

        private:
            static bool getCmdLineClientIdAndKey(int &id, QString &key);

            void replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void replyToConfigQuery(const BlackMisc::Aviation::CCallsign &callsign);
            void sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign);
            void sendIncrementalAircraftConfig();

            //! \name Shimlib callbacks
            //! @{
            static void onConnectionStatusChanged(VatSessionID, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbvar);
            static void onTextMessageReceived(VatSessionID, const char *from, const char *to, const char *msg, void *cbvar);
            static void onRadioMessageReceived(VatSessionID, const char *from, int freqCount, int *freqList, const char *message, void *cbvar);
            static void onControllerDisconnected(VatSessionID, const char *callsign, void *cbvar);
            static void onInterimPilotPositionUpdate(VatSessionID, const char *sender, const VatInterimPilotPosition *position, void *cbvar);
            static void onAtcPositionUpdate(VatSessionID, const char *callsign, const VatAtcPosition *pos, void *cbvar);
            static void onKicked(VatSessionID, const char *reason, void *cbvar);
            static void onPong(VatSessionID, const char *sender, double elapsedTime, void *cbvar);
            static void onMetarReceived(VatSessionID, const char *data, void *cbvar);
            static void onInfoQueryRequestReceived(VatSessionID, const char *callsign, VatClientQueryType type, const char *data, void *cbvar);
            static void onInfoQueryReplyReceived(VatSessionID, const char *callsign, VatClientQueryType type, const char *data, const char *data2, void *cbvar);
            static void onCapabilitiesReplyReceived(VatSessionID, const char *callsign, int capabilityFlags, void *cbvar);
            static void onAtisReplyReceived(VatSessionID, const char *callsign, const VatControllerAtis *atis, void *cbvar);
            static void onFlightPlanReceived(VatSessionID, const char *callsign, const VatFlightPlan *fp, void *cbvar);
            static void onErrorReceived(VatSessionID, VatServerError error, const char *msg, const char *data, void *cbvar);
            static void onPilotDisconnected(VatSessionID, const char *callsign, void *cbvar);
            static void onPilotInfoRequestReceived(VatSessionID, const char *callsign, void *cbvar);
            static void onPilotInfoReceived(VatSessionID, const char *callsign, const VatAircraftInfo *aircraftInfo, void *cbvar);
            static void onPilotPositionUpdate(VatSessionID, const char *callsign, const VatPilotPosition *position, void *cbvar);
            static void onAircraftConfigReceived(VatSessionID, const char *callsign, const char *aircraftConfig, void *cbvar);
            static void onCustomPacketReceived(VatSessionID, const char *callsign, const char *packetId, const char **data, int dataSize, void *cbvar);
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
            static void networkLogHandler(SeverityLevel severity, const char *message);
            void sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

            inline QString defaultModelString()
            {
                return QStringLiteral("Cessna Skyhawk 172SP");
            }

            struct JsonPackets
            {
                static QJsonObject aircraftConfigRequest();
            };

            void process();
            void sendPositionUpdate();
            void sendInterimPositions();
            void customPacketDispatcher(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

        signals:
            void terminate(); //!< \private

        private:
            //! Deletion policy for QScopedPointer
            struct VatlibQScopedPointerDeleter
            {
                //! Called by QScopedPointer destructor
                static void cleanup(VatSessionID session) { if (session) Vat_DestroyNetworkSession(session); }
            };

            QScopedPointer<PCSBClient, VatlibQScopedPointerDeleter> m_net;
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

            QTimer m_scheduledConfigUpdate;
            QTimer m_processingTimer;
            QTimer m_positionUpdateTimer;
            QTimer m_interimPositionUpdateTimer;

            static int const c_processingIntervalMsec = 100;            //!< interval for the processing timer
            static int const c_updatePostionIntervalMsec = 5000;        //!< interval for the position update timer (send our position to network)
            static int const c_updateInterimPostionIntervalMsec = 1000; //!< interval for iterim position updates (send our position as interim position)
        };
    } //namespace
} //namespace
#endif // guard

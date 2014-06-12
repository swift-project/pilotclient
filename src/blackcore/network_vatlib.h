/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_NETWORK_VATLIB_H
#define BLACKCORE_NETWORK_VATLIB_H

#include "network.h"
#include "blackmisc/avallclasses.h"
#include <vatlib/vatlib.h>
#include <QScopedPointer>
#include <QTimer>
#include <QTextCodec>
#include <QByteArray>
#include <QMap>

namespace BlackCore
{
    /*!
     * \brief Implementation of INetwork using the vatlib shim
     */
    class CNetworkVatlib : public INetwork
    {
        Q_OBJECT

    public:
        //! \brief Constructor
        CNetworkVatlib(QObject *parent = nullptr);

        //! \brief Destructor
        virtual ~CNetworkVatlib();

        //! \copydoc INetwork::isConnected()
        virtual bool isConnected() const override { return m_status == Cvatlib_Network::connStatus_Connected; }

        //! \copydoc INetwork::getStatusUrls()
        virtual QList<QUrl> getStatusUrls() const override;

        //! \copydoc INetwork::getKnownServers()
        virtual BlackMisc::Network::CServerList getKnownServers() const override;

        // Network slots
        virtual void presetServer(const BlackMisc::Network::CServer &server) override;
        virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao) override;
        virtual void presetLoginMode(LoginMode mode) override;
        virtual void initiateConnection() override;
        virtual void terminateConnection() override;
        virtual void sendPing(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendRealNameQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendIpQuery() override;
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data) override;
        virtual void sendFsipiCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineDesignator,
            const QString &aircraftDesignator, const QString &combinedType, const QString &modelString) override;
        virtual void sendFsipirCustomPacket(const BlackMisc::Aviation::CCallsign &callsign, const QString &airlineDesignator,
            const QString &aircraftDesignator, const QString &combinedType, const QString &modelString) override;

        // Text message slots
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages) override;

        // ATC slots
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;
        virtual void sendFlightPlanQuery(const BlackMisc::Aviation::CCallsign &callsign) override;

        // Aircraft slots
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendIcaoCodesQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void sendUserInfoQuery(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) override;
        virtual void setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;
        virtual void setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;
        virtual void setOwnCockpit(const BlackMisc::Aviation::CComSystem &com1,
                                            const BlackMisc::Aviation::CComSystem &com2,
                                            const BlackMisc::Aviation::CTransponder &xpdr) override;

        // Weather slots
        virtual void sendMetarQuery(const BlackMisc::Aviation::CAirportIcao &airportIcao) override;
        virtual void sendWeatherDataQuery(const BlackMisc::Aviation::CAirportIcao &airportIcao) override;

    private slots:
        void replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign);

    private: //shimlib callbacks
        static void onConnectionStatusChanged(Cvatlib_Network *, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void *cbvar);
        static void onTextMessageReceived(Cvatlib_Network *, const char *from, const char *to, const char *msg, void *cbvar);
        static void onRadioMessageReceived(Cvatlib_Network *, const char *from, INT numFreq, INT *freqList, const char *msg, void *cbvar);
        static void onControllerDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onInterimPilotPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::PilotPosUpdate pos, void *cbvar);
        static void onAtcPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::ATCPosUpdate pos, void *cbvar);
        static void onKicked(Cvatlib_Network *, const char *reason, void *cbvar);
        static void onPong(Cvatlib_Network *, const char *callsign, INT elapsedTime, void *cbvar);
        static void onMetarReceived(Cvatlib_Network *, const char *data, void *cbvar);
        static void onInfoQueryRequestReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::infoQuery type, const char *data, void *cbvar);
        static void onInfoQueryReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::infoQuery type, const char *data, const char *data2, void *cbvar);
        static void onCapabilitiesReplyReceived(Cvatlib_Network *, const char *callsign, const char **keysValues, void *cbvar);
        static void onAtisReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::atisLineType type, const char *data, void *cbvar);
        static void onFlightPlanReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::FlightPlan fp, void *cbvar);
        static void onTemperatureDataReceived(Cvatlib_Network *, Cvatlib_Network::TempLayer layers[4], INT pressure, void *cbvar);
        static void onErrorReceived(Cvatlib_Network *, Cvatlib_Network::error type, const char *msg, const char *data, void *cbvar);
        static void onWindDataReceived(Cvatlib_Network *, Cvatlib_Network::WindLayer layers[4], void *cbvar);
        static void onCloudDataReceived(Cvatlib_Network *, Cvatlib_Network::CloudLayer layers[2], Cvatlib_Network::StormLayer storm, float vis, void *cbvar);
        static void onPilotDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoRequestReceived(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoReceived(Cvatlib_Network *, const char *callsign, const char **keysValues, void *cbvar);
        static void onPilotPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::PilotPosUpdate pos, void *cbvar);
        static void onCustomPacketReceived(Cvatlib_Network *, const char *callsign, const char *packetId, const char **data, INT dataSize, void *cbvar);

    private:
        QByteArray toFSD(QString qstr) const;
        QByteArray toFSD(const BlackMisc::Aviation::CCallsign &callsign) const;
        std::function<const char **()> toFSD(QStringList qstrList) const;
        QString fromFSD(const char *cstr) const;
        QStringList fromFSD(const char **cstrArray, int size) const;

        void initializeSession();
        void changeConnectionStatus(Cvatlib_Network::connStatus newStatus, QString errorMessage = "");
        bool isDisconnected() const { return m_status != Cvatlib_Network::connStatus_Connecting && m_status != Cvatlib_Network::connStatus_Connected; }
        QString getSocketError() const;

    private slots:
        void process();
        void update();
        void customPacketDispatcher(const BlackMisc::Aviation::CCallsign &callsign, const QString &packetId, const QStringList &data);

    signals:
        void terminate(); //!< \private

    public:
        //! Deletion policy for QScopedPointer
        struct VatlibQScopedPointerDeleter
        {
            //! Called by QScopedPointer destructor
            static void cleanup(Cvatlib_Network *net) { if (net) net->Destroy(); }
        };

    private:
        QScopedPointer<Cvatlib_Network, VatlibQScopedPointerDeleter> m_net;
        LoginMode m_loginMode;
        Cvatlib_Network::connStatus m_status;
        BlackMisc::Network::CServer m_server;
        BlackMisc::Aviation::CCallsign m_callsign;
        BlackMisc::Aviation::CAircraftIcao m_icaoCode;
        BlackMisc::Aviation::CAircraft m_ownAircraft; // not using callsign, user, or icao parts of this member because they can't be changed when connected
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CInformationMessage> m_atisParts;

        QTimer m_processingTimer;
        QTimer m_updateTimer;
        static int const c_processingIntervalMsec = 100;
        static int const c_updateIntervalMsec = 5000;
        static int const c_logoffTimeoutSec = 5;
        QTextCodec *m_fsdTextCodec;
    };

} //namespace BlackCore

#endif // guard

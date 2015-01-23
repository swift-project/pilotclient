/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_NETWORK_VATLIB_H
#define BLACKCORE_NETWORK_VATLIB_H

#include "network.h"
#include <vatlib/vatlib2.h>
#include <QScopedPointer>
#include <QTimer>
#include <QTextCodec>
#include <QByteArray>
#include <QMap>

namespace BlackCore
{
    /*!
     * Implementation of INetwork using the vatlib shim
     */
    class CNetworkVatlib : public INetwork
    {
        Q_OBJECT

    public:
        //! Constructor
        CNetworkVatlib(QObject *parent = nullptr);

        //! Destructor
        virtual ~CNetworkVatlib();

        //! \copydoc INetwork::isConnected()
        virtual bool isConnected() const override { return m_status == vatStatusConnected; }

        //! \copydoc INetwork::pendingConnection()
        virtual bool isPendingConnection() const override { return m_status == vatStatusConnecting; }

        // Network slots
        //! \copydoc INetwork::presetLoginMode
        virtual void presetLoginMode(LoginMode mode) override;
        virtual void presetServer(const BlackMisc::Network::CServer &server) override;
        virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;
        virtual void presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao) override;
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
        static void onConnectionStatusChanged(VatSessionID, VatConnectionStatus oldStatus, VatConnectionStatus newStatus, void *cbvar);
        static void onTextMessageReceived(VatSessionID, const char *from, const char *to, const char *msg, void *cbvar);

        static void onRadioMessageReceived(VatSessionID, const char *from, int freqCount, int *freqList, const char *message, void *cbvar);

        static void onControllerDisconnected(VatSessionID, const char *callsign, void *cbvar);
        static void onInterimPilotPositionUpdate(VatSessionID, const char *callsign, const VatPilotPosition *position, void *cbvar);
        static void onAtcPositionUpdate(VatSessionID, const char *callsign, const VatAtcPosition *pos, void *cbvar);
        static void onKicked(VatSessionID, const char *reason, void *cbvar);
        static void onPong(VatSessionID, const char *sender, double elapsedTime, void *cbvar);
        static void onMetarReceived(VatSessionID, const char *data, void *cbvar);
        static void onInfoQueryRequestReceived(VatSessionID, const char *callsign, VatInfoQueryType type, const char *data, void *cbvar);
        static void onInfoQueryReplyReceived(VatSessionID, const char *callsign, VatInfoQueryType type, const char *data, const char *data2, void *cbvar);
        static void onCapabilitiesReplyReceived(VatSessionID, const char *callsign, int capabilityFlags, void *cbvar);
        static void onAtisReplyReceived(VatSessionID, const char *callsign, const VatControllerAtis *atis, void *cbvar);
        static void onFlightPlanReceived(VatSessionID, const char *callsign, const VatFlightPlan *fp, void *cbvar);
        static void onTemperatureDataReceived(VatSessionID, const VatTempLayer layer[4], int pressure, void *cbvar);
        static void onWindDataReceived(VatSessionID, const VatWindLayer layer[4], void *cbvar);
        static void onCloudDataReceived(VatSessionID, const VatCloudLayer cloudLayer[2], VatThunderStormLayer thunderStormLayer, float visibility, void *cbvar);
        static void onErrorReceived(VatSessionID, VatServerError error, const char *msg, const char *data, void *cbvar);
        static void onPilotDisconnected(VatSessionID, const char *callsign, void *cbvar);
        static void onPilotInfoRequestReceived(VatSessionID, const char *callsign, void *cbvar);
        static void onPilotInfoReceived(VatSessionID, const char *callsign, const VatAircraftInfo *aircraftInfo, void *cbvar);
        static void onPilotPositionUpdate(VatSessionID, const char *callsign, const VatPilotPosition *position, void *cbvar);
        static void onCustomPacketReceived(VatSessionID, const char *callsign, const char *packetId, const char **data, int dataSize, void *cbvar);

    private:
        QByteArray toFSD(QString qstr) const;
        QByteArray toFSD(const BlackMisc::Aviation::CCallsign &callsign) const;
        std::function<const char **()> toFSD(QStringList qstrList) const;
        QString fromFSD(const char *cstr) const;
        QStringList fromFSD(const char **cstrArray, int size) const;

        void initializeSession();
        void changeConnectionStatus(VatConnectionStatus newStatus);
        bool isDisconnected() const { return m_status != vatStatusConnecting && m_status != vatStatusConnected; }
        static void networkErrorHandler(const char *message);

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
            static void cleanup(VatSessionID session) { if (session) Vat_DestroyNetworkSession(session); }
        };

    private:
        QScopedPointer<PCSBClient, VatlibQScopedPointerDeleter> m_net;
        LoginMode m_loginMode;
        VatConnectionStatus m_status;
        BlackMisc::Network::CServer m_server;
        BlackMisc::Aviation::CCallsign m_callsign;
        BlackMisc::Aviation::CAircraftIcao m_icaoCode;
        BlackMisc::Aviation::CAircraft m_ownAircraft; // not using callsign, user, or icao parts of this member because they can't be changed when connected

        QTimer m_processingTimer;
        QTimer m_updateTimer;
        static int const c_processingIntervalMsec = 100;
        static int const c_updateIntervalMsec = 5000;
        static int const c_logoffTimeoutSec = 5;
        QTextCodec *m_fsdTextCodec;
    };

} //namespace BlackCore

#endif // guard

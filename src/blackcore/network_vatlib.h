/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_NETWORK_VATLIB_H
#define BLACKCORE_NETWORK_VATLIB_H

#include "network.h"
#include "blackmisc/avallclasses.h"
#include <vatlib/vatlib.h>
#include <QScopedPointer>
#include <QTimer>
#include <QTextCodec>
#include <QByteArray>

namespace BlackCore
{

    class CNetworkVatlib : public INetwork
    {
        Q_OBJECT

    public:
        CNetworkVatlib(QObject *parent = nullptr);
        virtual ~CNetworkVatlib();

    public: // INetwork slots overrides

        // Network
        virtual void setServer(const BlackMisc::Network::CServer &server);
        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void setRealName(const QString &name);
        virtual void initiateConnection();
        virtual void terminateConnection();
        virtual void sendIpQuery();
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void ping(const BlackMisc::Aviation::CCallsign &callsign);

        // Weather
        virtual void requestWeatherData(const QString &airportICAO);

        // Text messages
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages);

        // Aircraft
        virtual void requestAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void setOwnAircraftPosition(const BlackMisc::Aviation::CAircraftSituation &aircraft);
        virtual void setOwnAircraftTransponder(const BlackMisc::Aviation::CTransponder &xpdr);
        virtual void setOwnAircraftFrequency(const BlackMisc::PhysicalQuantities::CFrequency &freq);
        virtual void setOwnAircraftIcao(const BlackMisc::Aviation::CAircraftIcao &icao);

        // ATC
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void requestMetar(const QString &airportICAO);

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
        static void onTemperatureDataReceived(Cvatlib_Network *, Cvatlib_Network::TempLayer layers[4], INT pressure, void *cbvar);
        static void onErrorReceived(Cvatlib_Network *, Cvatlib_Network::error type, const char *msg, const char *data, void *cbvar);
        static void onWindDataReceived(Cvatlib_Network *, Cvatlib_Network::WindLayer layers[4], void *cbvar);
        static void onCloudDataReceived(Cvatlib_Network *, Cvatlib_Network::CloudLayer layers[2], Cvatlib_Network::StormLayer storm, float vis, void *cbvar);
        static void onPilotDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoRequestReceived(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoReceived(Cvatlib_Network *, const char *callsign, const char **keysValues, void *cbvar);
        static void onPilotPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::PilotPosUpdate pos, void *cbvar);

    private:
        QByteArray toFSD(QString qstr) const;
        QByteArray toFSD(const BlackMisc::Aviation::CCallsign &callsign) const;
        QString fromFSD(const char *cstr) const;
        bool isDisconnected() const { return m_status == Cvatlib_Network::connStatus_Idle || m_status == Cvatlib_Network::connStatus_Disconnected; }

    private slots:
        void process();
        void update();

    signals:
        void terminate();

    public:
        //! Deletion policy for QScopedPointer
        struct VatlibQScopedPointerDeleter
        {
            //! Called by QScopedPointer destructor
            static void cleanup(Cvatlib_Network *net) { if (net) net->Destroy(); }
        };

    private:
        QScopedPointer<Cvatlib_Network, VatlibQScopedPointerDeleter> m_net;
        Cvatlib_Network::connStatus m_status;
        BlackMisc::Network::CServer m_server;

        QTimer m_processingTimer;
        QTimer m_updateTimer;
        static int const c_processingIntervalMsec = 100;
        static int const c_updateIntervalMsec = 5000;
        static int const c_logoffTimeoutSec = 5;

        QByteArray m_callsign;
        QByteArray m_realname;
        BlackMisc::Aviation::CAircraftSituation m_ownAircraft;
        BlackMisc::Aviation::CTransponder m_ownTransponder;
        BlackMisc::PhysicalQuantities::CFrequency m_ownFrequency;
        BlackMisc::Aviation::CAircraftIcao m_ownAircraftIcao;

        QTextCodec *m_fsdTextCodec;
    };

} //namespace BlackCore

#endif // guard

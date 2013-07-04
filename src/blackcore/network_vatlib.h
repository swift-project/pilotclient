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
#include "../../vatlib/vatlib.h"
#include <QScopedPointer>
#include <QBasicTimer>

namespace BlackCore
{

    class NetworkVatlib : public INetwork
    {
        Q_OBJECT;

    public:
        NetworkVatlib();
        virtual ~NetworkVatlib();

    protected: //QObject overrides
        virtual void timerEvent(QTimerEvent*);

    public: //INetwork slots overrides
        virtual void setServerDetails(const QString& hostname, quint16 port);
        virtual void setUserCredentials(const QString& username, const QString& password);
        virtual void initiateConnection();
        virtual void terminateConnection();
        virtual void sendPrivateTextMessage(const QString& callsign, const QString& msg);
        virtual void sendRadioTextMessage(const QVector<BlackMisc::PhysicalQuantities::CFrequency>& freqs, const QString& msg);
        virtual void requestPlaneInfo(const QString& callsign);
        virtual void sendPlaneInfo(const QString& callsign, const QString& acTypeICAO, const QString& airlineICAO, const QString& livery);
        virtual void ping(const QString& callsign);
        virtual void requestMetar(const QString& airportICAO);
        virtual void requestWeatherData(const QString& airportICAO);

    private: //shimlib callbacks
        static void onConnectionStatusChanged(Cvatlib_Network*, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void* cbvar);
        static void onTextMessageReceived(Cvatlib_Network*, const char* from, const char* to, const char* msg, void* cbvar);
        static void onRadioMessageReceived(Cvatlib_Network*, const char* from, INT numFreq, INT* freqList, const char* msg, void* cbvar);
        static void onPilotDisconnected(Cvatlib_Network*, const char* callsign, void* cbvar);
        static void onControllerDisconnected(Cvatlib_Network*, const char* callsign, void* cbvar);
        static void onPilotPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::PilotPosUpdate pos, void* cbvar);
        static void onInterimPilotPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::PilotPosUpdate pos, void* cbvar);
        static void onAtcPositionUpdate(Cvatlib_Network*, const char* callsign, Cvatlib_Network::ATCPosUpdate pos, void* cbvar);
        static void onKicked(Cvatlib_Network*, const char* reason, void* cbvar);
        static void onPong(Cvatlib_Network*, const char* callsign, INT elapsedTime, void* cbvar);
        static void onMetarReceived(Cvatlib_Network*, const char* data, void* cbvar);
        static void onInfoQueryRequestReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::infoQuery type, const char* data, void* cbvar);
        static void onInfoQueryReplyReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::infoQuery type, const char* data, const char* data2, void* cbvar);
        static void onCapabilitiesReplyReceived(Cvatlib_Network*, const char* callsign, const char** keysValues, void* cbvar);
        static void onAtisReplyReceived(Cvatlib_Network*, const char* callsign, Cvatlib_Network::atisLineType type, const char* data, void* cbvar);
        static void onTemperatureDataReceived(Cvatlib_Network*, Cvatlib_Network::TempLayer layers[4], INT pressure, void* cbvar);
        static void onErrorReceived(Cvatlib_Network*, Cvatlib_Network::error type, const char* msg, const char* data, void* cbvar);
        static void onWindDataReceived(Cvatlib_Network*, Cvatlib_Network::WindLayer layers[4], void* cbvar);
        static void onCloudDataReceived(Cvatlib_Network*, Cvatlib_Network::CloudLayer layers[2], Cvatlib_Network::StormLayer storm, float vis, void* cbvar);
        static void onPilotInfoRequestReceived(Cvatlib_Network*, const char* callsign, void* cbvar);
        static void onPilotInfoReceived(Cvatlib_Network*, const char* callsign, const char** keysValues, void* cbvar);

    private:
        void exceptionDispatcher();

    private:
        QScopedPointer<Cvatlib_Network> m_net;

        QBasicTimer m_timer;
        static int const c_updateIntervalMillisecs = 100;
        static int const c_logoffTimeoutSeconds = 5;

        QString m_serverHost;
        quint16 m_serverPort;
        QString m_username;
        QString m_password;
    };

} //namespace BlackCore

#endif //BLACKCORE_NETWORK_VATLIB_H

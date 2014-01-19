/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_NETWORK_H
#define BLACKCORE_NETWORK_H

#include "blackmisc/avaircraft.h"
#include "blackmisc/pqfrequency.h"
#include "blackmisc/coordinategeodetic.h"
#include "blackmisc/pqlength.h"
#include "blackmisc/pqtime.h"
#include "blackmisc/nwserverlist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avinformationmessage.h"
#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QMetaEnum>

namespace BlackCore
{

    /*!
     * Interface for a connection to a multi-user flight simulation and ATC network.
     *
     * The connection can be in one of three essential states: disconnected, connecting, and
     * connected. (There is a fourth state, disconnected due to error, which is a substate of
     * disconnected.) Some slots may only be called when connected, and some may only be called
     * when disconnected; there is a naming convention to highlight this fact using prefixes:
     * "preset" slots are only callable when disconnected, "send" slots are only callable when
     * connected, and "set" slots are callable in any state.
     *
     * Slots with the word "query" in their names are handled asynchronously, with one or more
     * "reply" signals being sent in response to each invokation of a query slot.
     *
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     */
    class INetwork : public QObject
    {
        Q_OBJECT
        Q_ENUMS(ConnectionStatus)

    protected:
        /*!
         * \brief Constructor
         * \param parent
         */
        INetwork(QObject *parent = nullptr) : QObject(parent) {}

    public:
        virtual ~INetwork() {}

        enum
        {
            AcceptsAtisResponses        = 1 << 0,
            SupportsInterimPosUpdates   = 1 << 1,
            SupportsModelDescriptions   = 1 << 2
        };

        enum LoginMode
        {
            LoginNormal = 0,
            LoginAsObserver,
            LoginStealth
        };

        enum ConnectionStatus
        {
            Disconnected = 0,
            DisconnectedError,
            Connecting,
            Connected
        };

        static QString connectionStatusToString(ConnectionStatus status)
        {
            int index = staticMetaObject.indexOfEnumerator("ConnectionStatus");
            QMetaEnum metaEnum = staticMetaObject.enumerator(index);
            return metaEnum.valueToKey(status);
        }

        static bool isErrorStatus(ConnectionStatus status)
        {
            return status == DisconnectedError;
        }

        virtual bool isConnected() const = 0;

    public slots:
        // Network
        virtual void presetServer(const BlackMisc::Network::CServer &server) = 0;
        virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao) = 0;
        virtual void presetLoginMode(LoginMode mode) = 0;
        virtual void initiateConnection() = 0;
        virtual void terminateConnection() = 0;
        virtual void sendPing(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        virtual void sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendIpQuery() = 0;
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        // Text messages
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages) = 0;

        // ATC
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        // Aircraft
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendAircraftInfoQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) = 0;
        virtual void setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) = 0;
        virtual void setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;
        virtual void setOwnAircraftAvionics(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
                                            const BlackMisc::Aviation::CTransponder &transponder) = 0;

        // Weather / flight plan
        virtual void sendMetarQuery(const QString &airportICAO) = 0;
        virtual void sendWeatherDataQuery(const QString &airportICAO) = 0;
        // TODO virtual void sendFlightPlan(...) = 0;

    signals:
        // ATC
        void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                               const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
        void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void atcReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);
        void atisReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);
        void atisVoiceRoomReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void atisLogoffTimeReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void metarReplyReceived(const QString &data);

        // Aircraft
        void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void aircraftInfoReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icao);
        void aircraftPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation,
                                    const BlackMisc::Aviation::CTransponder &transponder);
        // TODO void aircraftInterimPositionUpdate(...);
        void frequencyReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);

        // Connection / Network in general
        void statusMessage(const BlackMisc::CStatusMessage &message);
        void kicked(const QString &msg);
        void connectionStatusChanged(ConnectionStatus oldStatus, ConnectionStatus newStatus);
        void pongReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);
        void capabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void ipReplyReceived(const QString &ip);
        void serverReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname);
        void nameReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        // Text messages
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);

        // Weather
        // TODO void temperatureDataReplyReceived(...);
        // TODO void windDataReplyReceived(...);
        // TODO void cloudDataReplyReceived(...);
    };

} // namespace

#endif // guard

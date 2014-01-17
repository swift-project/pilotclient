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
     * \warning If an INetwork signal is connected to a slot, and that slot emits a signal
     *          which is connected to an INetwork slot, then at least one of those connections
     *          must be a Qt::QueuedConnection.
     */
    class INetwork : public QObject
    {
        Q_OBJECT

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

        enum ConnectionStatus
        {
            Disconnected = 0,
            DisconnectedError,
            Connecting,
            Connected
        };

        static const QString connectionStatusToString(ConnectionStatus status)
        {
            // the version with metaObject, metaObject()->indexOfEnumerator does not work anymore
            // an interface cannot be used with Q_DECLAREMETATYPE
            switch (status)
            {
            case Disconnected: return "disconnected";
            case DisconnectedError: return "disconnectedError";
            case Connecting: return "connecting";
            case Connected: return "connected";
            default: break;
            }
            qFatal("Missing value");
            return ""; // just for compiler warning
        }

        virtual bool isConnected() const = 0;

    public slots:
        // Network
        virtual void setServer(const BlackMisc::Network::CServer &server) = 0;
        virtual void setCallsign(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void setRealName(const QString &name) = 0;
        virtual void initiateConnection() = 0;
        virtual void terminateConnection() = 0;
        virtual void ping(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        virtual void sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendIpQuery() = 0;
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        // Text messages
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        // ATC
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;

        // Aircraft
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void requestAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign) = 0;
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) = 0;
        virtual void setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) = 0;
        virtual void setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;
        virtual void setOwnAircraftAvionics(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
                                            const BlackMisc::Aviation::CTransponder &transponder) = 0;

        // Weather / flight plan
        virtual void requestMetar(const QString &airportICAO) = 0;
        virtual void requestWeatherData(const QString &airportICAO) = 0;
        // TODO virtual void sendFlightPlan(...) = 0;

    signals:
        // ATC
        void atcPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
                               const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range);
        void atcDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void atcQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, bool isATC);
        void atisQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CInformationMessage &atis);
        void atisQueryVoiceRoomReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &url);
        void atisQueryLogoffTimeReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &zuluTime);
        void metarReceived(const QString &data);

        // Aircraft
        void pilotDisconnected(const BlackMisc::Aviation::CCallsign &callsign);
        void aircraftInfoReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftIcao &icao);
        void aircraftPositionUpdate(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation,
                                    const BlackMisc::Aviation::CTransponder &transponder);
        // TODO void aircraftInterimPositionUpdate(...);
        void frequencyQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq);

        // Connection / Network in general
        void statusMessage(const BlackMisc::CStatusMessage &message);
        void kicked(const QString &msg);
        void connectionStatusChanged(ConnectionStatus oldStatus, ConnectionStatus newStatus);
        void pong(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::PhysicalQuantities::CTime &elapsedTime);
        void capabilitiesQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags);
        void ipQueryReplyReceived(const QString &ip);
        void serverQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &hostname);
        void nameQueryReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, const QString &realname);

        // Text messages
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &messages);

        // Weather
        // TODO void temperatureDataReceived(...);
        // TODO void windDataReceived(...);
        // TODO void cloudDataReceived(...);
    };

} // namespace

#endif // guard

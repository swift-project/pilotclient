/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_INTERFACE_H
#define BLACKCORE_CONTEXTNETWORK_INTERFACE_H

#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessages.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackcore/network_vatlib.h"
#include <QObject>
#include <QDBusAbstractInterface>

#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "blackcore.contextnetwork"
#define BLACKCORE_CONTEXTNETWORK_SERVICEPATH "/network"

// SERVICENAME must contain at least one ".", otherwise generation fails
// as this is interpreted in the way comain.somename

namespace BlackCore
{

    /*!
     * \brief The IContextNetwork class
     */
    class IContextNetwork : public QObject
    {
        Q_OBJECT
        Q_ENUMS(ConnectionStatus)
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

    public:
        /*!
         * \brief Service name
         * \return
         */
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_INTERFACENAME);
            return s;
        }

        /*!
         * \brief Service path
         * \return
         */
        static const QString &ServicePath()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_SERVICEPATH);
            return s;
        }

        /*!
         * \brief Qt compliant status
         */
        enum ConnectionStatus
        {
            ConnectionStatusIdle = static_cast<uint>(Cvatlib_Network::connStatus_Idle),
            ConnectionStatusConnected = static_cast<uint>(Cvatlib_Network::connStatus_Connected),
            ConnectionStatusConnecting = static_cast<uint>(Cvatlib_Network::connStatus_Connecting),
            ConnectionStatusDisconnected = static_cast<uint>(Cvatlib_Network::connStatus_Disconnected),
            ConnectionStatusError = static_cast<uint>(Cvatlib_Network::connStatus_Error)
        };

        /*!
         * \brief DBus version constructor
         * \param serviceName
         * \param connection
         * \param parent
         */
        IContextNetwork(const QString &serviceName, QDBusConnection &connection, QObject *parent = 0);

        /*!
         * Destructor
         */
        ~IContextNetwork() {}


        /*!
         * \brief Using local objects?
         * \return
         */
        virtual bool usingLocalObjects() const { return false; }

    private:
        BlackMisc::CGenericDBusInterface *m_dBusInterface;

        /*!
         * Relay connection signals to local signals
         * No idea why this has to be wired and is not done automatically
         * \param connection
         */
        void relaySignals(const QString &serviceName, QDBusConnection &connection);

    protected:
        /*!
         * \brief IContextNetwork
         * \param parent
         */
        IContextNetwork(QObject *parent = 0) : QObject(parent), m_dBusInterface(0) {}

        /*!
         * Connection status as cleartext
         */
        QString connectionStatusToString(ConnectionStatus status) const;

        /*!
         * \brief Helper for logging, likely to be removed / changed
         * \param method
         * \param m1
         * \param m2
         * \param m3
         * \param m4
         */
        void log(const QString &method, const QString &m1 = "", const QString &m2 = "", const QString &m3 = "", const QString &m4 = "") const;

    signals:

        /*!
         * \brief ATC station list has been changed
         * \param message
         */
        void statusMessage(const BlackMisc::CStatusMessage &message);

        /*!
         * \brief List has been changed
         */
        void changedAtcStationsOnline();

        /*!
         * \brief ATC station list has been changed
         */
        void changedAtcStationsBooked();

        /*!
         * \brief Aircraft list has been changed
         */
        void changedAircraftsInRange();

        /*!
         * \brief Terminated connection
         */
        void connectionTerminated();

        /*!
         * \brief Connection status changed
         * \param from
         * \param to
         */
        // If I use the enum, adaptor / interface are not created correctly
        void connectionStatusChanged(uint from, uint to);

        /*!
         * \brief Text messages (also private chat messages)
         * \param textMessage
         */
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    public slots:

        /*!
         * \brief Read ATC bookings
         * \return
         */
        virtual void readAtcBookingsFromSource() const;

        /*!
         * \brief The "central" ATC list with online ATC controllers
         * \return
         */
        // If I make this &getAtcStations XML is not generated correctly
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const;

        /*!
         * \brief ATC list, with booked controllers
         * \return
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const;

        /*!
         * \brief Aircraft list
         * \return
         */
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const;

        /*!
         * \brief Get own aircraft
         * \return
         */
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        /*!
         * \brief Connect to Network
         * \return
         */
        virtual BlackMisc::CStatusMessages connectToNetwork();

        /*!
         * \brief Disconnect from network
         * \return
         */
        virtual BlackMisc::CStatusMessages disconnectFromNetwork();

        /*!
         * \brief Network connected?
         * \return
         */
        virtual bool isConnected() const;

        /*!
         * Set own aircraft
         * \param aircraft
         * \return
         */
        virtual BlackMisc::CStatusMessages setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        /*!
         * \brief Own position, be aware height is terrain height
         * \param Position
         * \param altitude
         * \return
         */
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \brief Complete situation update
         * \param Situation
         * \return
         */
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        /*!
         * \brief Update own cockpit
         * \param com1
         * \param com2
         * \param transponder
         */
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * \brief Text messages (radio and private chat messages)
         * \param textMessage
         */
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);

        /*!
         * \brief Get METAR, if not available request it
         * \param airportIcaoCode
         * \return
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode);

        /*!
         * \brief Ping
         * \param token
         * \return
         */
        virtual qint64 ping(qint64 token) const;

    };
}

#endif // guard

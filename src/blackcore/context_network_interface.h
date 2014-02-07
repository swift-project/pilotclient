/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_INTERFACE_H
#define BLACKCORE_CONTEXTNETWORK_INTERFACE_H

#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/genericdbusinterface.h"
#include "blackcore/network_vatlib.h"
#include "blackmisc/vvoiceroomlist.h"
#include <QObject>
#include <QStringList>
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
        IContextNetwork(QObject *parent = nullptr) : QObject(parent), m_dBusInterface(0) {}

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
         * \brief Send status messages
         * \param message
         */
        void statusMessage(const BlackMisc::CStatusMessage &message);

        /*!
         * \brief Send status messages
         * \param messages
         */
        void statusMessages(const BlackMisc::CStatusMessageList &messages);

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
         * \param from  old status
         * \param to    new status
         * \remarks If I use the enum, adaptor / interface are not created correctly
         * \see INetwork::ConnectionStatus
         */
        void connectionStatusChanged(uint from, uint to);

        /*!
         * \brief Text messages (also private chat messages)
         */
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    public slots:

        /*!
         * \brief Read ATC bookings
         * \return
         */
        virtual void readAtcBookingsFromSource();

        /*!
         * \brief The ATC list with online ATC controllers
         * \remarks If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const;

        /*!
         * \brief ATC list, with booked controllers
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const;

        /*!
         * \brief Aircraft list
         */
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const;

        /*!
         * \brief Get all users
         */
        virtual BlackMisc::Network::CUserList getUsers() const;

        /*!
         * \brief All users with callsign, e.g. for voice room resolution
         */
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const;

        /*!
         * \brief Get own aircraft
         */
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const;

        /*!
         * \brief Connect to Network
         * \return messages gererated during connecting
         * \see INetwork::LoginMode
         */
        virtual BlackMisc::CStatusMessageList connectToNetwork(uint loginMode);

        /*!
         * \brief Disconnect from network
         * \return messages generated during disconnecting
         */
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork();

        /*!
         * \brief Network connected?
         */
        virtual bool isConnected() const;

        /*!
         * Set own aircraft
         * \param aircraft
         * \return message list, as aircraft can only be set prior connecting
         */
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);

        /*!
         * \brief Own position, be aware height is terrain height
         */
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);

        /*!
         * \brief Complete situation update
         */
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        /*!
         * \brief Update own cockpit
         */
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder);

        /*!
         * \brief Text messages (radio and private chat messages)
         */
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages);

        /*!
         * \brief Get METAR, if not available request it
         * \param airportIcaoCode such as EDDF, KLAX
         * \return
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode);

        /*!
         * \brief Use the selected COM1/2 frequencies, and get the corresponding voice room for it
         */
        virtual BlackMisc::Voice::CVoiceRoomList getSelectedVoiceRooms() const;

        /*!
         * \brief Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
         */
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const;

        /*!
         * \brief Request data updates (pilot' frequencies, ATIS, ..)
         */
        virtual void requestDataUpdates();

        /*!
         * \brief Request ATIS updates (for all stations)
         */
        virtual void requestAtisUpdates();
    };
}

#endif // guard

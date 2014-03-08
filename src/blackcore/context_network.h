/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_H
#define BLACKCORE_CONTEXTNETWORK_H

#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/vvoiceroomlist.h"
#include <QObject>

#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextNetwork"
#define BLACKCORE_CONTEXTNETWORK_OBJECTPATH "/Network"

namespace BlackCore
{

    //! \brief Network context proxy
    class IContextNetwork : public QObject
    {
        Q_OBJECT
        Q_ENUMS(ConnectionStatus)
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

    public:
        //! \brief DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_INTERFACENAME);
            return s;
        }

        //! \brief DBus object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_OBJECTPATH);
            return s;
        }

        /*!
         * \brief Constructor
         * \param parent
         */
        IContextNetwork(QObject *parent = nullptr) : QObject(parent) {}

        //! Destructor
        virtual ~IContextNetwork() {}

        //! \brief Using local objects?
        virtual bool usingLocalObjects() const = 0;

    signals:

        /*!
         * \brief Send status message
         * \param message
         */
        void statusMessage(const BlackMisc::CStatusMessage &message);

        /*!
         * \brief Send status messages
         * \param messages
         */
        void statusMessages(const BlackMisc::CStatusMessageList &messages);

        //! \brief ATC station (online) list has been changed
        void changedAtcStationsOnline();

        //! \brief ATC station (booked) list has been changed
        void changedAtcStationsBooked();

        //! \brief Aircraft list has been changed
        void changedAircraftsInRange();

        //! \brief Terminated connection
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
         * \param textMessages message list
         */
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    public slots:

        //! \brief Reload bookings from booking service
        virtual void readAtcBookingsFromSource() const = 0;

        /*!
         * \brief The ATC list with online ATC controllers
         * \remarks If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const = 0;

        //! \brief ATC list, with booked controllers
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const = 0 ;

        //! \brief Aircraft list
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const = 0;

        //! \brief Get all users
        virtual BlackMisc::Network::CUserList getUsers() const = 0;

        //! \brief Users for given callsigns, e.g. for voice room resolution
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const = 0;

        //! \brief Get own aircraft
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

        /*!
         * \brief Connect to Network
         * \return messages gererated during connecting
         * \see INetwork::LoginMode
         */
        virtual BlackMisc::CStatusMessageList connectToNetwork(uint loginMode) = 0;

        /*!
         * \brief Disconnect from network
         * \return messages generated during disconnecting
         */
        virtual BlackMisc::CStatusMessageList disconnectFromNetwork() = 0;

        //! \brief Network connected?
        virtual bool isConnected() const = 0;

        /*!
         * Set own aircraft
         * \param aircraft
         * \return message list, as aircraft can only be set prior connecting
         */
        virtual BlackMisc::CStatusMessageList setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft) = 0;

        //! \brief Own position, be aware height is terrain height
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) = 0;

        //! \brief Complete situation update
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        //! \brief Update own cockpit
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) = 0;

        //! \brief Text messages (radio and private chat messages)
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        /*!
         * \brief Get METAR, if not available request it
         * \param airportIcaoCode such as EDDF, KLAX
         * \return
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode) = 0;

        //! \brief Use the selected COM1/2 frequencies, and get the corresponding voice room for it
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const = 0;

        //! \brief Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const = 0;

        //! \brief Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestDataUpdates() = 0;

        //! \brief Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;
    };
}

#endif // guard

/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTNETWORK_H
#define BLACKCORE_CONTEXTNETWORK_H

#include "blackcore/context.h"
#include "blackmisc/avallclasses.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/voiceroomlist.h"
#include "blackcore/network.h"

#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextNetwork"
#define BLACKCORE_CONTEXTNETWORK_OBJECTPATH "/Network"

namespace BlackCore
{

    //! \brief Network context proxy
    class IContextNetwork : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTNETWORK_OBJECTPATH);
            return s;
        }

        //! Destructor
        virtual ~IContextNetwork() {}

    signals:
        //! ATC station (online) list has been changed
        void changedAtcStationsOnline();

        //! ATC station (booked) list has been changed
        void changedAtcStationsBooked();

        //! Aircraft list has been changed
        void changedAircraftsInRange();

        //! Aircraft situation update
        void changedAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Terminated connection
        void connectionTerminated();

        /*!
         * \brief Connection status changed
         * \param from  old status
         * \param to    new status
         * \param message further details
         * \remarks If I use the enum, adaptor / interface are not created correctly
         * \see INetwork::ConnectionStatus
         */
        void connectionStatusChanged(uint from, uint to, const QString &message);

        //! Text messages received (also private chat messages, rfaio channel messages)
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    public slots:

        //! Reload bookings from booking service
        virtual void readAtcBookingsFromSource() const = 0;

        /*!
         * \brief The ATC list with online ATC controllers
         * \remarks If I make this &getAtcStations XML is not generated correctly
         */
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const = 0;

        //! ATC list, with booked controllers
        virtual const BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const = 0 ;

        //! Aircraft list
        virtual const BlackMisc::Aviation::CAircraftList getAircraftsInRange() const = 0;

        //! Get all users
        virtual BlackMisc::Network::CUserList getUsers() const = 0;

        //! Users for given callsigns, e.g. for voice room resolution
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const = 0;

        //! Get own aircraft
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

        //! Own position, be aware height is terrain height
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) = 0;

        //! Complete situation update
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) = 0;

        //! Update own cockpit
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder) = 0;

        //! Text messages (radio and private chat messages)
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        /*!
         * \brief Get METAR, if not available request it
         * \param airportIcaoCode such as EDDF, KLAX
         * \return
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const QString &airportIcaoCode) = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding voice room for it
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const = 0;

        //! Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestDataUpdates() = 0;

        //! Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;

    protected:
        //! Constructor
        IContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };
}

#endif // guard

/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTNETWORK_H
#define BLACKCORE_CONTEXTNETWORK_H

#include "blackcore/context.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/nwtextmessagelist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/voiceroomlist.h"
#include "blackcore/network.h"

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextNetwork"

//! DBus object path for context
#define BLACKCORE_CONTEXTNETWORK_OBJECTPATH "/Network"

//! @}

namespace BlackCore
{

    //! Network context proxy
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

        //! \copydoc CContext::getPathAndContextId()
        virtual QString getPathAndContextId() const { return this->buildPathAndContextId(ObjectPath()); }

        //! Factory method
        static IContextNetwork *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &conn);

        //! Destructor
        virtual ~IContextNetwork() {}

    signals:
        //! ATC station (online) list has been changed
        void changedAtcStationsOnline();

        //! Digest signal changedAtcStationsOnline()
        void changedAtcStationsOnlineDigest();

        //! ATC station (booked) list has been changed
        void changedAtcStationsBooked();

        //! Digest signal changedAtcStationsBooked()
        void changedAtcStationsBookedDigest();

        //! Aircraft list has been changed
        void changedAircraftsInRange();

        //! Digest signal changedAircraftsInRange()
        void changedAircraftsInRangeDigest();

        //! Aircraft situation update
        void changedAircraftSituation(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Connection status changed for online station
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &atcStation, bool connected);

        //! Terminated connection
        void connectionTerminated();

        //! VATSIM data file was read
        void vatsimDataFileRead();

        //! Bookings read
        void vatsimBookingsRead();

        /*!
         * Connection status changed
         * \param from  old status
         * \param to    new status
         * \remarks If I use the enum, adaptor / interface are not created correctly
         * \see INetwork::ConnectionStatus
         */
        void connectionStatusChanged(uint from, uint to);

        //! Text messages received (also private chat messages, rfaio channel messages)
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

    public slots:

        //! Reload bookings from booking service
        virtual void readAtcBookingsFromSource() const = 0;

        /*!
         * The ATC list with online ATC controllers
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

        //! User for given callsign, e.g. for text messages
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Information about other clients
        virtual BlackMisc::Network::CClientList getOtherClients() const = 0;

        //! Clients for given callsign, e.g. to test/fetch direct aircraft model
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const = 0;

        //! Known voice servers, available when data file was first read
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const = 0;

        //! Known FSD servers, available when data file was first read
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const = 0;

        /*!
         * Connect to Network
         * \return messages gererated during connecting
         * \see INetwork::LoginMode
         */
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, uint loginMode) = 0;

        /*!
         * Disconnect from network
         * \return messages generated during disconnecting
         */
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() = 0;

        //! Network connected?
        virtual bool isConnected() const = 0;

        //! Text messages (radio and private chat messages)
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) = 0;

        //! Send flight plan
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) = 0;

        //! Load flight plan (from network)
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Command line was entered
        virtual bool parseCommandLine(const QString &commandLine) = 0;

        /*!
         * Get METAR, if not available request it
         * \param airportIcaoCode such as EDDF, KLAX
         * \return
         */
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode) = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding voice room for it
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const = 0;

        //! Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestDataUpdates() = 0;

        //! Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;

        //! Create dummy ATC stations for performance tests etc.
        virtual void testCreateDummyOnlineAtcStations(int number) = 0;

    protected:
        //! Constructor
        IContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };
}

#endif // guard

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

#include "blackcoreexport.h"
#include "blackcore/context.h"
#include "blackmisc/identifier.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackcore/network.h"

//! \addtogroup dbus
//! @{

//! DBus interface for context
#define BLACKCORE_CONTEXTNETWORK_INTERFACENAME "org.swift_project.blackcore.contextnetwork"

//! DBus object path for context
#define BLACKCORE_CONTEXTNETWORK_OBJECTPATH "/network"

//! @}

namespace BlackCore
{

    //! Network context proxy
    class BLACKCORE_EXPORT IContextNetwork : public CContext
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
        static IContextNetwork *create(CRuntime *parent, CRuntimeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection);

        //! Destructor
        virtual ~IContextNetwork() {}

    signals:
        //! An aircraft disappeared
        void removedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

        //! A new aircraft appeared
        void addedAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

        //! Read for model matching
        void readyForModelMatching(const BlackMisc::Simulation::CSimulatedAircraft &renderedAircraft);

        //! ATC station (online) list has been changed
        void changedAtcStationsOnline();

        //! Digest signal changedAtcStationsOnline()
        void changedAtcStationsOnlineDigest();

        //! ATC station (booked) list has been changed
        void changedAtcStationsBooked();

        //! Digest signal changedAtcStationsBooked()
        void changedAtcStationsBookedDigest();

        //! Aircraft list has been changed
        void changedAircraftInRange();

        //! Digest signal changedAircraftInRange()
        void changedAircraftInRangeDigest();

        //! Aircraft model was changed
        //! \details All remote aircraft are stored in the network context. The model can be updated here
        //!          via \sa updateAircraftModel and then this signal is fired
        void changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! Aircraft enabled / disabled
        //! \details All remote aircraft are stored in the network context. The aircraft can be enabled (for rendering) here
        //!          via \sa updateAircraftEnabled and then this signal is fired
        void changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! Aircraft enabled / disabled
        void changedFastPositionUpdates(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

        //! Connection status changed for online station
        void changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &atcStation, bool connected);

        //! Terminated connection
        void connectionTerminated();

        //! VATSIM data file was read
        void vatsimDataFileRead();

        //! Bookings read
        void vatsimBookingsRead();

        //! Connection status changed
        //! \param from  old status
        //! \param to    new status
        //! \remarks If I use the enum, adaptor / interface are not created correctly
        //! \see INetwork::ConnectionStatus
        void connectionStatusChanged(int from, int to);

        //! Text messages received (also private chat messages, rfaio channel messages)
        void textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

        //! A superivisor text message was received
        void supervisorTextMessageReceived(const BlackMisc::Network::CTextMessage &message);

        //! Text message sent (by me)
        void textMessageSent(const BlackMisc::Network::CTextMessage &sentMessage);

    public slots:

        //! Reload bookings from booking service
        virtual void readAtcBookingsFromSource() const = 0;

        //! The ATC list with online ATC controllers
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const = 0;

        //! ATC list, with booked controllers
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const = 0 ;

        //! Aircraft list
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const = 0;

        //! Aircraft for given callsign
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Aircraft count
        virtual int getAircraftInRangeCount() const = 0;

        //! Online station for callsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Get all users
        virtual BlackMisc::Network::CUserList getUsers() const = 0;

        //! Users for given callsigns, e.g. for voice room resolution
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const = 0;

        //! User for given callsign, e.g. for text messages
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

        //! Information about other clients
        virtual BlackMisc::Network::CClientList getOtherClients() const = 0;

        //! Clients for given callsign, e.g. to test/fetch direct aircraft model
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const = 0;

        //! Known voice servers, available when data file was first read
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const = 0;

        //! Known FSD servers, available when data file was first read
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const = 0;

        //! Is interim position sending enabled?
        virtual bool isFastPositionSendingEnabled() const = 0;

        //! Enable interim position sending
        virtual void enableFastPositionSending(bool enable) = 0;

        //! Callsigns enabled for fast position updates
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) = 0;

        //! Callsigns enabled for fast position updates
        virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() = 0;

        //! Connect to Network
        //! \return messages generated during connecting
        //! \see INetwork::LoginMode
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, int loginMode) = 0;

        //! Server which is connected, if not connected empty default object.
        virtual BlackMisc::Network::CServer getConnectedServer() const = 0;

        //! Disconnect from network
        //! \return messages generated during disconnecting
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
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) = 0;

        //! Get METAR, if not available request it (code such as EDDF, KLAX)
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding voice room for it
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const = 0;

        //! Use the selected COM1/2 frequencies, and get the corresponding ATC stations for it
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const = 0;

        //! Request data updates (pilot's frequencies, ATIS, ..)
        virtual void requestDataUpdates() = 0;

        //! Request ATIS updates (for all stations)
        virtual void requestAtisUpdates() = 0;

        //! Enable/disable rendering
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator) = 0;

        //! Change model string
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::CIdentifier &originator) = 0;

        //! Change fast position updates
        virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending, const BlackMisc::CIdentifier &originator) = 0;

        //! Create dummy ATC stations for performance tests etc.
        virtual void testCreateDummyOnlineAtcStations(int number) = 0;

        //! Inject aircraft parts for testing
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) = 0;

    protected:
        //! Constructor
        IContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };
}

#endif // guard

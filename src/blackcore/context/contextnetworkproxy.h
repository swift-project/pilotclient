/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H
#define BLACKCORE_CONTEXT_CONTEXTNETWORK_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcoreexport.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/clientlist.h"
#include "blackmisc/network/network.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/weather/metar.h"

#include <stdbool.h>
#include <QObject>
#include <QString>

class QDBusConnection;

namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Aviation
    {
        class CAircraftParts;
        class CCallsign;
    }
    namespace Simulation { class CAircraftModel; }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! Network context proxy
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextNetworkProxy : public IContextNetwork
        {
            Q_OBJECT
            friend class IContextNetwork;

        public:

            //! Destructor
            virtual ~CContextNetworkProxy() {}

        public slots:
            //! \copydoc IContextNetwork::readAtcBookingsFromSource()
            virtual void readAtcBookingsFromSource() const override;

            //! \copydoc IContextNetwork::getAtcStationsOnline()
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override;

            //! \copydoc IContextNetwork::getAtcStationsBooked()
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override;

            //! \copydoc IContextNetwork::getAircraftInRange()
            virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;

            //! \copydoc IContextNetwork::getAircraftInRangeCallsigns()
            virtual BlackMisc::Aviation::CCallsignSet getAircraftInRangeCallsigns() const override;

            //! \copydoc IContextNetwork::getAircraftInRangeCount
            virtual int getAircraftInRangeCount() const override;

            //! \copydoc IContextNetwork::getAircraftInRangeForCallsign
            virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftInRangeForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IContextNetwork::getOnlineStationForCallsign
            virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IContextNetwork::connectToNetwork
            virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, BlackCore::INetwork::LoginMode mode) override;

            //! \copydoc IContextNetwork::disconnectFromNetwork()
            virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;

            //! \copydoc IContextNetwork::isConnected()
            virtual bool isConnected() const override;

            //! \copydoc IContextNetwork::getConnectedServer
            virtual BlackMisc::Network::CServer getConnectedServer() const override;

            //! \copydoc IContextNetwork::parseCommandLine
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

            //! \copydoc IContextNetwork::sendTextMessages()
            virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

            //! \copydoc IContextNetwork::sendFlightPlan()
            virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

            //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
            virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IContextNetwork::getMetarForAirport
            BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode) const override;

            //! \copydoc IContextNetwork::getSelectedVoiceRooms()
            virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

            //! \copydoc IContextNetwork::getSelectedAtcStations
            virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

            //! \copydoc IContextNetwork::getUsers()
            virtual BlackMisc::Network::CUserList getUsers() const override;

            //! \copydoc IContextNetwork::getUsersForCallsigns
            virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

            //! \copydoc IContextNetwork::getUserForCallsign
            virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IContextNetwork::getOtherClients
            virtual BlackMisc::Network::CClientList getOtherClients() const override;

            //! \copydoc IContextNetwork::getVatsimVoiceServers
            virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

            //! \copydoc IContextNetwork::getVatsimFsdServers
            virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

            //! \copydoc IContextNetwork::getOtherClientsForCallsigns
            virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const override;

            //! \copydoc IContextNetwork::requestDataUpdates
            virtual void requestDataUpdates()override;

            //! \copydoc IContextNetwork::requestAtisUpdates
            virtual void requestAtisUpdates() override;

            //! \copydoc IContextNetwork::updateAircraftEnabled
            virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering) override;

            //! \copydoc IContextNetwork::updateAircraftModel
            virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model) override;

            //! \copydoc IContextNetwork::updateFastPositionEnabled
            virtual bool updateFastPositionEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositionSending) override;

            //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
            virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignSet &callsigns) override;

            //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
            virtual BlackMisc::Aviation::CCallsignSet getFastPositionEnabledCallsigns() override;

            //! \copydoc IContextNetwork::getReverseLookupMessages
            virtual BlackMisc::CStatusMessageList getReverseLookupMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;

            //! \copydoc IContextNetwork::isReverseLookupMessagesEnabled
            virtual bool isReverseLookupMessagesEnabled() const override;

            //! \copydoc IContextNetwork::enableReverseLookupMessages
            virtual void enableReverseLookupMessages(bool enabled) override;

            //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
            virtual void testCreateDummyOnlineAtcStations(int number) override;

            //! \copydoc IContextNetwork::testAddAircraftParts
            virtual void testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface; /*!< DBus interface */

            //! Relay connection signals to local signals.
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextNetworkProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr) {}

            //! DBus version constructor
            CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard

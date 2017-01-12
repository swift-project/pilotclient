/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextnetworkproxy.h"
#include "blackmisc/audio/voiceroomlist.h"
#include "blackmisc/dbus.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1Literal>
#include <QObject>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Weather;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    namespace Context
    {
        CContextNetworkProxy::CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr)
        {
            this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
                serviceName , IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextNetworkProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
        {
            bool s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                        "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(BlackCore::INetwork::ConnectionStatus, BlackCore::INetwork::ConnectionStatus)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAtcStationsBooked", this, SIGNAL(changedAtcStationsBooked()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAtcStationsOnline", this, SIGNAL(changedAtcStationsOnline()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAtcStationsBookedDigest", this, SIGNAL(changedAtcStationsBookedDigest()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAtcStationsOnlineDigest", this, SIGNAL(changedAtcStationsOnlineDigest()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAircraftInRange", this, SIGNAL(changedAircraftInRange()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAtcStationOnlineConnectionStatus", this, SIGNAL(changedAtcStationOnlineConnectionStatus(BlackMisc::Aviation::CAtcStation, bool)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedAircraftInRangeDigest", this, SIGNAL(changedAircraftInRangeDigest()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "connectionTerminated", this, SIGNAL(connectionTerminated()));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "textMessagesReceived", this, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "textMessageSent", this, SIGNAL(textMessageSent(BlackMisc::Network::CTextMessage)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "webServiceDataRead", this, SIGNAL(webServiceDataRead(int, int, int)));

            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedRemoteAircraftEnabled", this, SIGNAL(changedRemoteAircraftEnabled(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedFastPositionUpdates", this, SIGNAL(changedFastPositionUpdates(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "addedAircraft", this, SIGNAL(addedAircraft(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "removedAircraft", this, SIGNAL(removedAircraft(BlackMisc::Aviation::CCallsign)));
            Q_ASSERT(s);
            Q_UNUSED(s);
            this->relayBaseClassSignals(serviceName, connection, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName());
        }

        void CContextNetworkProxy::requestAtcBookingsUpdate() const
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("requestAtcBookingsUpdate"));
        }

        BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsOnline() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsOnline"));
        }

        CAtcStationList CContextNetworkProxy::getAtcStationsBooked() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsBooked"));
        }

        CSimulatedAircraftList CContextNetworkProxy::getAircraftInRange() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraftList>(QLatin1Literal("getAircraftInRange"));
        }

        CCallsignSet CContextNetworkProxy::getAircraftInRangeCallsigns() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignSet>(QLatin1Literal("getAircraftInRangeCallsigns"));
        }

        int CContextNetworkProxy::getAircraftInRangeCount() const
        {
            return this->m_dBusInterface->callDBusRet<int>(QLatin1Literal("getAircraftInRangeCount"));
        }

        CSimulatedAircraft CContextNetworkProxy::getAircraftInRangeForCallsign(const CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraft>(QLatin1Literal("getAircraftInRangeForCallsign"), callsign);
        }

        CAtcStation CContextNetworkProxy::getOnlineStationForCallsign(const CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStation>(QLatin1Literal("getOnlineStationForCallsign"), callsign);
        }

        CUserList CContextNetworkProxy::getUsers() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsers"));
        }

        CUserList CContextNetworkProxy::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsersForCallsigns"), callsigns);
        }

        CUser CContextNetworkProxy::getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUser>(QLatin1Literal("getUserForCallsign"), callsign);
        }

        CClientList CContextNetworkProxy::getOtherClients() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1Literal("getOtherClients"));
        }

        CServerList CContextNetworkProxy::getVatsimFsdServers() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1Literal("getVatsimFsdServers"));
        }

        CServerList CContextNetworkProxy::getVatsimVoiceServers() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1Literal("getVatsimVoiceServers"));
        }

        CClientList CContextNetworkProxy::getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1Literal("getOtherClientsForCallsigns"), callsigns);
        }

        CVoiceRoomList CContextNetworkProxy::getSelectedVoiceRooms() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Audio::CVoiceRoomList>(QLatin1Literal("getSelectedVoiceRooms"));
        }

        CAtcStationList CContextNetworkProxy::getSelectedAtcStations() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getSelectedAtcStations"));
        }

        void CContextNetworkProxy::requestDataUpdates()
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("requestDataUpdates"));
        }

        void CContextNetworkProxy::requestAtisUpdates()
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("requestAtisUpdates"));
        }

        bool CContextNetworkProxy::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateAircraftEnabled"), callsign, enabledForRedering);
        }

        bool CContextNetworkProxy::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateAircraftModel"), callsign, model, originator);
        }

        bool CContextNetworkProxy::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateAircraftNetworkModel"), callsign, model, originator);
        }

        bool CContextNetworkProxy::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositionSending)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateFastPositionEnabled"), callsign, enableFastPositionSending);
        }

        void CContextNetworkProxy::setFastPositionEnabledCallsigns(CCallsignSet &callsigns)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("setFastPositionEnabledCallsigns"), callsigns);
        }

        CCallsignSet CContextNetworkProxy::getFastPositionEnabledCallsigns() const
        {
            return this->m_dBusInterface->callDBusRet<CCallsignSet>(QLatin1Literal("getFastPositionEnabledCallsigns"));
        }

        CStatusMessageList CContextNetworkProxy::getReverseLookupMessages(const CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<CStatusMessageList>(QLatin1Literal("getReverseLookupMessages"), callsign);
        }

        bool CContextNetworkProxy::isReverseLookupMessagesEnabled() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isReverseLookupLoggingEnabled"));
        }

        void CContextNetworkProxy::enableReverseLookupMessages(bool enabled)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("enableReverseLookupLogging"), enabled);
        }

        CStatusMessageList CContextNetworkProxy::getAircraftPartsHistory(const CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<CStatusMessageList>(QLatin1Literal("getAircraftPartsHistory"), callsign);
        }

        CAircraftPartsList CContextNetworkProxy::getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTimeValuesBefore) const
        {
            return this->m_dBusInterface->callDBusRet<CAircraftPartsList>(QLatin1Literal("getRemoteAircraftParts"), callsign, cutoffTimeValuesBefore);
        }

        bool CContextNetworkProxy::isAircraftPartsHistoryEnabled() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isAircraftPartsHistoryEnabled"));
        }

        void CContextNetworkProxy::enableAircraftPartsHistory(bool enabled)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("enableAircraftPartsHistory"), enabled);
        }

        void CContextNetworkProxy::testCreateDummyOnlineAtcStations(int number)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("testCreateDummyOnlineAtcStations"), number);
        }

        void CContextNetworkProxy::testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts, bool incremental)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("testAddAircraftParts"), callsign, parts, incremental);
        }

        void CContextNetworkProxy::testReceivedTextMessages(const CTextMessageList &textMessages)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("testReceivedTextMessages"), textMessages);
        }

        CStatusMessage CContextNetworkProxy::connectToNetwork(const CServer &server, INetwork::LoginMode loginMode)
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("connectToNetwork"), server, loginMode);
        }

        CStatusMessage CContextNetworkProxy::disconnectFromNetwork()
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("disconnectFromNetwork"));
        }

        bool CContextNetworkProxy::isConnected() const
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
        }

        CServer CContextNetworkProxy::getConnectedServer() const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CServer>(QLatin1Literal("getConnectedServer"));
        }

        bool CContextNetworkProxy::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("parseCommandLine"), commandLine, originator);
        }

        void CContextNetworkProxy::sendTextMessages(const CTextMessageList &textMessages)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("sendTextMessages"), textMessages);
        }

        void CContextNetworkProxy::sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan)
        {
            this->m_dBusInterface->callDBus(QLatin1Literal("sendFlightPlan"), flightPlan);
        }

        CFlightPlan CContextNetworkProxy::loadFlightPlanFromNetwork(const CCallsign &callsign) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CFlightPlan>(QLatin1Literal("loadFlightPlanFromNetwork"), callsign);
        }

        CMetar CContextNetworkProxy::getMetarForAirport(const CAirportIcaoCode &airportIcaoCode) const
        {
            return this->m_dBusInterface->callDBusRet<BlackMisc::Weather::CMetar>(QLatin1Literal("getMetarForAirport"), airportIcaoCode);
        }
    } // ns
} // ns

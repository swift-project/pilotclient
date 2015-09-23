/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_network_proxy.h"
#include "blackmisc/audio/voiceroomlist.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Weather;

namespace BlackCore
{
    /*
     * Constructor for DBus
     */
    CContextNetworkProxy::CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void CContextNetworkProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                    "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(int, int)));
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
                               "vatsimDataFileRead", this, SIGNAL(vatsimDataFileRead(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "vatsimBookingsRead", this, SIGNAL(vatsimBookingsRead(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "vatsimMetarsRead", this, SIGNAL(vatsimMetarsRead(int)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedRemoteAircraftModel", this, SIGNAL(changedRemoteAircraftModel(BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedRemoteAircraftEnabled", this, SIGNAL(changedRemoteAircraftEnabled(BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedFastPositionUpdates", this, SIGNAL(changedFastPositionUpdates(BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::CIdentifier)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "addedAircraft", this, SIGNAL(addedAircraft(BlackMisc::Simulation::CSimulatedAircraft)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "removedAircraft", this, SIGNAL(removedAircraft(BlackMisc::Aviation::CCallsign)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    void CContextNetworkProxy::readAtcBookingsFromSource() const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("readAtcBookingsFromSource"));
    }

    BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsOnline() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsOnline"));
    }

    CAtcStationList CContextNetworkProxy::getAtcStationsBooked() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsBooked"));
    }

    Simulation::CSimulatedAircraftList CContextNetworkProxy::getAircraftInRange() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraftList>(QLatin1Literal("getAircraftInRange"));
    }

    int CContextNetworkProxy::getAircraftInRangeCount() const
    {
        return this->m_dBusInterface->callDBusRet<int>(QLatin1Literal("getAircraftInRangeCount"));
    }

    Simulation::CSimulatedAircraft CContextNetworkProxy::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraft>(QLatin1Literal("getAircraftInRangeForCallsign"), callsign);
    }

    CAtcStation CContextNetworkProxy::getOnlineStationForCallsign(const CCallsign &callsign) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStation>(QLatin1Literal("getOnlineStationForCallsign"), callsign);
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsers"));
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsersForCallsigns"), callsigns);
    }

    BlackMisc::Network::CUser CContextNetworkProxy::getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUser>(QLatin1Literal("getUserForCallsign"), callsign);
    }

    BlackMisc::Network::CClientList CContextNetworkProxy::getOtherClients() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1Literal("getOtherClients"));
    }

    BlackMisc::Network::CServerList CContextNetworkProxy::getVatsimFsdServers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1Literal("getVatsimFsdServers"));
    }

    BlackMisc::Network::CServerList CContextNetworkProxy::getVatsimVoiceServers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1Literal("getVatsimVoiceServers"));
    }

    BlackMisc::Network::CClientList CContextNetworkProxy::getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1Literal("getOtherClientsForCallsigns"), callsigns);
    }

    BlackMisc::Audio::CVoiceRoomList CContextNetworkProxy::getSelectedVoiceRooms() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Audio::CVoiceRoomList>(QLatin1Literal("getSelectedVoiceRooms"));
    }

    BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getSelectedAtcStations() const
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

    bool CContextNetworkProxy::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const CIdentifier &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateAircraftEnabled"), callsign, enabledForRedering, originator);
    }

    bool CContextNetworkProxy::updateAircraftModel(const CCallsign &callsign, const Simulation::CAircraftModel &model, const CIdentifier &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateAircraftModel"), callsign, model, originator);
    }

    bool CContextNetworkProxy::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositionSending, const CIdentifier &originator)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("updateFastPositionEnabled"), callsign, enableFastPositionSending, originator);
    }

    bool CContextNetworkProxy::isFastPositionSendingEnabled() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isFastPositionSendingEnabled"));
    }

    void CContextNetworkProxy::enableFastPositionSending(bool enable)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("enableFastPositionSending"), enable);
    }

    void CContextNetworkProxy::setFastPositionEnabledCallsigns(CCallsignSet &callsigns)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("setFastPositionEnabledCallsigns"), callsigns);
    }

    CCallsignSet CContextNetworkProxy::getFastPositionEnabledCallsigns()
    {
        return this->m_dBusInterface->callDBusRet<CCallsignSet>(QLatin1Literal("getFastPositionEnabledCallsigns"));
    }

    void CContextNetworkProxy::testCreateDummyOnlineAtcStations(int number)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("testCreateDummyOnlineAtcStations"), number);
    }

    void CContextNetworkProxy::testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts, bool incremental)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("testAddAircraftParts"), callsign, parts, incremental);
    }

    CStatusMessage CContextNetworkProxy::connectToNetwork(const CServer &server, int loginMode)
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

    CMetar CContextNetworkProxy::getMetar(const CAirportIcaoCode &airportIcaoCode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Weather::CMetar>(QLatin1Literal("getMetar"), airportIcaoCode);
    }

} // namespace

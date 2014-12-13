/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_network_proxy.h"
#include "blackmisc/voiceroomlist.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

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
                                    "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(uint, uint)));
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
                               "changedAircraftsInRange", this, SIGNAL(changedAircraftsInRange()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedAtcStationOnlineConnectionStatus", this, SIGNAL(changedAtcStationOnlineConnectionStatus(BlackMisc::Aviation::CAtcStation, bool)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedAircraftsInRangeDigest", this, SIGNAL(changedAircraftsInRangeDigest()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "connectionTerminated", this, SIGNAL(connectionTerminated()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "textMessagesReceived", this, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "vatsimDataFileRead", this, SIGNAL(vatsimDataFileRead()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "vatsimBookingsRead", this, SIGNAL(vatsimBookingsRead()));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    void CContextNetworkProxy::readAtcBookingsFromSource() const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("readAtcBookingsFromSource"));
    }

    const BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsOnline() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsOnline"));
    }

    const BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsBooked() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsBooked"));
    }

    const BlackMisc::Aviation::CAircraftList CContextNetworkProxy::getAircraftsInRange() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraftList>(QLatin1Literal("getAircraftsInRange"));
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsers"));
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const
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

    BlackMisc::Network::CClientList CContextNetworkProxy::getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const
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

    void CContextNetworkProxy::testCreateDummyOnlineAtcStations(int number)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("testCreateDummyOnlineAtcStations"), number);
    }

    BlackMisc::CStatusMessage CContextNetworkProxy::connectToNetwork(const Network::CServer &server, uint loginMode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("connectToNetwork"), server, loginMode);
    }

    BlackMisc::CStatusMessage CContextNetworkProxy::disconnectFromNetwork()
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1Literal("disconnectFromNetwork"));
    }

    bool CContextNetworkProxy::isConnected() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    bool CContextNetworkProxy::parseCommandLine(const QString &commandLine)
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("commandLineEntered"), commandLine);
    }

    void CContextNetworkProxy::sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendTextMessages"), textMessages);
    }

    void CContextNetworkProxy::sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendFlightPlan"), flightPlan);
    }

    BlackMisc::Aviation::CFlightPlan CContextNetworkProxy::loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CFlightPlan>(QLatin1Literal("loadFlightPlanFromNetwork"), callsign);
    }

    BlackMisc::Aviation::CInformationMessage CContextNetworkProxy::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CInformationMessage>(QLatin1Literal("getMetar"), airportIcaoCode);
    }

} // namespace

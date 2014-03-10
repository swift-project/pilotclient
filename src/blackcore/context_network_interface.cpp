/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_network_interface.h"
#include "blackmisc/voiceroomlist.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    IContextNetwork::IContextNetwork(const QString &serviceName, QDBusConnection &connection, QObject *parent) : QObject(parent), m_dBusInterface(0)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void IContextNetwork::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(uint, uint)));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "changedAtcStationsBooked", this, SIGNAL(changedAtcStationsBooked()));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "changedAtcStationsOnline", this, SIGNAL(changedAtcStationsOnline()));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "connectionTerminated", this, SIGNAL(connectionTerminated()));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "statusMessage", this, SIGNAL(statusMessage(BlackMisc::CStatusMessage)));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "statusMessages", this, SIGNAL(statusMessages(BlackMisc::CStatusMessageList)));
        connection.connect(serviceName, IContextNetwork::ServicePath(), IContextNetwork::InterfaceName(),
                           "textMessagesReceived", this, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)));
    }

    /*
     * Logging
     */
    void IContextNetwork::log(const QString &method, const QString &m1, const QString &m2, const QString &m3, const QString &m4) const
    {
        if (m1.isEmpty())
            qDebug() << "   LOG: " << method;
        else if (m2.isEmpty())
            qDebug() << "   LOG: " << method << m1;
        else if (m3.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2;
        else if (m4.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2 << m3;
        else
            qDebug() << "   LOG: " << method << m1 << m2 << m3 << m4;
    }

    void IContextNetwork::readAtcBookingsFromSource()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("readAtcBookingsFromSource"));
    }

    const BlackMisc::Aviation::CAtcStationList IContextNetwork::getAtcStationsOnline() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsOnline"));
    }

    const BlackMisc::Aviation::CAtcStationList IContextNetwork::getAtcStationsBooked() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsBooked"));
    }

    const BlackMisc::Aviation::CAircraftList IContextNetwork::getAircraftsInRange() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraftList>(QLatin1Literal("getAircraftsInRange"));
    }

    BlackMisc::Network::CUserList IContextNetwork::getUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsers"));
    }

    BlackMisc::Network::CUserList IContextNetwork::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsersForCallsigns"), callsigns);
    }

    BlackMisc::Audio::CVoiceRoomList IContextNetwork::getSelectedVoiceRooms() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Audio::CVoiceRoomList>(QLatin1Literal("getSelectedVoiceRooms"));
    }

    BlackMisc::Aviation::CAtcStationList IContextNetwork::getSelectedAtcStations() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getSelectedAtcStations"));
    }

    void IContextNetwork::requestDataUpdates()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("requestDataUpdates"));
    }

    void IContextNetwork::requestAtisUpdates()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("requestAtisUpdates"));
    }

    BlackMisc::Aviation::CAircraft IContextNetwork::getOwnAircraft() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("getOwnAircraft"));
    }

    BlackMisc::CStatusMessageList IContextNetwork::connectToNetwork(uint loginMode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("connectToNetwork"), loginMode);
    }

    BlackMisc::CStatusMessageList IContextNetwork::disconnectFromNetwork()
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("disconnectFromNetwork"));
    }

    bool IContextNetwork::isConnected() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    BlackMisc::CStatusMessageList IContextNetwork::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("setOwnAircraft"), aircraft);
    }

    void IContextNetwork::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnPosition"), position, altitude);
    }

    void IContextNetwork::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnSituation"), situation);
    }

    void IContextNetwork::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnCockpit"), com1, com2, transponder);
    }

    void IContextNetwork::sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendTextMessages"), textMessages);
    }

    BlackMisc::Aviation::CInformationMessage IContextNetwork::getMetar(const QString &airportIcaoCode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CInformationMessage>(QLatin1Literal("getMetar"), airportIcaoCode);
    }

} // namespace

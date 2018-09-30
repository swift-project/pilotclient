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
#include "blackmisc/dbusserver.h"
#include "blackmisc/genericdbusinterface.h"

#include <QDBusConnection>
#include <QLatin1String>
#include <QObject>
#include <QtGlobal>
#include <QMetaObject>

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
            m_dBusInterface = new CGenericDBusInterface(
                serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                connection, this);
            this->relaySignals(serviceName, connection);
        }

        void CContextNetworkProxy::unitTestRelaySignals()
        {
            // connect signals, asserts when failures
            QDBusConnection con = QDBusConnection::sessionBus();
            CContextNetworkProxy c(CDBusServer::coreServiceName(), con, CCoreFacadeConfig::Remote, nullptr);
            Q_UNUSED(c);
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
                                   "kicked", this, SIGNAL(kicked(QString)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "textMessagesReceived", this, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "textMessageSent", this, SIGNAL(textMessageSent(BlackMisc::Network::CTextMessage)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedRemoteAircraftEnabled", this, SIGNAL(changedRemoteAircraftEnabled(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedFastPositionUpdates", this, SIGNAL(changedFastPositionUpdates(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "changedGndFlagCapability", this, SIGNAL(changedGndFlagCapability(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "addedAircraft", this, SIGNAL(addedAircraft(BlackMisc::Simulation::CSimulatedAircraft)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "removedAircraft", this, SIGNAL(removedAircraft(BlackMisc::Aviation::CCallsign)));
            Q_ASSERT(s);
            s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                   "connectedServerChanged", this, SIGNAL(connectedServerChanged(BlackMisc::Network::CServer)));
            Q_ASSERT(s);
            Q_UNUSED(s);
            this->relayBaseClassSignals(serviceName, connection, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName());
        }

        void CContextNetworkProxy::requestAtcBookingsUpdate() const
        {
            m_dBusInterface->callDBus(QLatin1String("requestAtcBookingsUpdate"));
        }

        BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsOnline(bool recalculateDistance) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1String("getAtcStationsOnline"), recalculateDistance);
        }

        CAtcStationList CContextNetworkProxy::getAtcStationsBooked(bool recalculateDistance) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1String("getAtcStationsBooked"), recalculateDistance);
        }

        CSimulatedAircraftList CContextNetworkProxy::getAircraftInRange() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraftList>(QLatin1String("getAircraftInRange"));
        }

        CCallsignSet CContextNetworkProxy::getAircraftInRangeCallsigns() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CCallsignSet>(QLatin1String("getAircraftInRangeCallsigns"));
        }

        int CContextNetworkProxy::getAircraftInRangeCount() const
        {
            return m_dBusInterface->callDBusRet<int>(QLatin1String("getAircraftInRangeCount"));
        }

        bool CContextNetworkProxy::isAircraftInRange(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("isAircraftInRange"), callsign);
        }

        CSimulatedAircraft CContextNetworkProxy::getAircraftInRangeForCallsign(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Simulation::CSimulatedAircraft>(QLatin1String("getAircraftInRangeForCallsign"), callsign);
        }

        CAtcStation CContextNetworkProxy::getOnlineStationForCallsign(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStation>(QLatin1String("getOnlineStationForCallsign"), callsign);
        }

        CUserList CContextNetworkProxy::getUsers() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1String("getUsers"));
        }

        CUserList CContextNetworkProxy::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1String("getUsersForCallsigns"), callsigns);
        }

        CUser CContextNetworkProxy::getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CUser>(QLatin1String("getUserForCallsign"), callsign);
        }

        CClientList CContextNetworkProxy::getClients() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1String("getClients"));
        }

        CServerList CContextNetworkProxy::getVatsimFsdServers() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1String("getVatsimFsdServers"));
        }

        CServerList CContextNetworkProxy::getVatsimVoiceServers() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CServerList>(QLatin1String("getVatsimVoiceServers"));
        }

        CClientList CContextNetworkProxy::getClientsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CClientList>(QLatin1String("getClientsForCallsigns"), callsigns);
        }

        bool CContextNetworkProxy::setOtherClient(const CClient &client)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("setOtherClientForCallsign"), client);
        }

        CVoiceRoomList CContextNetworkProxy::getSelectedVoiceRooms() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Audio::CVoiceRoomList>(QLatin1String("getSelectedVoiceRooms"));
        }

        CAtcStationList CContextNetworkProxy::getSelectedAtcStations() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1String("getSelectedAtcStations"));
        }

        void CContextNetworkProxy::requestDataUpdates()
        {
            m_dBusInterface->callDBus(QLatin1String("requestDataUpdates"));
        }

        void CContextNetworkProxy::requestAtisUpdates()
        {
            m_dBusInterface->callDBus(QLatin1String("requestAtisUpdates"));
        }

        bool CContextNetworkProxy::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateAircraftEnabled"), callsign, enabledForRendering);
        }

        bool CContextNetworkProxy::setAircraftEnabledFlag(const CCallsign &callsign, bool enabledForRendering)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("setEnabledFlag"), callsign, enabledForRendering);
        }

        bool CContextNetworkProxy::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateAircraftModel"), callsign, model, originator);
        }

        bool CContextNetworkProxy::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateAircraftNetworkModel"), callsign, model, originator);
        }

        bool CContextNetworkProxy::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositionSending)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateFastPositionEnabled"), callsign, enableFastPositionSending);
        }

        bool CContextNetworkProxy::updateAircraftSupportingGndFLag(const CCallsign &callsign, bool supportGndFlag)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("updateAircraftSupportingGndFLag"), callsign, supportGndFlag);
        }

        int CContextNetworkProxy::reInitializeAllAircraft()
        {
            return m_dBusInterface->callDBusRet<int>(QLatin1String("reInitializeAllAircraft"));
        }

        void CContextNetworkProxy::setFastPositionEnabledCallsigns(CCallsignSet &callsigns)
        {
            m_dBusInterface->callDBus(QLatin1String("setFastPositionEnabledCallsigns"), callsigns);
        }

        CCallsignSet CContextNetworkProxy::getFastPositionEnabledCallsigns() const
        {
            return m_dBusInterface->callDBusRet<CCallsignSet>(QLatin1String("getFastPositionEnabledCallsigns"));
        }

        CStatusMessageList CContextNetworkProxy::getReverseLookupMessages(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<CStatusMessageList>(QLatin1String("getReverseLookupMessages"), callsign);
        }

        bool CContextNetworkProxy::isReverseLookupMessagesEnabled() const
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("isReverseLookupLoggingEnabled"));
        }

        void CContextNetworkProxy::enableReverseLookupMessages(bool enabled)
        {
            m_dBusInterface->callDBus(QLatin1String("enableReverseLookupLogging"), enabled);
        }

        CStatusMessageList CContextNetworkProxy::getAircraftPartsHistory(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<CStatusMessageList>(QLatin1String("getAircraftPartsHistory"), callsign);
        }

        CAircraftPartsList CContextNetworkProxy::getRemoteAircraftParts(const BlackMisc::Aviation::CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<CAircraftPartsList>(QLatin1String("getRemoteAircraftParts"), callsign);
        }

        QString CContextNetworkProxy::getLibraryInfo(bool detailed) const
        {
            return m_dBusInterface->callDBusRet<QString>(QLatin1String("getLibraryInfo"), detailed);
        }

        bool CContextNetworkProxy::isAircraftPartsHistoryEnabled() const
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("isAircraftPartsHistoryEnabled"));
        }

        void CContextNetworkProxy::enableAircraftPartsHistory(bool enabled)
        {
            m_dBusInterface->callDBus(QLatin1String("enableAircraftPartsHistory"), enabled);
        }

        void CContextNetworkProxy::testCreateDummyOnlineAtcStations(int number)
        {
            m_dBusInterface->callDBus(QLatin1String("testCreateDummyOnlineAtcStations"), number);
        }

        void CContextNetworkProxy::testAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts, bool incremental)
        {
            m_dBusInterface->callDBus(QLatin1String("testAddAircraftParts"), callsign, parts, incremental);
        }

        void CContextNetworkProxy::testReceivedTextMessages(const CTextMessageList &textMessages)
        {
            m_dBusInterface->callDBus(QLatin1String("testReceivedTextMessages"), textMessages);
        }

        void CContextNetworkProxy::testRequestAircraftConfig(const CCallsign &callsign)
        {
            m_dBusInterface->callDBus(QLatin1String("testRequestAircraftConfig"), callsign);
        }

        bool CContextNetworkProxy::testAddAltitudeOffset(const CCallsign &callsign, const PhysicalQuantities::CLength &offset)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("testAddAltitudeOffset"), callsign, offset);
        }

        CStatusMessage CContextNetworkProxy::connectToNetwork(const CServer &server, INetwork::LoginMode loginMode)
        {
            return m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("connectToNetwork"), server, loginMode);
        }

        CStatusMessage CContextNetworkProxy::disconnectFromNetwork()
        {
            return m_dBusInterface->callDBusRet<BlackMisc::CStatusMessage>(QLatin1String("disconnectFromNetwork"));
        }

        bool CContextNetworkProxy::isConnected() const
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("isConnected"));
        }

        CServer CContextNetworkProxy::getConnectedServer() const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Network::CServer>(QLatin1String("getConnectedServer"));
        }

        INetwork::LoginMode CContextNetworkProxy::getLoginMode() const
        {
            return m_dBusInterface->callDBusRet<BlackCore::INetwork::LoginMode>(QLatin1String("getLoginMode"));
        }

        bool CContextNetworkProxy::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            return m_dBusInterface->callDBusRet<bool>(QLatin1String("parseCommandLine"), commandLine, originator);
        }

        void CContextNetworkProxy::sendTextMessages(const CTextMessageList &textMessages)
        {
            m_dBusInterface->callDBus(QLatin1String("sendTextMessages"), textMessages);
        }

        void CContextNetworkProxy::sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan)
        {
            m_dBusInterface->callDBus(QLatin1String("sendFlightPlan"), flightPlan);
        }

        CFlightPlan CContextNetworkProxy::loadFlightPlanFromNetwork(const CCallsign &callsign) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Aviation::CFlightPlan>(QLatin1String("loadFlightPlanFromNetwork"), callsign);
        }

        CMetar CContextNetworkProxy::getMetarForAirport(const CAirportIcaoCode &airportIcaoCode) const
        {
            return m_dBusInterface->callDBusRet<BlackMisc::Weather::CMetar>(QLatin1String("getMetarForAirport"), airportIcaoCode);
        }

        QMetaObject::Connection CContextNetworkProxy::connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot)
        {
            Q_UNUSED(receiver);
            Q_UNUSED(rawFsdMessageReceivedSlot);
            return {};
        }
    } // ns
} // ns

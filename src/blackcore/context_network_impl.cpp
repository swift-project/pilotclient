/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "context_runtime.h"
#include "context_settings.h"
#include "context_application.h"
#include "context_simulator.h"
#include "network_vatlib.h"
#include "vatsimbookingreader.h"
#include "vatsimdatafilereader.h"

#include "blackmisc/networkutils.h"
#include "blackmisc/avatcstationlist.h"

#include <QtXml/QDomElement>
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextNetwork::CContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextNetwork(mode, runtime), m_network(nullptr), m_vatsimBookingReader(nullptr), m_vatsimDataFileReader(nullptr), m_dataUpdateTimer(nullptr)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this);

        // 2. Init own aircraft
        this->initOwnAircraft();

        // 3. Init VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl(), this);
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::psReceivedBookings);
        this->m_vatsimBookingReader->setInterval(10 * 1000); // first read

        // 4. VATSIM data file
        QStringList dataFileUrls;
        dataFileUrls << "http://info.vroute.net/vatsim-data.txt";
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(dataFileUrls, this);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::psDataFileRead);
        this->m_vatsimDataFileReader->setInterval(5 * 1000); // first read

        // 5. Update timer for data
        this->m_dataUpdateTimer = new QTimer(this);
        this->connect(this->m_dataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
        this->m_dataUpdateTimer->start(30 * 1000);

        // 6. connect signals and slots
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::psFsdConnectionStatusChanged);
        this->connect(this->m_network, &INetwork::atcPositionUpdate, this, &CContextNetwork::psFsdAtcPositionUpdate);
        this->connect(this->m_network, &INetwork::atisReplyReceived, this, &CContextNetwork::psFsdAtisQueryReceived);
        this->connect(this->m_network, &INetwork::atisVoiceRoomReplyReceived, this, &CContextNetwork::psFsdAtisVoiceRoomQueryReceived);
        this->connect(this->m_network, &INetwork::atisLogoffTimeReplyReceived, this, &CContextNetwork::psFsdAtisLogoffTimeQueryReceived);
        this->connect(this->m_network, &INetwork::metarReplyReceived, this, &CContextNetwork::psFsdMetarReceived);
        this->connect(this->m_network, &INetwork::realNameReplyReceived, this, &CContextNetwork::psFsdRealNameReplyReceived);
        this->connect(this->m_network, &INetwork::icaoCodesReplyReceived, this, &CContextNetwork::psFsdIcaoCodesReceived);
        this->connect(this->m_network, &INetwork::pilotDisconnected, this, &CContextNetwork::psFsdPilotDisconnected);
        this->connect(this->m_network, &INetwork::atcDisconnected, this, &CContextNetwork::psFsdAtcControllerDisconnected);
        this->connect(this->m_network, &INetwork::aircraftPositionUpdate, this, &CContextNetwork::psFsdAircraftUpdateReceived);
        this->connect(this->m_network, &INetwork::frequencyReplyReceived, this, &CContextNetwork::psFsdFrequencyReceived);
        this->connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::psFsdTextMessageReceived);
        this->connect(this->m_network, &INetwork::capabilitiesReplyReceived, this, &CContextNetwork::psFsdCapabilitiesReplyReceived);
        this->connect(this->m_network, &INetwork::customPacketReceived, this, &CContextNetwork::psFsdCustomPackageReceived);
        this->connect(this->m_network, &INetwork::serverReplyReceived, this, &CContextNetwork::psFsdServerReplyReceived);
        if (this->getIContextApplication()) this->connect(this->m_network, &INetwork::statusMessage, this->getIContextApplication(), &IContextApplication::sendStatusMessage);
    }

    /*
     * Cleanup
     */
    CContextNetwork::~CContextNetwork()
    {
        if (this->isConnected()) this->disconnectFromNetwork();
    }

    /*
     * Init own aircraft
     */
    void CContextNetwork::initOwnAircraft()
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());
        this->m_ownAircraft.initComSystems();
        this->m_ownAircraft.initTransponder();
        CAircraftSituation situation(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CLength(0, CLengthUnit::m())),
            CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())
        );
        this->m_ownAircraft.setSituation(situation);
        this->m_ownAircraft.setPilot(this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer().getUser());

        // TODO: This would need to come from somewhere (mappings)
        // Own callsign, plane ICAO status, model used
        this->m_ownAircraft.setCallsign(CCallsign("BLACK"));
        this->m_ownAircraft.setIcaoInfo(CAircraftIcao("C172", "L1P", "GA", "GA", "0000ff"));
    }

    /*
     * Connect to network
     */
    CStatusMessageList CContextNetwork::connectToNetwork(uint loginMode)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        CStatusMessageList msgs;
        CServer currentServer = this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer();

        if (!currentServer.getUser().isValid())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid user credentials"));
        }
        else if (!this->m_ownAircraft.getIcaoInfo().hasAircraftAndAirlineDsignator())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid ICAO data for own aircraft"));
        }
        else if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already connected"));
        }
        else
        {
            QString msg;
            if (CNetworkUtils::canConnect(currentServer, msg, 2000))
            {
                INetwork::LoginMode mode = static_cast<INetwork::LoginMode>(loginMode);
                this->m_ownAircraft.setPilot(currentServer.getUser()); // still needed?
                this->m_network->presetServer(currentServer);
                this->m_network->presetLoginMode(mode);
                this->m_network->presetCallsign(this->m_ownAircraft.getCallsign());
                this->m_network->presetIcaoCodes(this->m_ownAircraft.getIcaoInfo());
                this->m_network->setOwnAircraft(this->m_ownAircraft);
                this->m_network->initiateConnection();
                msg = "Connection pending ";
                msg.append(" ").append(currentServer.getAddress()).append(" ").append(QString::number(currentServer.getPort()));
                msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
            }
            else
            {
                // I cannot connect, add error message
                msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, msg));
            }
        }
        return msgs;
    }

    /*
     * Disconnect from network
     */
    CStatusMessageList CContextNetwork::disconnectFromNetwork()
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        CStatusMessageList msgs;
        if (this->m_network->isConnected())
        {
            this->m_network->terminateConnection();
            this->m_aircraftsInRange.clear();
            this->m_atcStationsBooked.clear();
            this->m_atcStationsOnline.clear();
            this->m_otherClients.clear();
            this->m_metarCache.clear();
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, "Connection terminating"));
        }
        else
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already disconnected"));
        }
        return msgs;
    }

    /*
     * Connected
     */
    bool CContextNetwork::isConnected() const
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        return this->m_network->isConnected();
    }

    /*
     * Own Aircraft
     */
    CStatusMessageList CContextNetwork::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, aircraft.toQString());
        CStatusMessageList msgs;
        if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, "Cannot set aircraft info, network already connected"));
        }
        else
        {
            this->m_ownAircraft = aircraft;
        }
        return msgs;
    }

    /*
     * Own position
     */
    void CContextNetwork::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, position.toQString(), altitude.toQString());
        this->m_ownAircraft.setPosition(position);
        this->m_ownAircraft.setAltitude(altitude);
        this->m_network->setOwnAircraftPosition(position, altitude);
    }

    /*
     * Update own situation
     */
    void CContextNetwork::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, situation.toQString());
        this->m_ownAircraft.setSituation(situation);
        this->m_network->setOwnAircraftSituation(situation);
    }

    /*
     * Own cockpit data
     */
    void CContextNetwork::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, com1.toQString(), com2.toQString(), transponder.toQString());
        bool changed = false;
        if (com1 != this->m_ownAircraft.getCom1System())
        {
            this->m_ownAircraft.setCom1System(com1);
            changed = true;
        }
        if (com2 != this->m_ownAircraft.getCom2System())
        {
            this->m_ownAircraft.setCom2System(com2);
            changed = true;
        }
        if (transponder != this->m_ownAircraft.getTransponder())
        {
            this->m_ownAircraft.setTransponder(transponder);
            changed = true;
        }

        if (!changed) return;
        this->m_network->setOwnAircraftAvionics(com1, com2, transponder);
    }

    /*
     * Own aircraft
     */
    CAircraft CContextNetwork::getOwnAircraft() const
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, this->m_ownAircraft.toQString());
        return this->m_ownAircraft;
    }

    /*
     * Send text messages
     */
    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, textMessages.toQString());
        this->m_network->sendTextMessages(textMessages);
    }

    /*
     * Flight plan
     */
    void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, flightPlan.toQString());
        this->m_network->sendFlightPlan(flightPlan);
    }

    /*
     * All users
     */
    CUserList CContextNetwork::getUsers() const
    {
        CUserList users;
        foreach(CAtcStation station, this->m_atcStationsOnline)
        {
            CUser user = station.getController();
            users.push_back(user);
        }
        foreach(CAircraft aircraft, this->m_aircraftsInRange)
        {
            CUser user = aircraft.getPilot();
            users.push_back(user);
        }
        return users;
    }

    /*
     * Users with callsigns
     */
    CUserList CContextNetwork::getUsersForCallsigns(const CCallsignList &callsigns) const
    {
        CUserList users;
        if (callsigns.isEmpty()) return users;
        CCallsignList searchList(callsigns);

        // do aircrafts first, this will handle most callsigns
        foreach(CAircraft aircraft, this->m_aircraftsInRange)
        {
            if (searchList.isEmpty()) break;
            CCallsign callsign = aircraft.getCallsign();
            if (searchList.contains(callsign))
            {
                CUser user = aircraft.getPilot();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        foreach(CAtcStation station, this->m_atcStationsOnline)
        {
            if (searchList.isEmpty()) break;
            CCallsign callsign = station.getCallsign();
            if (searchList.contains(callsign))
            {
                CUser user = station.getController();
                users.push_back(user);
                searchList.remove(callsign);
            }
        }

        // we might have unresolved callsigns
        // these are the ones not in range
        foreach(CCallsign callsign, searchList)
        {
            CUserList usersByCallsign = this->m_vatsimDataFileReader->getUsersForCallsign(callsign);
            if (usersByCallsign.isEmpty())
            {
                CUser user;
                user.setCallsign(callsign);
                users.push_back(user);
            }
            else
            {
                users.push_back(usersByCallsign[0]);
            }
        }
        return users;
    }

    /*
     * User for callsign
     */
    CUser CContextNetwork::getUserForCallsign(const CCallsign &callsign) const
    {
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        CUserList users = this->getUsersForCallsigns(callsigns);
        if (users.size() < 1) return CUser();
        return users[0];
    }

    /*
     * Other clients
     */
    CClientList CContextNetwork::getOtherClients() const
    {
        return this->m_otherClients;
    }

    /*
     * Other clients
     */
    CClientList CContextNetwork::getOtherClientsForCallsigns(const CCallsignList &callsigns) const
    {
        CClientList clients;
        if (callsigns.isEmpty()) return clients;
        foreach(CCallsign callsign, callsigns)
        {
            clients.push_back(this->m_otherClients.findBy(&CClient::getCallsign, callsign));
        }
        return clients;
    }

    /*
     * Connection status changed
     */
    void CContextNetwork::psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, QString::number(from), QString::number(to));
        CStatusMessageList msgs;
        // send 1st position
        if (to == INetwork::Connected)
        {
            QString m("Connected, own aircraft ");
            m.append(this->m_ownAircraft.toQString(true));
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
        }

        // send as message
        QString m("connection status changed from %1 to %2");
        m = m.arg(INetwork::connectionStatusToString(from), INetwork::connectionStatusToString(to));
        if (!message.isEmpty()) m.append(" ").append(message);
        msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork,
                                      to == INetwork::DisconnectedError ? CStatusMessage::SeverityError : CStatusMessage::SeverityInfo, m));
        if (this->getIContextApplication()) this->getIContextApplication()->sendStatusMessages(msgs);

        // send as own signal
        emit this->connectionStatusChanged(from, to, message);
    }

    /*
     * Name query
     */
    void CContextNetwork::psFsdRealNameReplyReceived(const CCallsign &callsign, const QString &realname)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, callsign.toQString(), realname);
        if (realname.isEmpty()) return;
        CIndexVariantMap vm(CAtcStation::IndexControllerRealName, realname);
        this->m_atcStationsOnline.applyIf(&CAtcStation::getCallsign, callsign, vm);
        this->m_atcStationsBooked.applyIf(&CAtcStation::getCallsign, callsign, vm);

        vm = CIndexVariantMap(CAircraft::IndexPilotRealName, realname);
        this->m_aircraftsInRange.applyIf(&CAircraft::getCallsign, callsign, vm);

        vm = CIndexVariantMap(CClient::IndexRealName, realname);
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    /*
     * Data file has been read
     */
    void CContextNetwork::psDataFileRead()
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO);
        const int interval = 60 * 1000;
        if (this->m_vatsimDataFileReader->interval() < interval) this->m_vatsimDataFileReader->setInterval(interval);
    }

    /*
     * Radio text message received
     */
    void CContextNetwork::psFsdTextMessageReceived(const CTextMessageList &messages)
    {
        if (this->getRuntime()->isSlotLogForNetworkEnabled()) this->getRuntime()->logSlot(Q_FUNC_INFO, messages.toQString());
        this->textMessagesReceived(messages); // relay
    }

    /*
     * Capabilities
     */
    void CContextNetwork::psFsdCapabilitiesReplyReceived(const BlackMisc::Aviation::CCallsign &callsign, quint32 flags)
    {
        if (callsign.isEmpty()) return;
        CIndexVariantMap capabilities;
        capabilities.addValue(CClient::FsdAtisCanBeReceived, (flags & CNetworkVatlib::AcceptsAtisResponses));
        capabilities.addValue(CClient::FsdWithInterimPositions, (flags & CNetworkVatlib::SupportsInterimPosUpdates));
        capabilities.addValue(CClient::FsdWithModelDescription, (flags & CNetworkVatlib::SupportsModelDescriptions));
        CIndexVariantMap vm(CClient::IndexCapabilities, capabilities.toQVariant());
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    /*
     * Custom packages
     */
    void CContextNetwork::psFsdCustomPackageReceived(const CCallsign &callsign, const QString &package, const QStringList &data)
    {
        if (callsign.isEmpty() || data.isEmpty()) return;
        if (package.startsWith("FSIPIR", Qt::CaseInsensitive))
        {
            // Request of other client, I can get the other's model from that
            // FsInn response is usually my model
            QString model = data.last();
            if (model.isEmpty()) return;
            CIndexVariantMap vm(CClient::IndexQueriedModelString, QVariant(model));
            if (!this->m_otherClients.contains(&CClient::getCallsign, callsign))
            {
                // with custom packages it can happen,
                //the package is received before any other package
                this->m_otherClients.push_back(CClient(callsign));
            }
            this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
            this->sendFsipiCustomPackage(callsign); // response
        }
    }

    /*
     * Host
     */
    void CContextNetwork::psFsdServerReplyReceived(const CCallsign &callsign, const QString &host)
    {
        if (callsign.isEmpty() || host.isEmpty()) return;
        CIndexVariantMap vm(CClient::IndexHost, QVariant(host));
        this->m_otherClients.applyIf(&CClient::getCallsign, callsign, vm);
    }

    void CContextNetwork::sendFsipiCustomPackage(const CCallsign &recipientCallsign) const
    {
        QStringList data = this->createFsipiCustomPackageData();
        this->m_network->sendCustomPacket(recipientCallsign.asString(), "FSIPI", data);
    }

    void CContextNetwork::sendFsipirCustomPackage(const CCallsign &recipientCallsign) const
    {
        QStringList data = this->createFsipiCustomPackageData();
        this->m_network->sendCustomPacket(recipientCallsign.asString(), "FSIPIR", data);
    }

    QStringList CContextNetwork::createFsipiCustomPackageData() const
    {
        CAircraft me = this->getOwnAircraft();
        CAircraftIcao icao = me.getIcaoInfo();
        QString modelString;
        if (this->getIContextSimulator())
        {
            if (this->getIContextSimulator()->isConnected()) modelString = this->getIContextSimulator()->getOwnAircraftModel().getQueriedModelString();
        }
        if (modelString.isEmpty()) modelString = CProject::systemNameAndVersion();
        QStringList data = CNetworkVatlib::createFsipiCustomPackageData(
                               "0", icao.getAirlineDesignator(), icao.getAircraftDesignator(),
                               "", "", "", "",
                               icao.getAircraftCombinedType(), modelString);
        return data;
    }
} // namespace

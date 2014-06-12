/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "context_network_impl.h"
#include "context_runtime.h"
#include "context_settings.h"
#include "context_application.h"
#include "context_simulator.h"
#include "context_ownaircraft_impl.h"
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
using namespace BlackMisc::Audio;

namespace BlackCore
{

    /*
     * Init this context
     */
    CContextNetwork::CContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextNetwork(mode, runtime), m_network(nullptr), m_airspace(nullptr), m_vatsimBookingReader(nullptr), m_vatsimDataFileReader(nullptr), m_dataUpdateTimer(nullptr)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this);
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::psFsdConnectionStatusChanged);
        this->connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::psFsdTextMessageReceived);

        // 2. VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl(), this);
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::psReceivedBookings);
        this->m_vatsimBookingReader->read(); // first read
        this->m_vatsimBookingReader->setInterval(180 * 1000);

        // 3. VATSIM data file
        const QStringList dataFileUrls = { "http://info.vroute.net/vatsim-data.txt" };
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(dataFileUrls, this);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::psDataFileRead);
        this->m_vatsimDataFileReader->read(); // first read
        this->m_vatsimDataFileReader->setInterval(90 * 1000);

        // 4. Update timer for data (network data such as frequency)
        this->m_dataUpdateTimer = new QTimer(this);
        this->connect(this->m_dataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
        this->m_dataUpdateTimer->start(30 * 1000);

        // 5. Airspace contents
        this->m_airspace = new CAirspaceMonitor(this, this->m_network, this->m_vatsimBookingReader, this->m_vatsimDataFileReader);
        this->connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsOnline, this, &CContextNetwork::changedAtcStationsOnline);
        this->connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsBooked, this, &CContextNetwork::changedAtcStationsBooked);
        this->connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationOnlineConnectionStatus, this, &CContextNetwork::changedAtcStationOnlineConnectionStatus);
        this->connect(this->m_airspace, &CAirspaceMonitor::changedAircraftsInRange, this, &CContextNetwork::changedAircraftsInRange);
        this->connect(this->m_airspace, &CAirspaceMonitor::changedAircraftSituation, this, &CContextNetwork::changedAircraftSituation);
        this->connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraft, this->m_airspace, &CAirspaceMonitor::setOwnAircraft);

        // FIXME (MS) conditional increases the number of scenarios which must be considered and continuously tested
        if (this->getIContextApplication())
        {
            this->connect(this->m_network, &INetwork::statusMessage, this->getIContextApplication(), &IContextApplication::sendStatusMessage);
        }
    }

    /*
     * Cleanup
     */
    CContextNetwork::~CContextNetwork()
    {
        if (this->isConnected()) this->disconnectFromNetwork();
    }

    /*
     * Connect to network
     */
    CStatusMessageList CContextNetwork::connectToNetwork(uint loginMode)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        CStatusMessageList msgs;
        QString msg;
        CServer currentServer = this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer();

        if (!currentServer.getUser().isValid())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid user credentials"));
        }
        else if (!this->ownAircraft().getIcaoInfo().hasAircraftAndAirlineDesignator())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Invalid ICAO data for own aircraft"));
        }
        else if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already connected"));
        }
        else if (!CNetworkUtils::canConnect(currentServer, msg, 2000))
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, msg));
        }
        else
        {
            INetwork::LoginMode mode = static_cast<INetwork::LoginMode>(loginMode);
            this->getIContextOwnAircraft()->updatePilot(currentServer.getUser(), this->getPathAndContextId());
            const CAircraft ownAircraft = this->ownAircraft();
            this->m_network->presetServer(currentServer);
            this->m_network->presetLoginMode(mode);
            this->m_network->presetCallsign(ownAircraft.getCallsign());
            this->m_network->presetIcaoCodes(ownAircraft.getIcaoInfo());
            this->m_network->setOwnAircraft(ownAircraft);
            this->m_network->initiateConnection();
            msg = "Connection pending ";
            msg.append(" ").append(currentServer.getAddress()).append(" ").append(QString::number(currentServer.getPort()));
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, msg));
        }
        return msgs;
    }

    /*
     * Disconnect from network
     */
    CStatusMessageList CContextNetwork::disconnectFromNetwork()
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        CStatusMessageList msgs;
        if (this->m_network->isConnected())
        {
            this->m_network->terminateConnection();
            this->m_airspace->clear();
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
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->m_network->isConnected();
    }

    /*
     * Send text messages
     */
    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, textMessages.toQString());
        this->m_network->sendTextMessages(textMessages);
    }

    /*
     * Flight plan
     */
    void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, flightPlan.toQString());
        this->m_network->sendFlightPlan(flightPlan);
        this->m_network->sendFlightPlanQuery(this->ownAircraft().getCallsign());
    }

    CFlightPlan CContextNetwork::loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        return this->m_airspace->loadFlightPlanFromNetwork(callsign);
    }

    /*
     * All users
     */
    CUserList CContextNetwork::getUsers() const
    {
        return this->m_airspace->getUsers();
    }

    /*
     * Users with callsigns
     */
    CUserList CContextNetwork::getUsersForCallsigns(const CCallsignList &callsigns) const
    {
        CUserList users;
        if (callsigns.isEmpty()) return users;
        return this->m_airspace->getUsersForCallsigns(callsigns);
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
        return this->m_airspace->getOtherClients();
    }

    /*
     * Other clients
     */
    CClientList CContextNetwork::getOtherClientsForCallsigns(const CCallsignList &callsigns) const
    {
        return this->m_airspace->getOtherClientsForCallsigns(callsigns);
    }

    /*
     * Connection status changed
     */
    void CContextNetwork::psFsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, { QString::number(from), QString::number(to) });
        CStatusMessageList msgs;
        // send 1st position
        if (to == INetwork::Connected)
        {
            QString m("Connected, own aircraft ");
            m.append(this->ownAircraft().toQString(true));
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, m));
        }

        // send as message
        QString m("Connection status changed from %1 to %2");
        m = m.arg(INetwork::connectionStatusToString(from), INetwork::connectionStatusToString(to));
        if (!message.isEmpty()) m.append(" ").append(message);
        msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork,
                                      to == INetwork::DisconnectedError ? CStatusMessage::SeverityError : CStatusMessage::SeverityInfo, m));
        // FIXME (MS) conditional increases the number of scenarios which must be considered and continuously tested
        if (this->getIContextApplication())
        {
            this->getIContextApplication()->sendStatusMessages(msgs);
        }

        // send as own signal
        emit this->connectionStatusChanged(from, to, message);
    }

    /*
     * Data file (VATSIM) has been read
     */
    void CContextNetwork::psDataFileRead()
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        // TODO (MS) no test for if (this->getIContextApplication()) here?
        this->getIContextApplication()->sendStatusMessage(CStatusMessage::getInfoMessage("Read VATSIM data file", CStatusMessage::TypeTrafficNetwork));
    }

    /*
     * Radio text message received
     */
    void CContextNetwork::psFsdTextMessageReceived(const CTextMessageList &messages)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, messages.toQString());
        this->textMessagesReceived(messages); // relay
    }

    const CAircraft &CContextNetwork::ownAircraft() const
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
        return this->getRuntime()->getCContextOwnAircraft()->ownAircraft();
    }

    void CContextNetwork::psChangedOwnAircraft(const CAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_network);
        Q_UNUSED(originator);
        this->m_network->setOwnAircraft(aircraft);
    }

} // namespace

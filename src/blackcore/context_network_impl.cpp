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
        IContextNetwork(mode, runtime), m_airspace(nullptr), m_network(nullptr), m_currentStatus(INetwork::Disconnected),
        m_vatsimBookingReader(nullptr), m_vatsimDataFileReader(nullptr), m_dataUpdateTimer(nullptr)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this);
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::ps_FsdConnectionStatusChanged);
        this->connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::ps_FsdTextMessageReceived);

        // 2. VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl(), this);
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::ps_ReceivedBookings);
        this->m_vatsimBookingReader->read(); // first read
        this->m_vatsimBookingReader->setInterval(180 * 1000);

        // 3. VATSIM data file
        const QStringList dataFileUrls = { "http://info.vroute.net/vatsim-data.txt" };
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(dataFileUrls, this);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::ps_DataFileRead);
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
        this->connect(this->getIContextSimulator(), &IContextSimulator::ownAircraftModelChanged, this->m_airspace, &CAirspaceMonitor::setOwnAircraftModel);

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
        this->gracefulShutdown();
    }

    /*
     * Stop, going down
     */
    void CContextNetwork::gracefulShutdown()
    {
        if (this->m_vatsimBookingReader) this->m_vatsimBookingReader->stop();
        if (this->m_vatsimDataFileReader) this->m_vatsimDataFileReader->stop();
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
        else if (!CNetworkUtils::canConnect(currentServer, msg, 2000))
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityError, msg));
        }
        else if (this->m_network->isConnected())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Already connected"));
        }
        else if (this->isPendingConnection())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Pending connection, please wait"));
        }
        else
        {
            this->m_currentStatus = INetwork::Connecting; // as semaphore we are going to connect
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
            this->m_currentStatus = INetwork::Disconnecting; // as semaphore we are going to disconnect
            this->m_network->terminateConnection();
            this->m_airspace->clear();
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityInfo, "Connection terminating"));
        }
        else if (this->isPendingConnection())
        {
            msgs.push_back(CStatusMessage(CStatusMessage::TypeTrafficNetwork, CStatusMessage::SeverityWarning, "Pending connection, please wait"));
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

    bool CContextNetwork::isPendingConnection() const
    {
        // if underlying class says pending, we believe it. But not all states (e.g. disconnecting) are covered
        if (this->m_network->isPendingConnection()) return true;

        // now check out own extra states, e.g. disconnecting
        return INetwork::isPendingStatus(this->m_currentStatus);
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
    void CContextNetwork::ps_FsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, { QString::number(from), QString::number(to) });
        this->m_currentStatus = to;
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
        // This is more a guard than a real conditional, e.g. when system shuts down
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
    void CContextNetwork::ps_DataFileRead()
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO);
        // TODO (MS) no test for if (this->getIContextApplication()) here?
        this->getIContextApplication()->sendStatusMessage(CStatusMessage::getInfoMessage("Read VATSIM data file", CStatusMessage::TypeTrafficNetwork));
    }

    /*
     * Radio text message received
     */
    void CContextNetwork::ps_FsdTextMessageReceived(const CTextMessageList &messages)
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

    void CContextNetwork::ps_ChangedOwnAircraft(const CAircraft &aircraft, const QString &originator)
    {
        Q_ASSERT(this->m_network);
        Q_UNUSED(originator);
        this->m_network->setOwnAircraft(aircraft);
    }

    /*
     *  Reload bookings
     */
    void CContextNetwork::readAtcBookingsFromSource() const
    {
        Q_ASSERT(this->m_vatsimBookingReader);
        this->m_vatsimBookingReader->read();
    }

    /*
     * Update bookings
     */
    void CContextNetwork::ps_ReceivedBookings(const CAtcStationList &)
    {
        // TODO (MS) no test for if (this->getIContextApplication()) here?
        this->getIContextApplication()->sendStatusMessage(CStatusMessage::getInfoMessage("Read bookings from network", CStatusMessage::TypeTrafficNetwork));
    }

    /*
     * Update data
     */
    void CContextNetwork::requestDataUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;

        this->requestAtisUpdates();
        this->m_airspace->requestDataUpdates();
    }

    /*
     * Request new ATIS data
     */
    void CContextNetwork::requestAtisUpdates()
    {
        Q_ASSERT(this->m_network);
        if (!this->isConnected()) return;

        this->m_airspace->requestAtisUpdates();
    }

    /*
     * Request METAR
     */
    BlackMisc::Aviation::CInformationMessage CContextNetwork::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        this->getRuntime()->logSlot(c_logContext, Q_FUNC_INFO, airportIcaoCode.toQString());
        return m_airspace->getMetar(airportIcaoCode);
    }

    /*
     * Selected voice rooms
     */
    CAtcStationList CContextNetwork::getSelectedAtcStations() const
    {
        CAtcStation com1Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom1System());
        CAtcStation com2Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom2System());

        CAtcStationList selectedStations;
        selectedStations.push_back(com1Station);
        selectedStations.push_back(com2Station);
        return selectedStations;
    }

    /*
     * Selected voice rooms
     */
    CVoiceRoomList CContextNetwork::getSelectedVoiceRooms() const
    {
        CAtcStationList stations = this->getSelectedAtcStations();
        Q_ASSERT(stations.size() == 2);
        CVoiceRoomList rooms;
        CAtcStation s1 = stations[0];
        CAtcStation s2 = stations[1];
        rooms.push_back(s1.getVoiceRoom());
        rooms.push_back(s2.getVoiceRoom());

        // KB_REMOVE
        qDebug() << this->ownAircraft().getCom1System().getFrequencyActive() << s1.getCallsign() << s1.getFrequency() << s1.getVoiceRoom().getVoiceRoomUrl();
        qDebug() << this->ownAircraft().getCom2System().getFrequencyActive() << s2.getCallsign() << s2.getFrequency() << s2.getVoiceRoom().getVoiceRoomUrl();

        return rooms;
    }

} // namespace

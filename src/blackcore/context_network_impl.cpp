/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
#include "blackmisc/logmessage.h"
#include "blackmisc/simplecommandparser.h"

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
        IContextNetwork(mode, runtime)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getIContextSettings());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this);
        this->connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::ps_fsdConnectionStatusChanged);
        this->connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::ps_fsdTextMessageReceived);

        // 2. VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl(), this);
        this->connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::ps_receivedBookings);
        this->m_vatsimBookingReader->read(); // first read
        this->m_vatsimBookingReader->setInterval(180 * 1000);

        // 3. VATSIM data file
        const QStringList dataFileUrls = { "http://info.vroute.net/vatsim-data.txt" };
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(dataFileUrls, this);
        this->connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::ps_dataFileRead);
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
    CStatusMessage CContextNetwork::connectToNetwork(uint loginMode)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CServer currentServer = this->getIContextSettings()->getNetworkSettings().getCurrentTrafficNetworkServer();

        QString msg;
        if (!currentServer.getUser().isValid())
        {
            return CLogMessage(this).error("Invalid user credentials");
        }
        else if (!this->ownAircraft().getIcaoInfo().hasAircraftAndAirlineDesignator())
        {
            return CLogMessage(this).error("Invalid ICAO data for own aircraft");
        }
        else if (!CNetworkUtils::canConnect(currentServer, msg, 2000))
        {
            return CLogMessage(this).error(msg);
        }
        else if (this->m_network->isConnected())
        {
            return CLogMessage(this).error("Already connected");
        }
        else if (this->isPendingConnection())
        {
            return CLogMessage(this).error("Pending connection, please wait");
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
            return CLogMessage(this).info("Connection pending %1 %2") << currentServer.getAddress() << currentServer.getPort();
        }
    }

    /*
     * Disconnect from network
     */
    CStatusMessage CContextNetwork::disconnectFromNetwork()
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        if (this->m_network->isConnected())
        {
            this->m_currentStatus = INetwork::Disconnecting; // as semaphore we are going to disconnect
            this->m_network->terminateConnection();
            this->m_airspace->clear();
            return CLogMessage(this).info("Connection terminating");
        }
        else if (this->isPendingConnection())
        {
            return CLogMessage(this).warning("Pending connection, please wait");
        }
        else
        {
            return CLogMessage(this).warning("Already disconnected");
        }
    }

    /*
     * Connected
     */
    bool CContextNetwork::isConnected() const
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_network->isConnected();
    }

    /*
     * Pending connection
     */
    bool CContextNetwork::isPendingConnection() const
    {
        // if underlying class says pending, we believe it. But not all states (e.g. disconnecting) are covered
        if (this->m_network->isPendingConnection()) return true;

        // now check out own extra states, e.g. disconnecting
        return INetwork::isPendingStatus(this->m_currentStatus);
    }

    /*
     * Command line entered
     */
    bool CContextNetwork::parseCommandLine(const QString &commandLine)
    {
        Q_UNUSED(commandLine);
        return false;
    }

    /*
     * Send text messages
     */
    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages;
        this->m_network->sendTextMessages(textMessages);
    }

    /*
     * Flight plan
     */
    void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << flightPlan;
        this->m_network->sendFlightPlan(flightPlan);
        this->m_network->sendFlightPlanQuery(this->ownAircraft().getCallsign());
    }

    CFlightPlan CContextNetwork::loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
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
     * Other clients for callsign
     */
    CClientList CContextNetwork::getOtherClientsForCallsigns(const CCallsignList &callsigns) const
    {
        return this->m_airspace->getOtherClientsForCallsigns(callsigns);
    }

    /*
     * VATSIM FSD servers
     */
    CServerList CContextNetwork::getVatsimFsdServers() const
    {
        Q_ASSERT(this->m_vatsimDataFileReader);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_vatsimDataFileReader->getFsdServers();
    }

    /*
     * VATSIM Voice servers
     */
    CServerList CContextNetwork::getVatsimVoiceServers() const
    {
        Q_ASSERT(this->m_vatsimDataFileReader);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_vatsimDataFileReader->getVoiceServers();
    }

    /*
     * Connection status changed
     */
    void CContextNetwork::ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to, const QString &message)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << from << to;
        auto fromOld = this->m_currentStatus;
        this->m_currentStatus = to;

        if (fromOld == INetwork::Disconnecting)
        {
            // remark: vatlib does not know disconnecting. In vatlib's terminating connection method
            // state Disconnecting is sent manually. We fix the vatlib state here regarding disconnecting
            from = INetwork::Disconnecting;
        }

        // send 1st position
        if (to == INetwork::Connected)
        {
            CLogMessage(this).info("Connected, own aircraft %1") << this->ownAircraft().toQString();
        }

        // send as message
        if (to == INetwork::DisconnectedError)
        {
            CLogMessage(this).error("Connection status changed from %1 to %2 %3") << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to) << message;
        }
        else
        {
            CLogMessage(this).info("Connection status changed from %1 to %2 %3") << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to) << message;
        }

        // send as own signal
        emit this->connectionStatusChanged(from, to, message);
    }

    /*
     * Data file (VATSIM) has been read
     */
    void CContextNetwork::ps_dataFileRead()
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        CLogMessage(this).info("Read VATSIM data file");
    }

    /*
     * Radio text message received
     */
    void CContextNetwork::ps_fsdTextMessageReceived(const CTextMessageList &messages)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << messages;
        this->textMessagesReceived(messages); // relay
    }

    const CAircraft &CContextNetwork::ownAircraft() const
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
        return this->getRuntime()->getCContextOwnAircraft()->ownAircraft();
    }

    void CContextNetwork::ps_changedOwnAircraft(const CAircraft &aircraft, const QString &originator)
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
     * Updated bookings
     */
    void CContextNetwork::ps_receivedBookings(const CAtcStationList &)
    {
        CLogMessage(this).info("Read bookings from network");
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
     * Create dummy ATC stations
     */
    void CContextNetwork::testCreateDummyOnlineAtcStations(int number)
    {
        this->m_airspace->testCreateDummyOnlineAtcStations(number);
    }

    /*
     * Request METAR
     */
    BlackMisc::Aviation::CInformationMessage CContextNetwork::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << airportIcaoCode;
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
        return rooms;
    }

} // namespace

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
#include "blackmisc/simulation/simdirectaccessownaircraft.h"

#include <QtXml/QDomElement>
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;
using namespace BlackSim;

namespace BlackCore
{
    CContextNetwork::CContextNetwork(CRuntimeConfig::ContextMode mode, CRuntime *runtime) :
        IContextNetwork(mode, runtime)
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getIContextSettings());
        Q_ASSERT(this->getIContextOwnAircraft());
        Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());

        // 1. Init by "network driver"
        this->m_network = new CNetworkVatlib(this->getRuntime()->getCContextOwnAircraft(), this);
        connect(this->m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::ps_fsdConnectionStatusChanged);
        connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::ps_fsdTextMessageReceived);

        // 2. VATSIM bookings
        this->m_vatsimBookingReader = new CVatsimBookingReader(this, this->getRuntime()->getIContextSettings()->getNetworkSettings().getBookingServiceUrl());
        connect(this->m_vatsimBookingReader, &CVatsimBookingReader::dataRead, this, &CContextNetwork::ps_receivedBookings);
        this->m_vatsimBookingReader->start();
        this->m_vatsimBookingReader->setInterval(180 * 1000);
        this->m_vatsimBookingReader->readInBackgroundThread(); // first read

        // 3. VATSIM data file
        const QStringList dataFileUrls = { "http://info.vroute.net/vatsim-data.txt" };
        this->m_vatsimDataFileReader = new CVatsimDataFileReader(this, dataFileUrls);
        connect(this->m_vatsimDataFileReader, &CVatsimDataFileReader::dataRead, this, &CContextNetwork::ps_dataFileRead);
        this->m_vatsimDataFileReader->start();
        this->m_vatsimDataFileReader->readInBackgroundThread(); // first read
        this->m_vatsimDataFileReader->setInterval(90 * 1000);

        // 4. Update timer for data (network data such as frequency)
        this->m_dataUpdateTimer = new QTimer(this);
        connect(this->m_dataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
        this->m_dataUpdateTimer->start(30 * 1000);

        // 5. Airspace contents
        const IOwnAircraftProviderReadOnly *ownAircraft = runtime->getCContextOwnAircraft();
        this->m_airspace = new CAirspaceMonitor(this, ownAircraft, this->m_network, this->m_vatsimBookingReader, this->m_vatsimDataFileReader);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsOnline, this, &CContextNetwork::changedAtcStationsOnline);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsBooked, this, &CContextNetwork::changedAtcStationsBooked);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationOnlineConnectionStatus, this, &CContextNetwork::changedAtcStationOnlineConnectionStatus);
        connect(this->m_airspace, &CAirspaceMonitor::changedAircraftInRange, this, &CContextNetwork::changedAircraftInRange);
        connect(this->m_airspace, &CAirspaceMonitor::removedRemoteAircraft, this, &IContextNetwork::removedAircraft);
        connect(this->m_airspace, &CAirspaceMonitor::removedRemoteAircraft, this, &CContextNetwork::removedRemoteAircraft);
        connect(this->m_airspace, &CAirspaceMonitor::readyForModelMatching, this, &CContextNetwork::readyForModelMatching);
        connect(this->m_airspace, &CAirspaceMonitor::addedRemoteAircraftParts, this, &CContextNetwork::addedRemoteAircraftParts);
        connect(this->m_airspace, &CAirspaceMonitor::addedRemoteAircraftSituation, this, &CContextNetwork::addedRemoteAircraftSituation);
        connect(this->m_airspace, &CAirspaceMonitor::addedAircraft, this, &CContextNetwork::addedAircraft);
    }

    CContextNetwork::~CContextNetwork()
    {
        this->gracefulShutdown();
    }

    const CSimulatedAircraftList &CContextNetwork::remoteAircraft() const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraft();
    }

    CSimulatedAircraftList &CContextNetwork::remoteAircraft()
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraft();
    }

    CAircraftSituationList &CContextNetwork::remoteAircraftSituations()
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftSituations();
    }

    const CAircraftSituationList &CContextNetwork::remoteAircraftSituations() const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftSituations();
    }

    const CAircraftPartsList &CContextNetwork::remoteAircraftParts() const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftParts();
    }

    CAircraftPartsList &CContextNetwork::remoteAircraftParts()
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftParts();
    }

    bool CContextNetwork::connectRemoteAircraftProviderSignals(
        std::function<void (const CAircraftSituation &)> situationSlot,
        std::function<void (const CAircraftParts &)> partsSlot,
        std::function<void (const CCallsign &)> removedAircraftSlot)
    {
        Q_ASSERT(this->m_airspace);
        return this->m_airspace->connectRemoteAircraftProviderSignals(situationSlot, partsSlot, removedAircraftSlot);
    }

    void CContextNetwork::gracefulShutdown()
    {
        if (this->m_vatsimBookingReader)  { this->m_vatsimBookingReader->requestStop();  this->m_vatsimBookingReader->quit(); }
        if (this->m_vatsimDataFileReader) { this->m_vatsimDataFileReader->requestStop(); this->m_vatsimDataFileReader->quit(); }
        if (this->isConnected()) { this->disconnectFromNetwork(); }
    }

    CStatusMessage CContextNetwork::connectToNetwork(const CServer &server, uint loginMode)
    {
        if (this->isDebugEnabled()) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        QString msg;
        if (!server.getUser().isValid())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, "Invalid user credentials");
        }
        else if (!this->ownAircraft().getIcaoInfo().hasAircraftAndAirlineDesignator())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, "Invalid ICAO data for own aircraft");
        }
        else if (!CNetworkUtils::canConnect(server, msg, 2000))
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, msg);
        }
        else if (this->m_network->isConnected())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, "Already connected");
        }
        else if (this->isPendingConnection())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, "Pending connection, please wait");
        }
        else
        {
            this->m_currentStatus = INetwork::Connecting; // as semaphore we are going to connect
            this->m_airspace->setConnected(true);
            INetwork::LoginMode mode = static_cast<INetwork::LoginMode>(loginMode);
            this->getIContextOwnAircraft()->updatePilot(server.getUser());
            const CAircraft ownAircraft = this->ownAircraft();
            this->m_network->presetServer(server);
            this->m_network->presetLoginMode(mode);
            this->m_network->presetCallsign(ownAircraft.getCallsign());
            this->m_network->presetIcaoCodes(ownAircraft.getIcaoInfo());
            if (getIContextSimulator())
            {
                this->m_network->presetSimulatorInfo(getIContextSimulator()->getSimulatorInfo());
            }
            else
            {
                this->m_network->presetSimulatorInfo(CSimulatorInfo::UnspecifiedSim());
            }
            this->m_network->initiateConnection();
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "Connection pending " + server.getAddress() + " " + QString::number(server.getPort()));
        }
    }

    CStatusMessage CContextNetwork::disconnectFromNetwork()
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (this->m_network->isConnected())
        {
            this->m_currentStatus = INetwork::Disconnecting; // as semaphore we are going to disconnect
            this->m_network->terminateConnection();
            this->m_airspace->setConnected(false);
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "Connection terminating");
        }
        else if (this->isPendingConnection())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "Pending connection, please wait");
        }
        else
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityWarning, "Already disconnected");
        }
    }

    bool CContextNetwork::isConnected() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_network->isConnected();
    }

    bool CContextNetwork::isPendingConnection() const
    {
        // if underlying class says pending, we believe it. But not all states (e.g. disconnecting) are covered
        if (this->m_network->isPendingConnection()) return true;

        // now check out own extra states, e.g. disconnecting
        return INetwork::isPendingStatus(this->m_currentStatus);
    }

    bool CContextNetwork::parseCommandLine(const QString &commandLine)
    {
        Q_UNUSED(commandLine);
        return false;
    }

    void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages; }
        this->m_network->sendTextMessages(textMessages);
    }

    void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << flightPlan; }
        this->m_network->sendFlightPlan(flightPlan);
        this->m_network->sendFlightPlanQuery(this->ownAircraft().getCallsign());
    }

    CFlightPlan CContextNetwork::loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->loadFlightPlanFromNetwork(callsign);
    }

    CUserList CContextNetwork::getUsers() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getUsers();
    }

    CUserList CContextNetwork::getUsersForCallsigns(const CCallsignList &callsigns) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CUserList users;
        if (callsigns.isEmpty()) return users;
        return this->m_airspace->getUsersForCallsigns(callsigns);
    }

    CUser CContextNetwork::getUserForCallsign(const CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CCallsignList callsigns;
        callsigns.push_back(callsign);
        CUserList users = this->getUsersForCallsigns(callsigns);
        if (users.size() < 1) return CUser();
        return users[0];
    }

    CClientList CContextNetwork::getOtherClients() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getOtherClients();
    }

    CClientList CContextNetwork::getOtherClientsForCallsigns(const CCallsignList &callsigns) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getOtherClientsForCallsigns(callsigns);
    }

    CServerList CContextNetwork::getVatsimFsdServers() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT(this->m_vatsimDataFileReader);
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_vatsimDataFileReader->getFsdServers();
    }

    CServerList CContextNetwork::getVatsimVoiceServers() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT(this->m_vatsimDataFileReader);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        return this->m_vatsimDataFileReader->getVoiceServers();
    }

    void CContextNetwork::ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << from << to; }
        auto fromOld = this->m_currentStatus; // own status cached
        this->m_currentStatus = to;

        if (fromOld == INetwork::Disconnecting)
        {
            // remark: vatlib does not know disconnecting. In vatlib's terminating connection method
            // state Disconnecting is sent manually. We fix the vatlib state here regarding disconnecting
            from = INetwork::Disconnecting;
        }

        if (to == INetwork::Disconnected)
        {
            // make sure airspace is really cleaned up
            Q_ASSERT(m_airspace);
            m_airspace->clear();
        }

        // send 1st position
        if (to == INetwork::Connected)
        {
            CLogMessage(this).info("Connected, own aircraft %1") << this->ownAircraft().toQString();
        }

        // send as message
        if (to == INetwork::DisconnectedError)
        {
            CLogMessage(this).error("Connection status changed from %1 to %2") << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to);
        }
        else
        {
            CLogMessage(this).info("Connection status changed from %1 to %2") << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to);
        }

        // send as own signal
        emit this->connectionStatusChanged(from, to);
    }

    void CContextNetwork::ps_dataFileRead()
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CLogMessage(this).info("Read VATSIM data file");
        emit vatsimDataFileRead();
    }

    void CContextNetwork::ps_fsdTextMessageReceived(const CTextMessageList &messages)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << messages; }
        this->textMessagesReceived(messages); // relay
    }

    const CAircraft &CContextNetwork::ownAircraft() const
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
        return this->getRuntime()->getCContextOwnAircraft()->ownAircraft();
    }

    void CContextNetwork::readAtcBookingsFromSource() const
    {
        Q_ASSERT(this->m_vatsimBookingReader);
        CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO;
        this->m_vatsimBookingReader->readInBackgroundThread();
    }

    CAtcStationList CContextNetwork::getAtcStationsOnline() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getAtcStationsOnline();
    }

    CAtcStationList CContextNetwork::getAtcStationsBooked() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getAtcStationsBooked();
    }

    CSimulatedAircraftList CContextNetwork::getAircraftInRange() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->remoteAircraft();
    }

    CSimulatedAircraft CContextNetwork::getAircraftForCallsign(const CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
        return this->m_airspace->remoteAircraft().findFirstByCallsign(callsign);
    }

    void CContextNetwork::ps_receivedBookings(const CAtcStationList &)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CLogMessage(this).info("Read bookings from network");
        emit vatsimBookingsRead();
    }

    void CContextNetwork::requestDataUpdates()
    {
        Q_ASSERT(this->m_network);
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!this->isConnected()) { return; }

        this->requestAtisUpdates();
        this->m_airspace->requestDataUpdates();
    }

    void CContextNetwork::requestAtisUpdates()
    {
        Q_ASSERT(this->m_network);
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        if (!this->isConnected()) { return; }

        this->m_airspace->requestAtisUpdates();
    }

    bool CContextNetwork::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const QString &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enabledForRedering << originator; }
        bool c = this->m_airspace->updateAircraftEnabled(callsign, enabledForRedering, originator);
        if (c)
        {
            emit this->changedAircraftEnabled(this->remoteAircraft().findFirstByCallsign(callsign), originator);
        }
        return c;
    }

    bool CContextNetwork::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const QString &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << model << originator; }
        bool c = this->m_airspace->updateAircraftModel(callsign, model, originator);
        if (c)
        {
            emit this->changedRenderedAircraftModel(this->remoteAircraft().findFirstByCallsign(callsign), originator);
        }
        return c;
    }

    bool CContextNetwork::isInterimPositionSendingEnabled() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT(this->m_network);
        return m_network->isInterimPositionSendingEnabled();
    }

    void CContextNetwork::enableInterimPositionSending(bool enable)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enable; }
        Q_ASSERT(this->m_network);
        m_network->enableInterimPositionSending(enable);
    }

    void CContextNetwork::testCreateDummyOnlineAtcStations(int number)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }
        this->m_airspace->testCreateDummyOnlineAtcStations(number);
    }

    void CContextNetwork::testAddAircraftParts(const CAircraftParts &parts, bool incremental)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << parts << incremental; }
        this->m_airspace->testAddAircraftParts(parts, incremental);
    }

    BlackMisc::Aviation::CInformationMessage CContextNetwork::getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << airportIcaoCode; }
        return m_airspace->getMetar(airportIcaoCode);
    }

    CAtcStationList CContextNetwork::getSelectedAtcStations() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CAtcStation com1Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom1System());
        CAtcStation com2Station = this->m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom2System());

        CAtcStationList selectedStations;
        selectedStations.push_back(com1Station);
        selectedStations.push_back(com2Station);
        return selectedStations;
    }

    CVoiceRoomList CContextNetwork::getSelectedVoiceRooms() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
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

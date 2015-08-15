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
#include "vatsim_metar_reader.h"
#include "airspace_monitor.h"
#include "web_datareader.h"
#include "blackmisc/networkutils.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/simulation/ownaircraftprovider.h"

#include <QtXml/QDomElement>
#include <QNetworkReply>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

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
        connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::textMessagesReceived);
        connect(this->m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::ps_checkForSupervisiorTextMessage);
        connect(this->m_network, &INetwork::textMessageSent, this, &CContextNetwork::textMessageSent);

        // 2. Update timer for data (network data such as frequency)
        this->m_dataUpdateTimer = new QTimer(this);
        connect(this->m_dataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
        this->m_dataUpdateTimer->start(30 * 1000);

        // 3. data reader
        this->m_webDataReader = new CWebDataReader(CWebDataReader::AllReaders, this);
        this->m_webReaderSignalConnections = this->m_webDataReader->connectVatsimDataSignals(
                std::bind(&CContextNetwork::vatsimBookingsRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::vatsimDataFileRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::vatsimMetarsRead, this, std::placeholders::_1));
        this->m_webReaderSignalConnections.append(
            this->m_webDataReader->connectSwiftDatabaseSignals(
                this, // the object here must be the same as in the bind
                std::bind(&CContextNetwork::aircraftIcaoCodeRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::airlineIcaoCodeRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::liveriesRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::distributorsRead, this, std::placeholders::_1),
                std::bind(&CContextNetwork::modelsRead, this, std::placeholders::_1)
            ));
        this->m_webDataReader->readAllInBackground(1000);

        // 4. Airspace contents
        Q_ASSERT_X(this->getRuntime()->getCContextOwnAircraft(), Q_FUNC_INFO, "this and own aircraft context must be local");
        Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing reader");
        this->m_airspace = new CAirspaceMonitor(this->getRuntime()->getCContextOwnAircraft(), this->m_network, this->m_webDataReader, this);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsOnline, this, &CContextNetwork::changedAtcStationsOnline);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationsBooked, this, &CContextNetwork::changedAtcStationsBooked);
        connect(this->m_airspace, &CAirspaceMonitor::changedAtcStationOnlineConnectionStatus, this, &CContextNetwork::changedAtcStationOnlineConnectionStatus);
        connect(this->m_airspace, &CAirspaceMonitor::changedAircraftInRange, this, &CContextNetwork::changedAircraftInRange);
        connect(this->m_airspace, &CAirspaceMonitor::removedAircraft, this, &IContextNetwork::removedAircraft); // DBus
        connect(this->m_airspace, &CAirspaceMonitor::readyForModelMatching, this, &CContextNetwork::readyForModelMatching);
        connect(this->m_airspace, &CAirspaceMonitor::addedAircraft, this, &CContextNetwork::addedAircraft);
    }

    CContextNetwork *CContextNetwork::registerWithDBus(CDBusServer *server)
    {
        if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
        server->addObject(IContextNetwork::ObjectPath(), this);
        return this;
    }

    CContextNetwork::~CContextNetwork()
    {
        this->gracefulShutdown();
    }

    CAircraftSituationList CContextNetwork::remoteAircraftSituations(const CCallsign &callsign) const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftSituations(callsign);
    }

    CAircraftPartsList CContextNetwork::remoteAircraftParts(const CCallsign &callsign, qint64 cutoffTimeBefore) const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftParts(callsign, cutoffTimeBefore);
    }

    int CContextNetwork::remoteAircraftSituationsCount(const CCallsign &callsign) const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftSituationsCount(callsign);
    }

    bool CContextNetwork::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->isRemoteAircraftSupportingParts(callsign);
    }

    CCallsignSet CContextNetwork::remoteAircraftSupportingParts() const
    {
        Q_ASSERT(this->m_airspace);
        return m_airspace->remoteAircraftSupportingParts();
    }

    QList<QMetaObject::Connection> CContextNetwork::connectRemoteAircraftProviderSignals(
        QObject *receiver,
        std::function<void (const CAircraftSituation &)> situationSlot,
        std::function<void (const CAircraftParts &)> partsSlot,
        std::function<void (const CCallsign &)> removedAircraftSlot,
        std::function<void (const BlackMisc::Simulation::CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot)
    {
        Q_ASSERT_X(this->m_airspace, Q_FUNC_INFO, "Missing airspace");
        return this->m_airspace->connectRemoteAircraftProviderSignals(receiver, situationSlot, partsSlot, removedAircraftSlot, aircraftSnapshotSlot);
    }

    void CContextNetwork::gracefulShutdown()
    {
        this->disconnect(); // all signals
        this->m_webReaderSignalConnections.clear(); // disconnect
        this->m_webDataReader->gracefulShutdown();

        if (this->isConnected()) { this->disconnectFromNetwork(); }
        if (this->m_airspace) { this->m_airspace->gracefulShutdown(); }
    }

    CStatusMessage CContextNetwork::connectToNetwork(const CServer &server, int loginMode)
    {
        if (this->isDebugEnabled()) {CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        QString msg;
        if (!server.getUser().isValid())
        {
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, "Invalid user credentials");
        }
        else if (!this->ownAircraft().getIcaoInfo().hasAircraftDesignator())
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
            this->getIContextOwnAircraft()->updateOwnAircraftPilot(server.getUser());
            const CAircraft ownAircraft = this->ownAircraft();
            this->m_network->presetServer(server);
            this->m_network->presetLoginMode(mode);
            this->m_network->presetCallsign(ownAircraft.getCallsign());
            this->m_network->presetIcaoCodes(ownAircraft.getIcaoInfo());
            if (getIContextSimulator())
            {
                this->m_network->presetSimulatorInfo(getIContextSimulator()->getSimulatorPluginInfo());
            }
            else
            {
                this->m_network->presetSimulatorInfo(CSimulatorPluginInfo());
            }
            this->m_network->initiateConnection();
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, "Connection pending " + server.getAddress() + " " + QString::number(server.getPort()));
        }
    }

    CServer CContextNetwork::getConnectedServer() const
    {
        return this->isConnected() ?
               this->m_network->getPresetServer() :
               CServer();
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

    bool CContextNetwork::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        Q_UNUSED(originator;)
        if (commandLine.isEmpty()) { return false; }
        CSimpleCommandParser parser({ ".msg", ".m" });
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) { return false; }
        if (parser.matchesCommand(".msg", ".m"))
        {
            if (!this->getIContextNetwork()->isConnected())
            {
                CLogMessage(this).validationError("Network needs to be connected");
                return false;
            }
            else if (!this->getIContextOwnAircraft())
            {
                CLogMessage(this).validationError("No own aircraft data, no text message can be sent");
                return false;
            }
            if (parser.countParts() < 3)
            {
                CLogMessage(this).validationError("Incorrect message");
                return false;
            }
            QString receiver = parser.part(1).trimmed(); // receiver

            // set receiver
            CSimulatedAircraft ownAircraft(this->getIContextOwnAircraft()->getOwnAircraft());
            if (ownAircraft.getCallsign().isEmpty())
            {
                CLogMessage(this).validationError("No own callsign");
                return false;
            }

            CTextMessage tm;
            tm.setSenderCallsign(ownAircraft.getCallsign());

            if (receiver == "c1" || receiver == "com1")
            {
                tm.setFrequency(ownAircraft.getCom1System().getFrequencyActive());
            }
            else if (receiver == "c2" || receiver == "com2")
            {
                tm.setFrequency(ownAircraft.getCom2System().getFrequencyActive());
            }
            else if (receiver == "u" || receiver == "unicom" || receiver == "uni")
            {
                tm.setFrequency(CPhysicalQuantitiesConstants::FrequencyUnicom());
            }
            else
            {
                bool isNumber;
                double frequencyMhz = receiver.toDouble(&isNumber);
                if (isNumber)
                {
                    CFrequency radioFrequency = CFrequency(frequencyMhz, CFrequencyUnit::MHz());
                    if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
                    {
                        tm.setFrequency(radioFrequency);
                    }
                    else
                    {
                        CLogMessage(this).validationError("Wrong COM frequency for text message");
                        return false;
                    }
                }
                else
                {
                    CCallsign toCallsign(receiver);
                    tm.setRecipientCallsign(toCallsign);
                }
            }

            QString msg(parser.remainingStringAfter(2));
            tm.setMessage(msg);
            if (tm.isEmpty())
            {
                CLogMessage(this).validationError("No text message body");
                return false;
            }
            CTextMessageList tml(tm);
            this->sendTextMessages(tml);
            return true;
        }
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

    CUserList CContextNetwork::getUsersForCallsigns(const CCallsignSet &callsigns) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CUserList users;
        if (callsigns.isEmpty()) return users;
        return this->m_airspace->getUsersForCallsigns(callsigns);
    }

    CUser CContextNetwork::getUserForCallsign(const CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CCallsignSet callsigns;
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

    CClientList CContextNetwork::getOtherClientsForCallsigns(const CCallsignSet &callsigns) const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getOtherClientsForCallsigns(callsigns);
    }

    CServerList CContextNetwork::getVatsimFsdServers() const
    {
        Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing data reader");
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_webDataReader->getVatsimFsdServers();
    }

    CServerList CContextNetwork::getVatsimVoiceServers() const
    {
        Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "Missing data reader");
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_webDataReader->getVatsimVoiceServers();
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
            CLogMessage(this).info("Connected, own aircraft %1") << this->ownAircraft().getCallsignAsString();
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

    void CContextNetwork::ps_simulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const CLength &maxRenderedDistance, const CLength &maxRenderedBoundary)
    {
        // mainly passing changed restrictions from simulator to network
        if (!m_airspace) { return; }
        if (!m_airspace->analyzer()) { return; }
        m_airspace->analyzer()->setSimulatorRenderRestrictionsChanged(restricted, enabled, maxAircraft, maxRenderedDistance, maxRenderedBoundary);
    }

    void CContextNetwork::ps_updateMetars(const BlackMisc::Weather::CMetarSet &metars)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        CLogMessage(this).info("%1 METARs updated") << metars.size();
    }

    void CContextNetwork::ps_checkForSupervisiorTextMessage(const CTextMessageList &messages)
    {
        if (messages.containsPrivateMessages())
        {
            CTextMessageList supMessages(messages.getSupervisorMessages());
            for (const CTextMessage &m : supMessages)
            {
                emit supervisorTextMessageReceived(m);
            }
        }
    }

    const CSimulatedAircraft CContextNetwork::ownAircraft() const
    {
        Q_ASSERT(this->getRuntime());
        Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
        return this->getRuntime()->getCContextOwnAircraft()->getOwnAircraft();
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
        return this->m_airspace->getAircraftInRange();
    }

    int CContextNetwork::getAircraftInRangeCount() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        return this->m_airspace->getAircraftInRangeCount();
    }

    CSimulatedAircraft CContextNetwork::getAircraftInRangeForCallsign(const CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
        return this->m_airspace->getAircraftInRange().findFirstByCallsign(callsign);
    }

    CAtcStation CContextNetwork::getOnlineStationForCallsign(const CCallsign &callsign) const
    {
        if (this->isDebugEnabled()) { BlackMisc::CLogMessage(this, BlackMisc::CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
        return this->m_airspace->getAtcStationsOnline().findFirstByCallsign(callsign);
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

    bool CContextNetwork::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRedering, const BlackMisc::CIdentifier &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enabledForRedering << originator; }
        bool c = this->m_airspace->updateAircraftEnabled(callsign, enabledForRedering, originator);
        if (c)
        {
            CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
            emit this->changedRemoteAircraftEnabled(aircraft, originator);
        }
        return c;
    }

    bool CContextNetwork::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const BlackMisc::CIdentifier &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << model << originator; }
        bool c = this->m_airspace->updateAircraftModel(callsign, model, originator);
        if (c)
        {
            CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
            emit this->changedRemoteAircraftModel(aircraft, originator);
        }
        return c;
    }

    bool CContextNetwork::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates, const BlackMisc::CIdentifier &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enableFastPositonUpdates << originator; }
        bool c = this->m_airspace->updateFastPositionEnabled(callsign, enableFastPositonUpdates, originator);
        if (c)
        {
            CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
            CLogMessage(this).info("Callsign %1 sets fast positions ") << aircraft.getCallsign() << BlackMisc::boolToOnOff(aircraft.fastPositionUpdates());
            emit this->changedFastPositionUpdates(aircraft, originator);
        }
        return c;
    }

    void CContextNetwork::readAtcBookingsFromSource() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT_X(this->m_webDataReader, Q_FUNC_INFO, "missing reader");
        this->m_webDataReader->readAtcBookingsInBackground();
    }

    bool CContextNetwork::updateAircraftRendered(const CCallsign &callsign, bool rendered, const CIdentifier &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << rendered << originator; }
        bool c = this->m_airspace->updateAircraftRendered(callsign, rendered, originator);
        return c;
    }

    void CContextNetwork::updateMarkAllAsNotRendered(const CIdentifier &originator)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << originator; }
        this->m_airspace->updateMarkAllAsNotRendered(originator);
    }

    CAirspaceAircraftSnapshot CContextNetwork::getLatestAirspaceAircraftSnapshot() const
    {
        return this->m_airspace->getLatestAirspaceAircraftSnapshot();
    }

    bool CContextNetwork::isFastPositionSendingEnabled() const
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT(this->m_airspace);
        return m_airspace->isFastPositionSendingEnabled();
    }

    void CContextNetwork::enableFastPositionSending(bool enable)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << enable; }
        Q_ASSERT(this->m_airspace);
        m_airspace->enableFastPositionSending(enable);
    }

    void CContextNetwork::setFastPositionEnabledCallsigns(CCallsignSet &callsigns)
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsigns; }
        Q_ASSERT(this->m_network);
    }

    CCallsignSet CContextNetwork::getFastPositionEnabledCallsigns()
    {
        if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
        Q_ASSERT(this->m_network);
        //! \todo Fast position updates in vatlib
        return CCallsignSet();
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

    CMetar CContextNetwork::getMetar(const BlackMisc::Aviation::CAirportIcaoCode &airportIcaoCode)
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

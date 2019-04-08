/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/airspaceanalyzer.h"
#include "blackcore/airspacemonitor.h"
#include "blackcore/application.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/corefacade.h"
#include "blackcore/vatsim/networkvatlib.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/stringutils.h"
#include "contextnetworkimpl.h"

#include <stdbool.h>
#include <QStringBuilder>
#include <QTimer>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    namespace Context
    {
        CContextNetwork::CContextNetwork(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextNetwork(mode, runtime)
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());
            CContextNetwork::registerHelp();

            // 1. Init by "network driver"
            m_network = new CNetworkVatlib(nullptr, this->getRuntime()->getCContextOwnAircraft(), this);
            connect(m_network, &INetwork::connectionStatusChanged, this, &CContextNetwork::onFsdConnectionStatusChanged);
            connect(m_network, &INetwork::kicked, this, &CContextNetwork::kicked);
            connect(m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::textMessagesReceived);
            connect(m_network, &INetwork::textMessagesReceived, this, &CContextNetwork::checkForSupervisiorTextMessage);
            connect(m_network, &INetwork::textMessageSent, this, &CContextNetwork::textMessageSent);

            // 2. Update timer for data (network data such as frequency)
            m_networkDataUpdateTimer = new QTimer(this);
            connect(m_networkDataUpdateTimer, &QTimer::timeout, this, &CContextNetwork::requestDataUpdates);
            m_networkDataUpdateTimer->start(30 * 1000);

            // 3. Airspace contents
            Q_ASSERT_X(this->getRuntime()->getCContextOwnAircraft(), Q_FUNC_INFO, "this and own aircraft context must be local");
            m_airspace = new CAirspaceMonitor(this->getRuntime()->getCContextOwnAircraft(), m_network, this);
            m_network->setClientProvider(m_airspace);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationsOnline, this, &CContextNetwork::changedAtcStationsOnline, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationsBooked, this, &CContextNetwork::changedAtcStationsBooked, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationOnlineConnectionStatus, this, &CContextNetwork::changedAtcStationOnlineConnectionStatus, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAircraftInRange, this, &CContextNetwork::changedAircraftInRange, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::removedAircraft, this, &IContextNetwork::removedAircraft, Qt::QueuedConnection); // DBus
            connect(m_airspace, &CAirspaceMonitor::readyForModelMatching, this, &CContextNetwork::readyForModelMatching, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::addedAircraft, this, &CContextNetwork::addedAircraft, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtisReceived, this, &CContextNetwork::onChangedAtisReceived, Qt::QueuedConnection);
        }

        CContextNetwork *CContextNetwork::registerWithDBus(BlackMisc::CDBusServer *server)
        {
            if (!server || m_mode != CCoreFacadeConfig::LocalInDBusServer) return this;
            server->addObject(IContextNetwork::ObjectPath(), this);
            return this;
        }

        void CContextNetwork::setSimulationEnvironmentProvider(ISimulationEnvironmentProvider *provider)
        {
            if (m_airspace) { m_airspace->setSimulationEnvironmentProvider(provider); }
            if (m_network) { m_network->setSimulationEnvironmentProvider(provider); }
        }

        CContextNetwork::~CContextNetwork()
        {
            this->gracefulShutdown();
        }

        CAircraftSituationList CContextNetwork::remoteAircraftSituations(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituations(callsign);
        }

        CAircraftSituation CContextNetwork::remoteAircraftSituation(const Aviation::CCallsign &callsign, int index) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituation(callsign, index);
        }

        MillisecondsMinMaxMean CContextNetwork::remoteAircraftSituationsTimestampDifferenceMinMaxMean(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituationsTimestampDifferenceMinMaxMean(callsign);
        }

        CAircraftSituationList CContextNetwork::latestRemoteAircraftSituations() const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->latestRemoteAircraftSituations();
        }

        CAircraftSituationList CContextNetwork::latestOnGroundProviderElevations() const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->latestOnGroundProviderElevations();
        }

        CAircraftPartsList CContextNetwork::remoteAircraftParts(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftParts(callsign);
        }

        int CContextNetwork::remoteAircraftPartsCount(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftPartsCount(callsign);
        }

        int CContextNetwork::remoteAircraftSituationsCount(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituationsCount(callsign);
        }

        bool CContextNetwork::isRemoteAircraftSupportingParts(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->isRemoteAircraftSupportingParts(callsign);
        }

        CCallsignSet CContextNetwork::remoteAircraftSupportingParts() const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSupportingParts();
        }

        Aviation::CAircraftSituationChangeList CContextNetwork::remoteAircraftSituationChanges(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituationChanges(callsign);
        }

        int CContextNetwork::remoteAircraftSituationChangesCount(const CCallsign &callsign) const
        {
            Q_ASSERT(m_airspace);
            return m_airspace->remoteAircraftSituationChangesCount(callsign);
        }

        QList<QMetaObject::Connection> CContextNetwork::connectRemoteAircraftProviderSignals(
            QObject *receiver,
            std::function<void (const CAircraftSituation &)> situationSlot,
            std::function<void (const CCallsign &, const CAircraftParts &)> partsSlot,
            std::function<void (const CCallsign &)> removedAircraftSlot,
            std::function<void (const CAirspaceAircraftSnapshot &)> aircraftSnapshotSlot)
        {
            Q_ASSERT_X(m_airspace, Q_FUNC_INFO, "Missing airspace");
            return m_airspace->connectRemoteAircraftProviderSignals(receiver, situationSlot, partsSlot, removedAircraftSlot, aircraftSnapshotSlot);
        }

        void CContextNetwork::gracefulShutdown()
        {
            this->disconnect(); // all signals
            if (this->isConnected()) { this->disconnectFromNetwork(); }
            if (m_network)  { m_network->setClientProvider(nullptr); }
            if (m_airspace) { m_airspace->gracefulShutdown(); }
        }

        CStatusMessage CContextNetwork::connectToNetwork(const CServer &server, INetwork::LoginMode mode)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            QString msg;
            if (!server.getUser().hasCredentials()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Invalid user credentials"); }
            if (!this->ownAircraft().getAircraftIcaoCode().hasDesignator()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Invalid ICAO data for own aircraft"); }
            if (!CNetworkUtils::canConnect(server, msg, 5000)) { return CStatusMessage(CStatusMessage::SeverityError, msg); }
            if (m_network->isConnected()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Already connected"); }
            if (this->isPendingConnection()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Pending connection, please wait"); }

            m_currentStatus = INetwork::Connecting; // as semaphore we are going to connect
            this->getIContextOwnAircraft()->updateOwnAircraftPilot(server.getUser());
            const CSimulatedAircraft ownAircraft(this->ownAircraft());
            m_network->presetServer(server);

            // Fall back to observer mode, if no simulator is available or not simulating
            if (!CBuildConfig::isLocalDeveloperDebugBuild() && !this->getIContextSimulator()->isSimulatorSimulating())
            {
                CLogMessage(this).info(u"No simulator connected or connected simulator not simulating. Falling back to observer mode");
                mode = INetwork::LoginAsObserver;
            }

            m_currentMode = mode;
            m_network->presetLoginMode(mode);
            m_network->presetCallsign(ownAircraft.getCallsign());
            m_network->presetIcaoCodes(ownAircraft);

            const CSimulatorPluginInfo sim = this->getIContextSimulator() ? this->getIContextSimulator()->getSimulatorPluginInfo() : CSimulatorPluginInfo();
            m_network->presetSimulatorInfo(sim);
            m_network->initiateConnection();
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, u"Connection pending " % server.getAddress() % u' ' % QString::number(server.getPort()));
        }

        CServer CContextNetwork::getConnectedServer() const
        {
            return this->isConnected() ? m_network->getPresetServer() : CServer();
        }

        INetwork::LoginMode CContextNetwork::getLoginMode() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_currentMode;
        }

        CStatusMessage CContextNetwork::disconnectFromNetwork()
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_network->isConnected() || m_network->isPendingConnection())
            {
                m_currentStatus = INetwork::Disconnecting; // as semaphore we are going to disconnect
                m_currentMode = INetwork::LoginNormal;
                m_network->terminateConnection();
                return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, u"Connection terminating");
            }
            else
            {
                return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityWarning, u"Already disconnected");
            }
        }

        bool CContextNetwork::isConnected() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_network->isConnected();
        }

        bool CContextNetwork::isPendingConnection() const
        {
            // if underlying class says pending, we believe it. But not all states (e.g. disconnecting) are covered
            if (m_network->isPendingConnection()) return true;

            // now check out own extra states, e.g. disconnecting
            return INetwork::isPendingStatus(m_currentStatus);
        }

        bool CContextNetwork::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            Q_UNUSED(originator;)
            if (commandLine.isEmpty()) { return false; }
            static const QStringList cmds({ ".msg", ".m", ".altos", ".altoffset", ".wallop", ".watchdog", ".reinit", ".reinitialize" });
            CSimpleCommandParser parser(cmds);
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }
            if (parser.matchesCommand(".msg", ".m"))
            {
                if (!this->getIContextNetwork()->isConnected())
                {
                    CLogMessage(this).validationError(u"Network needs to be connected");
                    return false;
                }
                else if (!this->getIContextOwnAircraft())
                {
                    CLogMessage(this).validationError(u"No own aircraft data, no text message can be sent");
                    return false;
                }
                if (parser.countParts() < 3)
                {
                    CLogMessage(this).validationError(u"Incorrect message");
                    return false;
                }

                // set receiver
                const QString receiver = parser.part(1).trimmed().toLower(); // receiver
                const CSimulatedAircraft ownAircraft(this->getIContextOwnAircraft()->getOwnAircraft());
                if (ownAircraft.getCallsign().isEmpty())
                {
                    CLogMessage(this).validationError(u"No own callsign");
                    return false;
                }

                CTextMessage tm;
                tm.setSenderCallsign(ownAircraft.getCallsign());

                if (receiver.startsWith("c") && receiver.endsWith("1"))
                {
                    tm.setFrequency(ownAircraft.getCom1System().getFrequencyActive());
                }
                else if (receiver.startsWith("c") && receiver.endsWith("2"))
                {
                    tm.setFrequency(ownAircraft.getCom2System().getFrequencyActive());
                }
                else if (receiver == "u" || receiver.startsWith("uni"))
                {
                    tm.setFrequency(CPhysicalQuantitiesConstants::FrequencyUnicom());
                }
                else
                {
                    const CFrequency radioFrequency = CComSystem::parseComFrequency(receiver, CPqString::SeparatorBestGuess);
                    if (!radioFrequency.isNull())
                    {
                        if (CComSystem::isValidCivilAviationFrequency(radioFrequency))
                        {
                            tm.setFrequency(radioFrequency);
                        }
                        else
                        {
                            CLogMessage(this).validationError(u"Wrong COM frequency for text message");
                            return false;
                        }
                    }
                    else
                    {
                        const CCallsign toCallsign(receiver);
                        tm.setRecipientCallsign(toCallsign);
                    }
                }

                const QString msg(parser.remainingStringAfter(2));
                tm.setMessage(msg);
                if (tm.isEmpty())
                {
                    CLogMessage(this).validationError(u"No text message body");
                    return false;
                }
                CTextMessageList tml(tm);
                this->sendTextMessages(tml);
                return true;
            }
            else if (parser.matchesCommand(".altos", ".altoffset"))
            {
                if (!m_airspace) { return false; }
                if (parser.countParts() < 2) { return false; }

                const QString csPart(parser.part(1));
                CCallsign cs;
                if (csPart.contains('?'))
                {
                    cs = IRemoteAircraftProvider::testAltitudeOffsetCallsign(); // wildcard
                }
                else
                {
                    cs = CCallsign(csPart);
                    if (!m_airspace->isAircraftInRange(cs))
                    {
                        CLogMessage(this).validationError(u"Altitude offset unknown callsign");
                        return false;
                    }
                }

                CLength os(CLength::null());
                if (parser.hasPart(2))
                {
                    os.parseFromString(parser.part(2), CPqString::SeparatorBestGuess);
                }

                const bool added = this->testAddAltitudeOffset(cs, os);
                if (added) { CLogMessage(this).info(u"Added altitude offset %1 for %2") << os.valueRoundedWithUnit(2) << cs.asString(); }
                else       { CLogMessage(this).info(u"Removed altitude offset %1") << cs.asString(); }

                return true;
            }
            else if (parser.matchesCommand(".watchdog"))
            {
                if (!m_airspace) { return false; }
                if (parser.countParts() < 2) { return false; }

                const bool watchdog = parser.toBool(1, true);
                m_airspace->enableAnalyzer(watchdog);
                CLogMessage(this).info(u"Enabled watchdog: %1") << boolToYesNo(watchdog);
            }
            else if (parser.matchesCommand(".reinit", ".reinitialize"))
            {
                if (!m_airspace) { return false; }
                const int count = m_airspace->reInitializeAllAircraft();
                if (count > 0)
                {
                    CLogMessage(this).info(u"Re-init %1 aircraft") << count;
                }
            }
            else if (parser.matchesCommand(".wallop"))
            {
                if (parser.countParts() < 2) { return false; }
                if (!m_network) { return false; }
                if (!this->isConnected()) { return false; }
                const QString wallopMsg = parser.part(1).simplified().trimmed();
                m_network->sendWallopMessage(wallopMsg);
                return true;
            }

            return false;
        }

        void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages; }
            m_network->sendTextMessages(textMessages);
        }

        void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << flightPlan; }
            m_network->sendFlightPlan(flightPlan);
            m_network->sendFlightPlanQuery(this->ownAircraft().getCallsign());
        }

        CFlightPlan CContextNetwork::loadFlightPlanFromNetwork(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->loadFlightPlanFromNetwork(callsign);
        }

        CUserList CContextNetwork::getUsers() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getUsers();
        }

        CUserList CContextNetwork::getUsersForCallsigns(const CCallsignSet &callsigns) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CUserList users;
            if (callsigns.isEmpty()) return users;
            return m_airspace->getUsersForCallsigns(callsigns);
        }

        CUser CContextNetwork::getUserForCallsign(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CCallsignSet callsigns;
            callsigns.push_back(callsign);
            const CUserList users = this->getUsersForCallsigns(callsigns);
            if (users.size() < 1) { return CUser(); }
            return users[0];
        }

        CClientList CContextNetwork::getClients() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getClients();
        }

        CClientList CContextNetwork::getClientsForCallsigns(const CCallsignSet &callsigns) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getClientsForCallsigns(callsigns);
        }

        bool CContextNetwork::setOtherClient(const CClient &client)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->setOtherClient(client);
        }

        int CContextNetwork::removeClient(const Aviation::CCallsign &callsign)
        {
            return m_airspace->removeClient(callsign);
        }

        bool CContextNetwork::autoAdjustCientGndCapability(const Aviation::CAircraftSituation &situation)
        {
            return m_airspace->autoAdjustCientGndCapability(situation);
        }

        bool CContextNetwork::addClientGndCapability(const CCallsign &callsign)
        {
            return m_airspace->addClientGndCapability(callsign);
        }

        bool CContextNetwork::setClientGndCapability(const Aviation::CCallsign &callsign, bool supportGndFlag)
        {
            return m_airspace->setClientGndCapability(callsign, supportGndFlag);
        }

        void CContextNetwork::markAsSwiftClient(const CCallsign &callsign)
        {
            m_airspace->markAsSwiftClient(callsign);
        }

        CServerList CContextNetwork::getVatsimFsdServers() const
        {
            Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "Missing data reader");
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return sApp->getWebDataServices()->getVatsimFsdServers();
        }

        CServerList CContextNetwork::getVatsimVoiceServers() const
        {
            Q_ASSERT_X(sApp->getWebDataServices(), Q_FUNC_INFO, "Missing data reader");
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return sApp->getWebDataServices()->getVatsimVoiceServers();
        }

        void CContextNetwork::onFsdConnectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << from << to; }
            const INetwork::ConnectionStatus fromOld = m_currentStatus; // own status cached
            m_currentStatus = to;

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
                CLogMessage(this).info(u"Connected, own aircraft %1") << this->ownAircraft().getCallsignAsString();

                if (m_network)
                {
                    const CServer server = m_network->getPresetServer();
                    emit this->connectedServerChanged(server);
                }
            }

            // send as message
            static const QString chgMsg("Connection status changed from '%1' to '%2'");
            if (to == INetwork::DisconnectedError)
            {
                CLogMessage(this).error(chgMsg) << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to);
            }
            else
            {
                CLogMessage(this).info(chgMsg) << INetwork::connectionStatusToString(from) << INetwork::connectionStatusToString(to);
            }

            // send as own signal
            emit this->connectionStatusChanged(from, to);
        }

        void CContextNetwork::xCtxSimulatorRenderRestrictionsChanged(bool restricted, bool enabled, int maxAircraft, const CLength &maxRenderedDistance)
        {
            // mainly passing changed restrictions from simulator to network
            if (!m_airspace) { return; }
            if (!m_airspace->analyzer()) { return; }
            m_airspace->analyzer()->setSimulatorRenderRestrictionsChanged(restricted, enabled, maxAircraft, maxRenderedDistance);
        }

        void CContextNetwork::xCtxSimulatorStatusChanged(int status)
        {
            const ISimulator::SimulatorStatus simStatus = static_cast<ISimulator::SimulatorStatus>(status);
            if (ISimulator::isAnyConnectedStatus(simStatus))
            {
                const CContextSimulator *sim = this->getRuntime()->getCContextSimulator();
                this->setSimulationEnvironmentProvider(sim ? sim->simulator() : nullptr);
            }
            else
            {
                this->setSimulationEnvironmentProvider(nullptr);
            }
        }

        void CContextNetwork::updateMetars(const BlackMisc::Weather::CMetarList &metars)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CLogMessage(this).info(u"%1 METARs updated") << metars.size();
        }

        void CContextNetwork::onChangedAtisReceived(const CCallsign &callsign)
        {
            Q_UNUSED(callsign);
            m_dsAtcStationsOnlineChanged.inputSignal(); // the ATIS data are stored in the station object
        }

        void CContextNetwork::checkForSupervisiorTextMessage(const CTextMessageList &messages)
        {
            if (messages.containsPrivateMessages())
            {
                CTextMessageList supMessages(messages.getSupervisorMessages());
                for (const CTextMessage &m : supMessages)
                {
                    emit this->supervisorTextMessageReceived(m);
                }
            }
        }

        const CSimulatedAircraft CContextNetwork::ownAircraft() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getCContextOwnAircraft());
            return this->getRuntime()->getCContextOwnAircraft()->getOwnAircraft();
        }

        CAtcStationList CContextNetwork::getAtcStationsOnline(bool recalculateDistance) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAtcStationList stations = m_airspace->getAtcStationsOnline();
            if (!recalculateDistance || !this->getIContextOwnAircraft()) { return stations; }
            stations.calculcateAndUpdateRelativeDistanceAndBearing(this->getIContextOwnAircraft()->getOwnAircraftSituation());
            return stations;
        }

        CAtcStationList CContextNetwork::getClosestAtcStationsOnline(int number) const
        {
            if (!this->getIContextOwnAircraft()) { return CAtcStationList(); }
            const CAircraftSituation ownSituation = this->getIContextOwnAircraft()->getOwnAircraftSituation();
            const CAtcStationList stations = m_airspace->getAtcStationsOnline().findClosest(number, ownSituation);
            return stations;
        }

        CAtcStationList CContextNetwork::getAtcStationsBooked(bool recalculateDistance) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAtcStationList stations = m_airspace->getAtcStationsBooked();
            if (!recalculateDistance || !this->getIContextOwnAircraft()) { return stations; }
            stations.calculcateAndUpdateRelativeDistanceAndBearing(this->getIContextOwnAircraft()->getOwnAircraftSituation());
            return stations;
        }

        CSimulatedAircraftList CContextNetwork::getAircraftInRange() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getAircraftInRange();
        }

        CCallsignSet CContextNetwork::getAircraftInRangeCallsigns() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getAircraftInRangeCallsigns();
        }

        int CContextNetwork::getAircraftInRangeCount() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getAircraftInRangeCount();
        }

        bool CContextNetwork::isAircraftInRange(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->isAircraftInRange(callsign);
        }

        bool CContextNetwork::isVtolAircraft(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->isVtolAircraft(callsign);
        }

        CSimulatedAircraft CContextNetwork::getAircraftInRangeForCallsign(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getAircraftInRangeForCallsign(callsign);
        }

        CAircraftModel CContextNetwork::getAircraftInRangeModelForCallsign(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getAircraftInRangeModelForCallsign(callsign);
        }

        CStatusMessageList CContextNetwork::getReverseLookupMessages(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getReverseLookupMessages(callsign);
        }

        ReverseLookupLogging CContextNetwork::isReverseLookupMessagesEnabled() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->isReverseLookupMessagesEnabled();
        }

        void CContextNetwork::enableReverseLookupMessages(ReverseLookupLogging enable)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << enable; }
            const ReverseLookupLogging revEnabled = m_airspace->isReverseLookupMessagesEnabled();
            if (revEnabled == enable) { return; }
            m_airspace->enableReverseLookupMessages(enable);
            emit CContext::changedLogOrDebugSettings();
        }

        CStatusMessageList CContextNetwork::getAircraftPartsHistory(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getAircraftPartsHistory(callsign);
        }

        CAircraftPartsList CContextNetwork::getRemoteAircraftParts(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->remoteAircraftParts(callsign);
        }

        int CContextNetwork::getRemoteAircraftSupportingPartsCount() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getRemoteAircraftSupportingPartsCount();
        }

        bool CContextNetwork::isAircraftPartsHistoryEnabled() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->isAircraftPartsHistoryEnabled();
        }

        void CContextNetwork::enableAircraftPartsHistory(bool enabled)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << enabled; }
            m_airspace->enableAircraftPartsHistory(enabled);
            emit CContext::changedLogOrDebugSettings();
        }

        int CContextNetwork::aircraftSituationsAdded() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->aircraftSituationsAdded();
        }

        int CContextNetwork::aircraftPartsAdded() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->aircraftPartsAdded();
        }

        qint64 CContextNetwork::situationsLastModified(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->situationsLastModified(callsign);
        }

        qint64 CContextNetwork::partsLastModified(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->partsLastModified(callsign);
        }

        bool CContextNetwork::testAddAltitudeOffset(const CCallsign &callsign, const CLength &offset)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->testAddAltitudeOffset(callsign, offset);
        }

        CAtcStation CContextNetwork::getOnlineStationForCallsign(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getAtcStationsOnline().findFirstByCallsign(callsign);
        }

        CAtcStationList CContextNetwork::getOnlineStationsForFrequency(const CFrequency &frequency, CComSystem::ChannelSpacing channelSpacing) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->getAtcStationsOnline().findIfFrequencyIsWithinSpacing(frequency, channelSpacing);
        }

        bool CContextNetwork::isOnlineStation(const CCallsign &callsign) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->getAtcStationsOnline().containsCallsign(callsign);
        }

        void CContextNetwork::requestDataUpdates()
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!this->isConnected()) { return; }

            this->requestAtisUpdates();
            m_airspace->requestDataUpdates();
        }

        void CContextNetwork::requestAtisUpdates()
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!this->isConnected()) { return; }

            m_airspace->requestAtisUpdates();
        }

        bool CContextNetwork::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enabledForRendering; }
            const bool c = m_airspace->updateAircraftEnabled(callsign, enabledForRendering);
            if (c)
            {
                CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
                Q_ASSERT_X(!aircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "missing callsign");
                emit this->changedRemoteAircraftEnabled(aircraft);
            }
            return c;
        }

        bool CContextNetwork::setAircraftEnabledFlag(const CCallsign &callsign, bool enabledForRendering)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            return m_airspace->setAircraftEnabledFlag(callsign, enabledForRendering);
        }

        bool CContextNetwork::updateAircraftModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << model; }
            const bool c = m_airspace->updateAircraftModel(callsign, model, originator);
            if (c)
            {
                const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
                Q_ASSERT_X(!aircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "missing callsign");
                emit this->changedRemoteAircraftModel(aircraft, originator);
            }
            return c;
        }

        bool CContextNetwork::updateAircraftNetworkModel(const CCallsign &callsign, const CAircraftModel &model, const CIdentifier &originator)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << model; }
            const bool c = m_airspace->updateAircraftNetworkModel(callsign, model, originator);
            if (c)
            {
                const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
                emit this->changedRemoteAircraftModel(aircraft, originator);
            }
            return c;
        }

        bool CContextNetwork::updateFastPositionEnabled(const CCallsign &callsign, bool enableFastPositonUpdates)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enableFastPositonUpdates; }
            const bool c = m_airspace->updateFastPositionEnabled(callsign, enableFastPositonUpdates);
            if (c)
            {
                const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
                CLogMessage(this).info(u"Callsign '%1' fast positions '%2'") << aircraft.getCallsign() << BlackMisc::boolToOnOff(aircraft.fastPositionUpdates());
                emit this->changedFastPositionUpdates(aircraft);
            }
            return c;
        }

        bool CContextNetwork::updateAircraftSupportingGndFLag(const CCallsign &callsign, bool supportGndFlag)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << supportGndFlag; }
            const bool c = m_airspace->setClientGndCapability(callsign, supportGndFlag);
            if (c)
            {
                const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
                CLogMessage(this).info(u"Callsign '%1' set gnd.capability: %2") << aircraft.getCallsign() << boolToOnOff(aircraft.isSupportingGndFlag());
                emit this->changedGndFlagCapability(aircraft);
            }
            return c;
        }

        bool CContextNetwork::updateCG(const Aviation::CCallsign &callsign, const CLength &cg)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << cg.valueRoundedWithUnit(1); }
            const bool c = m_airspace->updateCG(callsign, cg);
            return c;
        }

        bool CContextNetwork::updateCGAndModelString(const CCallsign &callsign, const CLength &cg, const QString &modelString)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << cg.valueRoundedWithUnit(1) << modelString; }
            const bool c = m_airspace->updateCGAndModelString(callsign, cg, modelString);
            return c;
        }

        void CContextNetwork::requestAtcBookingsUpdate() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            m_airspace->requestAtcBookingsUpdate();
        }

        bool CContextNetwork::updateAircraftRendered(const CCallsign &callsign, bool rendered)
        {
            const bool c = m_airspace->updateAircraftRendered(callsign, rendered);
            return c;
        }

        int CContextNetwork::updateMultipleAircraftRendered(const CCallsignSet &callsigns, bool rendered)
        {
            const int c = m_airspace->updateMultipleAircraftRendered(callsigns, rendered);
            return c;
        }

        int CContextNetwork::updateMultipleAircraftEnabled(const CCallsignSet &callsigns, bool enabled)
        {
            const int c = m_airspace->updateMultipleAircraftEnabled(callsigns, enabled);
            return c;
        }

        int CContextNetwork::updateAircraftGroundElevation(const CCallsign &callsign, const CElevationPlane &elevation, CAircraftSituation::GndElevationInfo info)
        {
            return m_airspace->updateAircraftGroundElevation(callsign, elevation, info);
        }

        void CContextNetwork::updateMarkAllAsNotRendered()
        {
            m_airspace->updateMarkAllAsNotRendered();
        }

        int CContextNetwork::reInitializeAllAircraft()
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->reInitializeAllAircraft();
        }

        CAirspaceAircraftSnapshot CContextNetwork::getLatestAirspaceAircraftSnapshot() const
        {
            return m_airspace->getLatestAirspaceAircraftSnapshot();
        }

        CElevationPlane CContextNetwork::averageElevationOfNonMovingAircraft(const CAircraftSituation &reference, const CLength &range, int minValues) const
        {
            return m_airspace->averageElevationOfNonMovingAircraft(reference, range, minValues);
        }

        void CContextNetwork::setClients(const CClientList &clients)
        {
            m_airspace->setClients(clients);
        }

        void CContextNetwork::clearClients()
        {
            m_airspace->clearClients();
        }

        CClient CContextNetwork::getClientOrDefaultForCallsign(const Aviation::CCallsign &callsign) const
        {
            return m_airspace->getClientOrDefaultForCallsign(callsign);
        }

        bool CContextNetwork::hasClientInfo(const Aviation::CCallsign &callsign) const
        {
            return m_airspace->hasClientInfo(callsign);
        }

        bool CContextNetwork::addNewClient(const CClient &client)
        {
            return m_airspace->addNewClient(client);
        }

        int CContextNetwork::updateOrAddClient(const Aviation::CCallsign &callsign, const CPropertyIndexVariantMap &vm, bool skipEqualValues)
        {
            return m_airspace->updateOrAddClient(callsign, vm, skipEqualValues);
        }

        void CContextNetwork::setFastPositionEnabledCallsigns(CCallsignSet &callsigns)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsigns; }
            Q_ASSERT(m_network);
            m_network->setInterimPositionReceivers(callsigns);
        }

        CCallsignSet CContextNetwork::getFastPositionEnabledCallsigns() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            Q_ASSERT(m_network);
            return m_network->getInterimPositionReceivers();
        }

        QString CContextNetwork::getLibraryInfo(bool detailed) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << detailed; }
            Q_ASSERT(m_network);
            return m_network->getLibraryInfo(detailed);
        }

        void CContextNetwork::testRequestAircraftConfig(const CCallsign &callsign)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            Q_ASSERT(m_network);
            m_network->sendAircraftConfigQuery(callsign);
        }

        void CContextNetwork::testCreateDummyOnlineAtcStations(int number)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << number; }
            m_airspace->testCreateDummyOnlineAtcStations(number);
        }

        void CContextNetwork::testAddAircraftParts(const CCallsign &callsign, const CAircraftParts &parts, bool incremental)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << parts << incremental; }
            m_airspace->testAddAircraftParts(callsign, parts, incremental);
        }

        void CContextNetwork::testReceivedAtisMessage(const CCallsign &callsign, const CInformationMessage &msg)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign.asString(); }
            if (this->network())
            {
                emit this->network()->atisReplyReceived(callsign, msg);
            }
        }

        void CContextNetwork::testReceivedTextMessages(const CTextMessageList &textMessages)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages.toQString(); }
            if (this->network())
            {
                emit this->network()->textMessagesReceived(textMessages);
            }
        }

        CMetar CContextNetwork::getMetarForAirport(const CAirportIcaoCode &airportIcaoCode) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << airportIcaoCode; }
            return sApp->getWebDataServices()->getMetarForAirport(airportIcaoCode);
        }

        CAtcStationList CContextNetwork::getSelectedAtcStations() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            CAtcStation com1Station = m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom1System());
            CAtcStation com2Station = m_airspace->getAtcStationForComUnit(this->ownAircraft().getCom2System());

            CAtcStationList selectedStations;
            selectedStations.push_back(com1Station);
            selectedStations.push_back(com2Station);
            return selectedStations;
        }

        CVoiceRoomList CContextNetwork::getSelectedVoiceRooms() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            const CAtcStationList stations = this->getSelectedAtcStations();
            Q_ASSERT(stations.size() == 2);
            CVoiceRoomList rooms;
            const CAtcStation s1 = stations[0];
            const CAtcStation s2 = stations[1];
            rooms.push_back(s1.getVoiceRoom());
            rooms.push_back(s2.getVoiceRoom());
            return rooms;
        }

        QMetaObject::Connection CContextNetwork::connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot)
        {
            Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

            // bind does not allow to define connection type, so we use receiver as workaround
            const QMetaObject::Connection uc; // unconnected
            const QMetaObject::Connection c = rawFsdMessageReceivedSlot ? connect(m_network, &INetwork::rawFsdMessageReceived, receiver, rawFsdMessageReceivedSlot) : uc;
            Q_ASSERT_X(c || !rawFsdMessageReceivedSlot, Q_FUNC_INFO, "connect failed");
            return c;
        }
    } // namespace
} // namespace

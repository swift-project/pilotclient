/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextsimulatorimpl.h"
#include "blackcore/airspaceanalyzer.h"
#include "blackcore/airspacemonitor.h"
#include "blackcore/application.h"
#include "blackcore/corefacade.h"
#include "blackcore/fsd/fsdclient.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/stringutils.h"
#include "blackconfig/buildconfig.h"
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
using namespace BlackCore::Fsd;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    namespace Context
    {
        CContextNetwork::CContextNetwork(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) :
            IContextNetwork(mode, runtime)
        {
            //! \fixme KB 2019-07 bad style we implicitly depend on 2 other contexts
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getIContextOwnAircraft());
            Q_ASSERT(this->getIContextOwnAircraft()->isUsingImplementingObject());
            CContextNetwork::registerHelp();

            // 1. Init by "network driver"
            m_fsdClient = new CFSDClient(this, this->getRuntime()->getCContextOwnAircraft(), this, this);
            connect(m_fsdClient, &CFSDClient::connectionStatusChanged, this, &CContextNetwork::onFsdConnectionStatusChanged);
            connect(m_fsdClient, &CFSDClient::killRequestReceived,     this, &CContextNetwork::kicked);
            connect(m_fsdClient, &CFSDClient::textMessagesReceived,    this, &CContextNetwork::textMessagesReceived);
            connect(m_fsdClient, &CFSDClient::textMessageSent,         this, &CContextNetwork::textMessageSent);
            connect(m_fsdClient, &CFSDClient::severeNetworkError,      this, &CContextNetwork::severeNetworkError);
            // connect(m_fsdClient, &FSDClient::textMessagesReceived,     this, &CContextNetwork::checkForSupervisiorTextMessage);

            // 2. Update timer for data (network data such as frequency)
            // we use 2 timers so we can query at different times (not too many queirs at once)
            m_requestAircraftDataTimer = new QTimer(this);
            connect(m_requestAircraftDataTimer, &QTimer::timeout, this,  &CContextNetwork::requestAircraftDataUpdates);
            m_requestAircraftDataTimer->start(30 * 1000);
            m_requestAircraftDataTimer->setObjectName("CContextNetwork::m_requestAircraftDataTimer");

            m_requestAtisTimer = new QTimer(this);
            connect(m_requestAtisTimer, &QTimer::timeout, this,  &CContextNetwork::requestAtisUpdates);
            m_requestAtisTimer->start(13 * 1000); // should not be called at the same time as above
            m_requestAtisTimer->setObjectName("CContextNetwork::m_requestAtisTimer");

            // 3. Airspace contents
            Q_ASSERT_X(this->getRuntime()->getCContextOwnAircraft(), Q_FUNC_INFO, "this and own aircraft context must be local");
            Q_ASSERT_X(this->getRuntime()->getCContextSimulator(),   Q_FUNC_INFO, "this and own simulator context must be local");
            m_airspace = new CAirspaceMonitor(
                this->getRuntime()->getCContextOwnAircraft(),
                this->getRuntime()->getCContextSimulator(),
                m_fsdClient, this);
            m_fsdClient->setClientProvider(m_airspace);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationsOnline, this, &CContextNetwork::changedAtcStationsOnline, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationsBooked, this, &CContextNetwork::changedAtcStationsBooked, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtcStationOnlineConnectionStatus, this, &CContextNetwork::changedAtcStationOnlineConnectionStatus, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAircraftInRange, this, &CContextNetwork::changedAircraftInRange, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::removedAircraft,        this, &IContextNetwork::removedAircraft, Qt::QueuedConnection); // DBus
            connect(m_airspace, &CAirspaceMonitor::readyForModelMatching,  this, &CContextNetwork::readyForModelMatching, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::addedAircraft,          this, &CContextNetwork::addedAircraft, Qt::QueuedConnection);
            connect(m_airspace, &CAirspaceMonitor::changedAtisReceived   , this, &CContextNetwork::onChangedAtisReceived, Qt::QueuedConnection);
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
            if (m_fsdClient) { m_fsdClient->setSimulationEnvironmentProvider(provider); }
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
            if (m_fsdClient)  { m_fsdClient->setClientProvider(nullptr); }
            if (m_airspace) { m_airspace->gracefulShutdown(); }
        }

        CStatusMessage CContextNetwork::connectToNetwork(const CServer &server, const QString &extraLiveryString, bool sendLivery, const QString &extraModelString, bool sendModelString, const CCallsign &partnerCallsign, CLoginMode mode)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            QString msg;
            if (!server.getUser().hasCredentials()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Invalid user credentials"); }
            if (!this->ownAircraft().getAircraftIcaoCode().hasDesignator()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Invalid ICAO data for own aircraft"); }
            if (!CNetworkUtils::canConnect(server, msg, 5000)) { return CStatusMessage(CStatusMessage::SeverityError, msg); }
            if (m_fsdClient->isConnected()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Already connected"); }
            if (this->isPendingConnection()) { return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityError, u"Pending connection, please wait"); }

            this->getIContextOwnAircraft()->updateOwnAircraftPilot(server.getUser());
            const CSimulatedAircraft ownAircraft(this->ownAircraft());
            m_fsdClient->setPartnerCallsign(partnerCallsign);

            // Fall back to observer mode, if no simulator is available or not simulating
            if (!CBuildConfig::isLocalDeveloperDebugBuild() && !this->getIContextSimulator()->isSimulatorSimulating())
            {
                CLogMessage(this).info(u"No simulator connected or connected simulator not simulating. Falling back to observer mode");
                mode.setLoginMode(CLoginMode::Observer);
            }

            const QString l = extraLiveryString.isEmpty() ?  ownAircraft.getModel().getSwiftLiveryString() : extraLiveryString;
            const QString m = extraModelString.isEmpty()  ?  ownAircraft.getModelString() : extraModelString;

            m_currentMode = mode;
            m_fsdClient->setLoginMode(mode);
            m_fsdClient->setCallsign(ownAircraft.getCallsign());
            m_fsdClient->setIcaoCodes(ownAircraft);
            m_fsdClient->setLiveryAndModelString(l, sendLivery, m, sendModelString);
            m_fsdClient->setClientName(sApp->swiftVersionChar());
            m_fsdClient->setVersion(CBuildConfig::getVersion().majorVersion(), CBuildConfig::getVersion().minorVersion());

            int clientId = 0;
            QString clientKey;
            if (!getCmdLineClientIdAndKey(clientId, clientKey))
            {
                clientId = CBuildConfig::vatsimClientId();
                clientKey = CBuildConfig::vatsimPrivateKey();
            }

            m_fsdClient->setClientIdAndKey(clientId, clientKey.toLocal8Bit());
            m_fsdClient->setClientCapabilities(Capabilities::AircraftInfo | Capabilities::FastPos | Capabilities::AtcInfo | Capabilities::AircraftConfig);
            m_fsdClient->setServer(server);

            const CSimulatorPluginInfo sim = this->getIContextSimulator() ? this->getIContextSimulator()->getSimulatorPluginInfo() : CSimulatorPluginInfo();
            m_fsdClient->setSimType(sim);
            m_fsdClient->setPilotRating(PilotRating::Student);
            m_fsdClient->setAtcRating(AtcRating::Observer);

            m_fsdClient->connectToServer();
            return CStatusMessage({ CLogCategory::validation() }, CStatusMessage::SeverityInfo, u"Connection pending " % server.getAddress() % u' ' % QString::number(server.getPort()));
        }

        CServer CContextNetwork::getConnectedServer() const
        {
            return this->isConnected() ? m_fsdClient->getServer() : CServer();
        }

        CLoginMode CContextNetwork::getLoginMode() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_fsdClient->getLoginMode();
        }

        CStatusMessage CContextNetwork::disconnectFromNetwork()
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (m_fsdClient->isConnected() || m_fsdClient->isPendingConnection())
            {
                m_fsdClient->disconnectFromServer();
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
            return m_fsdClient->isConnected();
        }

        bool CContextNetwork::isPendingConnection() const
        {
            return m_fsdClient->isPendingConnection();
        }

        bool CContextNetwork::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
        {
            Q_UNUSED(originator;)
            if (commandLine.isEmpty()) { return false; }
            static const QStringList cmds({ ".msg", ".m", ".chat", ".altos", ".altoffset", ".addtimeos", ".addtimeoffset", ".wallop", ".watchdog", ".reinit", ".reinitialize", ".enable", ".disable", ".ignore", ".unignore" });
            CSimpleCommandParser parser(cmds);
            parser.parse(commandLine);
            if (!parser.isKnownCommand()) { return false; }
            if (parser.matchesCommand(".msg", ".m", ".chat"))
            {
                if (!this->getIContextNetwork() || !this->getIContextNetwork()->isConnected())
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

                // based on the CPZ bug https://discordapp.com/channels/539048679160676382/539486309882789888/576765888401768449
                // no longer use starts/ends with
                if (receiver == QStringView(u"c1") || receiver == QStringView(u"com1") || receiver == QStringView(u"comm1"))
                {
                    tm.setFrequency(ownAircraft.getCom1System().getFrequencyActive());
                }
                else if (receiver == QStringView(u"c2") || receiver == QStringView(u"com2") || receiver == QStringView(u"comm2"))
                {
                    tm.setFrequency(ownAircraft.getCom2System().getFrequencyActive());
                }
                else if (receiver == "u" || receiver == QStringView(u"uni") || receiver == QStringView(u"unicom"))
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
            else if (parser.matchesCommand(".addtimeos", ".addtimeoffset"))
            {
                if (!m_airspace) { return false; }
                if (parser.countParts() < 2) { return false; }

                CTime os(CTime::null());
                if (parser.hasPart(2))
                {
                    os.parseFromString(parser.part(2), CPqString::SeparatorBestGuess);
                }

                if (!os.isNull() && os.isPositiveWithEpsilonConsidered())
                {
                    const qint64 ost = os.valueInteger(CTimeUnit::ms());
                    CLogMessage(this).info(u"Added add offset time %1ms") << ost;
                }
                else
                {
                    CLogMessage(this).info(u"Reset add. time offset");
                }
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
                if (!m_fsdClient) { return false; }
                if (!this->isConnected()) { return false; }
                const QString wallopMsg = parser.part(1).simplified().trimmed();
                m_fsdClient->sendTextMessage(TextMessageGroups::AllSups, wallopMsg);
                return true;
            }
            else if (parser.matchesCommand(".enable", ".unignore"))
            {
                if (parser.countParts() < 2) { return false; }
                if (!m_fsdClient) { return false; }
                if (!this->isConnected()) { return false; }
                const CCallsign cs(parser.part(1));
                if (cs.isValid()) { this->updateAircraftEnabled(cs, true); }
            }
            else if (parser.matchesCommand(".disable", ".ignore"))
            {
                if (parser.countParts() < 2) { return false; }
                if (!m_fsdClient) { return false; }
                if (!this->isConnected()) { return false; }
                const CCallsign cs(parser.part(1));
                if (cs.isValid()) { this->updateAircraftEnabled(cs, false); }
            }

            return false;
        }

        void CContextNetwork::sendTextMessages(const CTextMessageList &textMessages)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages; }
            m_fsdClient->sendTextMessages(textMessages);
        }

        void CContextNetwork::sendFlightPlan(const CFlightPlan &flightPlan)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << flightPlan; }
            m_fsdClient->sendFlightPlan(flightPlan);
            m_fsdClient->sendClientQueryFlightPlan(this->ownAircraft().getCallsign());
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

        void CContextNetwork::onFsdConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
        {
            // if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << from << to; }

            if (to.isDisconnected())
            {
                // make sure airspace is really cleaned up
                Q_ASSERT(m_airspace);
                m_airspace->clear();
            }

            // send 1st position
            if (to.isConnected())
            {
                CLogMessage(this).info(u"Connected, own aircraft %1") << this->ownAircraft().getCallsignAsString();

                if (m_fsdClient)
                {
                    const CServer server = m_fsdClient->getServer();
                    emit this->connectedServerChanged(server);
                }
            }

            // send as message
            static const QString chgMsg("Connection status changed from '%1' to '%2'");
            CLogMessage(this).info(chgMsg) << from.toQString() << to.toQString();

            // send as own signal
            emit this->connectionStatusChanged(from, to);
        }

        void CContextNetwork::createRelayMessageToPartnerCallsign(const CTextMessage &textMessage, const CCallsign &partnerCallsign, CTextMessageList &relayedMessages)
        {
            if (textMessage.isEmpty())     { return; }
            if (partnerCallsign.isEmpty()) { return; }
            if (textMessage.getSenderCallsign() == partnerCallsign) { return; } // no round trips

            CTextMessage modified(textMessage);
            modified.makeRelayedMessage(partnerCallsign);
            relayedMessages.push_back(modified);
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

        void CContextNetwork::onTextMessagesReceived(const CTextMessageList &messages)
        {
            const CTextMessageList textMessages = messages.withRelayedToPrivateMessages();
            emit this->textMessagesReceived(textMessages);

            if (textMessages.containsPrivateMessages())
            {
                const CTextMessageList supMessages(messages.getSupervisorMessages());
                for (const CTextMessage &m : supMessages)
                {
                    emit this->supervisorTextMessageReceived(m);
                }

                // part to send to partner "forward"
                if (m_fsdClient && !m_fsdClient->getPresetPartnerCallsign().isEmpty())
                {
                    const CCallsign partnerCallsign = m_fsdClient->getPresetPartnerCallsign();

                    // IMPORTANT: use messages AND NOT textMessages here, exclude messages from partner to avoid infinite roundtrips
                    CTextMessageList relayedMessages;
                    const CTextMessageList privateMessages = messages.getPrivateMessages().withRemovedPrivateMessagesFromCallsign(partnerCallsign);
                    for (const CTextMessage &m : privateMessages)
                    {
                        this->createRelayMessageToPartnerCallsign(m, partnerCallsign, relayedMessages);
                    }

                    if (!relayedMessages.isEmpty())
                    {
                        QPointer<CContextNetwork> myself(this);
                        QTimer::singleShot(10, this, [ = ]
                        {
                            if (myself) { myself->sendTextMessages(relayedMessages); }
                        });
                    }
                } // relay to partner
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
            emit IContext::changedLogOrDebugSettings();
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
            emit IContext::changedLogOrDebugSettings();
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

        QString CContextNetwork::getNetworkStatistics(bool reset, const QString &separator)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_fsdClient) { return QString(); }
            return m_fsdClient->getNetworkStatisticsAsText(reset, separator);
        }

        bool CContextNetwork::setNetworkStatisticsEnable(bool enabled)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!m_fsdClient) { return false; }
            return m_fsdClient->setStatisticsEnable(enabled);
        }

        bool CContextNetwork::testAddAltitudeOffset(const CCallsign &callsign, const CLength &offset)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            return m_airspace->testAddAltitudeOffset(callsign, offset);
        }

        QStringList CContextNetwork::getNetworkPresetValues() const
        {
            if (!m_fsdClient) { return {}; }
            return m_fsdClient->getPresetValues();
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

        void CContextNetwork::requestAircraftDataUpdates()
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!this->isConnected()) { return; }

            m_airspace->requestAircraftDataUpdates();
            if (m_requestAircraftDataTimer) { m_requestAircraftDataTimer->start(); } // restart
        }

        void CContextNetwork::requestAtisUpdates()
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            if (!this->isConnected()) { return; }

            m_airspace->requestAtisUpdates();
            if (m_requestAtisTimer) { m_requestAtisTimer->start(); } // restart
        }

        bool CContextNetwork::updateAircraftEnabled(const CCallsign &callsign, bool enabledForRendering)
        {
            Q_ASSERT(m_airspace);
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign << enabledForRendering; }
            const bool c = m_airspace->updateAircraftEnabled(callsign, enabledForRendering);
            if (c)
            {
                const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
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
                emit this->changedRemoteAircraftModel(aircraft, originator); // update aircraft model
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
                emit this->changedRemoteAircraftModel(aircraft, originator); // updated network model
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

        CLength CContextNetwork::getCGFromDB(const CCallsign &callsign) const
        {
            return m_airspace->getCGFromDB(callsign);
        }

        CLength CContextNetwork::getCGFromDB(const QString &modelString) const
        {
            return m_airspace->getCGFromDB(modelString);
        }

        void CContextNetwork::rememberCGFromDB(const CLength &cgFromDB, const CCallsign &callsign)
        {
            m_airspace->rememberCGFromDB(cgFromDB, callsign);
        }

        void CContextNetwork::rememberCGFromDB(const CLength &cgFromDB, const QString &modelString)
        {
            m_airspace->rememberCGFromDB(cgFromDB, modelString);
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
            Q_ASSERT(m_fsdClient);
            m_fsdClient->setInterimPositionReceivers(callsigns);
        }

        CCallsignSet CContextNetwork::getFastPositionEnabledCallsigns() const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO; }
            Q_ASSERT(m_fsdClient);
            return m_fsdClient->getInterimPositionReceivers();
        }

        QString CContextNetwork::getLibraryInfo(bool detailed) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << detailed; }
            Q_ASSERT(m_fsdClient);
            return "";
        }

        void CContextNetwork::testRequestAircraftConfig(const CCallsign &callsign)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << callsign; }
            Q_ASSERT(m_fsdClient);
            m_fsdClient->sendClientQueryAircraftConfig(callsign);
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
            if (this->fsdClient())
            {
                emit this->fsdClient()->atisReplyReceived(callsign, msg);
            }
        }

        void CContextNetwork::testReceivedTextMessages(const CTextMessageList &textMessages)
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << textMessages.toQString(); }
            if (this->fsdClient())
            {
                emit this->fsdClient()->textMessagesReceived(textMessages);
            }
        }

        CMetar CContextNetwork::getMetarForAirport(const CAirportIcaoCode &airportIcaoCode) const
        {
            if (this->isDebugEnabled()) { CLogMessage(this, CLogCategory::contextSlot()).debug() << Q_FUNC_INFO << airportIcaoCode; }
            if (!sApp || !sApp->getWebDataServices()) { return {}; }
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

        QMetaObject::Connection CContextNetwork::connectRawFsdMessageSignal(QObject *receiver, RawFsdMessageReceivedSlot rawFsdMessageReceivedSlot)
        {
            Q_ASSERT_X(receiver, Q_FUNC_INFO, "Missing receiver");

            // bind does not allow to define connection type, so we use receiver as workaround
            const QMetaObject::Connection uc; // unconnected
            const QMetaObject::Connection c = rawFsdMessageReceivedSlot ? connect(m_fsdClient, &CFSDClient::rawFsdMessage, receiver, rawFsdMessageReceivedSlot) : uc;
            Q_ASSERT_X(c || !rawFsdMessageReceivedSlot, Q_FUNC_INFO, "connect failed");
            return c;
        }
    } // namespace
} // namespace

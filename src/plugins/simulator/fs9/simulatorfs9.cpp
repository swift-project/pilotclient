// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfs9.h"

#include <algorithm>

#include <QPointer>
#include <QTimer>

#include "../fscommon/simulatorfscommonfunctions.h"
#include "directplayerror.h"
#include "fs9.h"
#include "fs9client.h"
#include "multiplayerpacketparser.h"
#include "multiplayerpackets.h"
#include "registermetadata.h"

#include "config/buildconfig.h"
#include "misc/logmessage.h"
#include "misc/network/textmessage.h"
#include "misc/propertyindexallclasses.h"
#include "misc/simulation/fscommon/fscommonutil.h"
#include "misc/simulation/simulatorplugininfo.h"
#include "misc/verify.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::fscommon;
using namespace swift::core;
using namespace swift::simplugin::fscommon;
using namespace swift::config;

namespace swift::simplugin::fs9
{
    CSimulatorFs9::CSimulatorFs9(const CSimulatorPluginInfo &info, const QSharedPointer<CFs9Host> &fs9Host,
                                 const QSharedPointer<CLobbyClient> &lobbyClient,
                                 IOwnAircraftProvider *ownAircraftProvider,
                                 IRemoteAircraftProvider *remoteAircraftProvider, IClientProvider *clientProvider,
                                 QObject *parent)
        : CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent),
          m_fs9Host(fs9Host), m_lobbyClient(lobbyClient), m_fsuipc(new CFsuipc(this))
    {
        // disabled CG/elevation parts
        this->setSimulationProviderEnabled(false, false);

        //! \fixme KB 7/2017 change or remove comment when reviewed Could we just use: connect(lobbyClient.data(),
        //! &CLobbyClient::disconnected, this, &CSimulatorFs9::disconnectFrom);
        connect(lobbyClient.data(), &CLobbyClient::disconnected, this, [=] { this->emitSimulatorCombinedStatus(); });

        this->setDefaultModel({ "Boeing 737-400", CAircraftModel::TypeModelMatchingDefaultModel,
                                "B737-400 default model", CAircraftIcaoCode("B734", "L2J") });
    }

    bool CSimulatorFs9::isConnected() const { return m_simConnected; }

    bool CSimulatorFs9::connectTo()
    {
        Q_ASSERT_X(m_fs9Host, Q_FUNC_INFO, "No FS9 host");
        if (!m_fs9Host->isConnected()) { return false; } // host not available, we quit

        Q_ASSERT_X(m_fsuipc, Q_FUNC_INFO, "No FSUIPC");
        m_connectionHostMessages =
            connect(m_fs9Host.data(), &CFs9Host::customPacketReceived, this, &CSimulatorFs9::processFs9Message);

        m_fsuipc->open();

        this->initSimulatorInternals();
        m_timerId = startTimer(50);
        return true;
    }

    bool CSimulatorFs9::disconnectFrom()
    {
        if (!m_simConnected) { return true; }

        // Don't forward messages when disconnected
        disconnect(m_connectionHostMessages);
        safeKillTimer();
        disconnectAllClients();

        if (m_fsuipc) { m_fsuipc->close(); }

        //  emit status
        CSimulatorFsCommon::disconnectFrom();
        m_simConnected = false;
        emitSimulatorCombinedStatus();
        return true;
    }

    bool CSimulatorFs9::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
    {
        const CCallsign callsign = newRemoteAircraft.getCallsign();
        this->logAddingAircraftModel(newRemoteAircraft);

        if (m_hashFs9Clients.contains(callsign))
        {
            // already exists, remove first
            this->physicallyRemoveRemoteAircraft(callsign);
        }

        CFs9Client *client =
            new CFs9Client(newRemoteAircraft, CTime(25, CTimeUnit::ms()), &m_interpolationLogger, this);
        client->setHostAddress(m_fs9Host->getHostAddress());
        client->setPlayerUserId(m_fs9Host->getPlayerUserId());
        connect(client, &CFs9Client::statusChanged, this, &CSimulatorFs9::updateRenderStatus);
        client->start();

        m_hashFs9Clients.insert(callsign, client);
        return true;
    }

    bool CSimulatorFs9::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        if (!m_hashFs9Clients.contains(callsign)) { return false; }

        auto fs9Client = m_hashFs9Clients.value(callsign);
        delete fs9Client;
        m_hashFs9Clients.remove(callsign);
        updateAircraftRendered(callsign, false);
        CLogMessage(this).info(u"FS9: Removed aircraft '%1'") << callsign.toQString();
        return CSimulatorFsCommon::physicallyRemoveRemoteAircraft(callsign);
    }

    int CSimulatorFs9::physicallyRemoveAllRemoteAircraft()
    {
        if (m_hashFs9Clients.isEmpty()) { return 0; }
        QList<CCallsign> callsigns(m_hashFs9Clients.keys());
        int r = 0;
        for (const CCallsign &cs : callsigns)
        {
            if (physicallyRemoveRemoteAircraft(cs)) { r++; }
        }

        CSimulatorFsCommon::physicallyRemoveAllRemoteAircraft();
        return r;
    }

    CCallsignSet CSimulatorFs9::physicallyRenderedAircraft() const
    {
        return CCollection<CCallsign>(m_hashFs9Clients.keys());
    }

    bool CSimulatorFs9::updateOwnSimulatorCockpit(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
    {
        if (originator == this->identifier()) { return false; }
        if (!this->isSimulating()) { return false; }

        // actually those data should be the same as ownAircraft
        const CTransponder newTransponder = ownAircraft.getTransponder();

        bool changed = false;
        if (newTransponder.getTransponderCode() != m_simTransponder.getTransponderCode()) { changed = true; }
        if (newTransponder.getTransponderMode() != m_simTransponder.getTransponderMode()) { changed = true; }

        m_fsuipc->write(newTransponder);

        // avoid changes of cockpit back to old values due to an outdated read back value
        if (changed) { m_skipCockpitUpdateCycles = SkipUpdateCyclesForCockpit; }

        // bye
        return changed;
    }

    bool CSimulatorFs9::updateOwnSimulatorSelcal(const CSelcal &selcal, const CIdentifier &originator)
    {
        if (originator == this->identifier()) { return false; }
        if (!this->isSimulating()) { return false; }

        bool changed = false;
        if (selcal != m_selcal)
        {
            //! KB 2018-11 that would need to go to updateOwnAircraftFromSimulator if the simulator ever supports SELCAL
            //! KB 2018-11 as we would need to send the value to FS9/FSX (currently we only deal with it on FS9/FSX
            //! level)
            m_selcal = selcal;
            changed = true;
        }

        return changed;
    }

    void CSimulatorFs9::displayStatusMessage(const CStatusMessage &message) const
    {
        if (!m_fs9Host.data()) { return; }

        // Avoid errors from CDirectPlayPeer as it may end in infinite loop
        if (message.getSeverity() == CStatusMessage::SeverityError && message.isFromClass<CDirectPlayPeer>())
        {
            return;
        }

        if (message.getSeverity() != CStatusMessage::SeverityDebug)
        {
            QMetaObject::invokeMethod(m_fs9Host.data(), "sendTextMessage", Q_ARG(QString, message.toQString()));
        }
    }

    void CSimulatorFs9::displayTextMessage(const CTextMessage &message) const
    {
        if (!m_fs9Host.data()) { return; }
        QMetaObject::invokeMethod(m_fs9Host.data(), "sendTextMessage", Q_ARG(QString, message.asString(true, true)));
    }

    CStatusMessageList CSimulatorFs9::getInterpolationMessages(const CCallsign &callsign) const
    {
        if (!m_hashFs9Clients.contains(callsign)) { return CStatusMessageList(); }
        const CFs9Client *client = m_hashFs9Clients[callsign].data();
        if (!client) { return CStatusMessageList(); }
        const CInterpolationAndRenderingSetupPerCallsign setup =
            this->getInterpolationSetupPerCallsignOrDefault(callsign);
        return client->getInterpolationMessages(setup.getInterpolatorMode());
    }

    bool CSimulatorFs9::testSendSituationAndParts(const CCallsign &callsign, const CAircraftSituation &situation,
                                                  const CAircraftParts &parts)
    {
        if (!m_hashFs9Clients.contains(callsign)) { return false; }
        CFs9Client *client = m_hashFs9Clients[callsign].data();
        if (!client) { return false; }

        Q_UNUSED(parts)
        int u = 0;
        if (!situation.isNull())
        {
            u++;
            client->sendMultiplayerPosition(situation);
            if (!parts.isNull()) { client->sendMultiplayerParts(parts); }
        }
        return u > 0;
    }

    bool CSimulatorFs9::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
    {
        return m_hashFs9Clients.contains(callsign);
    }

    void CSimulatorFs9::timerEvent(QTimerEvent *event)
    {
        Q_UNUSED(event)
        dispatch();
    }

    void CSimulatorFs9::dispatch()
    {
        if (m_fsuipc && m_fsuipc->isOpened())
        {
            CSimulatedAircraft fsuipcAircraft(getOwnAircraft());
            const bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
            if (ok) { updateOwnAircraftFromSimulator(fsuipcAircraft); }
            else
            {
                // FSUIPC read error means almost always that FS9 closed. Shutdown the driver.
                CLogMessage(this).debug() << "Lost connection to FSUIPC. Shutting down.";
                disconnectFrom();
            }
            synchronizeTime();
        }
    }

    QString getChangedParamsAsString(const MPParam &old, const MPParam &newParam)
    {
        // for debugging
        QString str;
        if (old.unknown8 != newParam.unknown8) str += "unknown8 " + QString::number(newParam.unknown8) + "\n";
        if (old.unknown9 != newParam.unknown9) str += "unknown9 " + QString::number(newParam.unknown9) + "\n";
        if (old.flaps_left != newParam.flaps_left) str += "flaps_left " + QString::number(newParam.flaps_left) + "\n";
        if (old.flaps_right != newParam.flaps_right)
            str += "flaps_right " + QString::number(newParam.flaps_right) + "\n";
        if (old.unknown12 != newParam.unknown12) str += "unknown12 " + QString::number(newParam.unknown12) + "\n";
        if (old.unknown13 != newParam.unknown13) str += "unknown13 " + QString::number(newParam.unknown13) + "\n";
        if (old.unknown14 != newParam.unknown14) str += "unknown14 " + QString::number(newParam.unknown14) + "\n";
        if (old.unknown15 != newParam.unknown15) str += "unknown15 " + QString::number(newParam.unknown15) + "\n";
        if (old.unknown16 != newParam.unknown16) str += "unknown16 " + QString::number(newParam.unknown16) + "\n";
        if (old.unknown17 != newParam.unknown17) str += "unknown17 " + QString::number(newParam.unknown17) + "\n";
        if (old.unknown18 != newParam.unknown18) str += "unknown18 " + QString::number(newParam.unknown18) + "\n";
        if (old.unknown19 != newParam.unknown19) str += "unknown19 " + QString::number(newParam.unknown19) + "\n";
        if (old.gear_center != newParam.gear_center)
            str += "gear_center " + QString::number(newParam.gear_center) + "\n";
        if (old.gear_left != newParam.gear_left) str += "gear_left " + QString::number(newParam.gear_left) + "\n";
        if (old.gear_right != newParam.gear_right) str += "gear_right " + QString::number(newParam.gear_right) + "\n";
        if (old.engine_1 != newParam.engine_1) str += "engine_1 " + QString::number(newParam.engine_1) + "\n";
        if (old.engine_2 != newParam.engine_2) str += "engine_2 " + QString::number(newParam.engine_2) + "\n";
        if (old.unknown25 != newParam.unknown25) str += "unknown25 " + QString::number(newParam.unknown25) + "\n";
        if (old.unknown26 != newParam.unknown26) str += "unknown26 " + QString::number(newParam.unknown26) + "\n";
        if (old.unknown27 != newParam.unknown27) str += "unknown27 " + QString::number(newParam.unknown27) + "\n";
        return str;
    }

    void CSimulatorFs9::processFs9Message(const QByteArray &message)
    {
        if (!m_simConnected)
        {
            m_simConnected = true;
            emitSimulatorCombinedStatus();
        }
        CFs9Sdk::MULTIPLAYER_PACKET_ID messageType = MultiPlayerPacketParser::readType(message);
        switch (messageType)
        {
        case CFs9Sdk::MULTIPLAYER_PACKET_ID_PARAMS:
        {
            MPParam mpParam;
            MultiPlayerPacketParser::readMessage(message, mpParam);
            // For debugging:
            // QTextStream qtstdout(stdout);
            // QString paramString = getChangedParamsAsString(m_lastParameters, mpParam);
            // if (! paramString.isEmpty())
            // {
            //     qtstdout << message.mid(4 * sizeof(qint32)).toHex() << Qt::endl;
            //     qtstdout << paramString << Qt::endl;
            // }
            // m_lastParameters = mpParam;
            break;
        }
        case CFs9Sdk::MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE:
        {
            MPChangePlayerPlane mpChangePlayerPlane;
            MultiPlayerPacketParser::readMessage(message, mpChangePlayerPlane);
            reverseLookupAndUpdateOwnAircraftModel(mpChangePlayerPlane.aircraft_name);
            break;
        }
        case CFs9Sdk::MULTIPLAYER_PACKET_ID_POSITION_VELOCITY:
        {
            break;
        }
        case CFs9Sdk::MPCHAT_PACKET_ID_CHAT_TEXT_SEND:
        {
            MPChatText mpChatText;
            MultiPlayerPacketParser::readMessage(message, mpChatText);
            break;
        }
        default: break;
        }
    }

    void CSimulatorFs9::updateOwnAircraftFromSimulator(const CSimulatedAircraft &simDataOwnAircraft)
    {
        // When I change cockpit values in the sim (from GUI to simulator, not originating from simulator)
        // it takes a little while before these values are set in the simulator.
        // To avoid jitters, I wait some update cylces to stabilize the values
        if (m_skipCockpitUpdateCycles < 1)
        {
            // we always use COM1 and COM2 from swift
            const CComSystem oldCom1 = getOwnComSystem(CComSystem::Com1);
            const CComSystem oldCom2 = getOwnComSystem(CComSystem::Com2);

            this->updateCockpit(oldCom1, oldCom2, simDataOwnAircraft.getTransponder(), this->identifier());
        }
        else { --m_skipCockpitUpdateCycles; }

        const CAircraftSituation aircraftSituation = simDataOwnAircraft.getSituation();
        this->updateOwnSituationAndGroundElevation(aircraftSituation);

        this->updateOwnParts(simDataOwnAircraft.getParts());

        // slow updates
        if (m_ownAircraftUpdateCycles % 25 == 0)
        {
            this->reverseLookupAndUpdateOwnAircraftModel(simDataOwnAircraft.getModelString());
            const CLength cg = simDataOwnAircraft.getCG();
            if (!cg.isNull()) { this->updateOwnCG(cg); }
        } // slow updates

        m_ownAircraftUpdateCycles++;
    }

    void CSimulatorFs9::updateRenderStatus(const CSimulatedAircraft &remoteAircraft, CFs9Client::ClientStatus)
    {
        const bool updated = updateAircraftRendered(remoteAircraft.getCallsign(), true);
        CSimulatedAircraft remoteAircraftCopy(remoteAircraft);
        remoteAircraftCopy.setRendered(true);
        if (updated) { emit aircraftRenderingChanged(remoteAircraftCopy); }
        CLogMessage(this).info(u"FS9: Added aircraft '%1'") << remoteAircraft.getCallsignAsString();
    }

    void CSimulatorFs9::disconnectAllClients()
    {
        // Stop all FS9 client tasks
        const QList<CCallsign> callsigns(m_hashFs9Clients.keys());
        for (auto fs9Client : callsigns) { physicallyRemoveRemoteAircraft(fs9Client); }
    }

    void CSimulatorFs9::synchronizeTime()
    {
        if (!m_simTimeSynced) { return; }
        if (!this->isConnected()) { return; }
        if (!m_fsuipc) { return; }
        if (!m_fsuipc->isOpened()) { return; }

        QDateTime myDateTime = QDateTime::currentDateTimeUtc();
        if (!m_syncTimeOffset.isZeroEpsilonConsidered())
        {
            int offsetSeconds = m_syncTimeOffset.valueInteger(CTimeUnit::s());
            myDateTime = myDateTime.addSecs(offsetSeconds);
        }

        const QTime myTime = myDateTime.time();
        const int h = myTime.hour();
        const int m = myTime.minute();
        m_fsuipc->setSimulatorTime(h, m);
    }

    CSimulatorFs9Listener::CSimulatorFs9Listener(const CSimulatorPluginInfo &info,
                                                 const QSharedPointer<CFs9Host> &fs9Host,
                                                 const QSharedPointer<CLobbyClient> &lobbyClient)
        : ISimulatorListener(info), m_timer(new QTimer(this)), m_fs9Host(fs9Host), m_lobbyClient(lobbyClient),
          m_fsuipc(new CFsuipc(this))
    {
        const int QueryInterval = 5 * 1000; // 5 seconds
        m_timer->setInterval(QueryInterval);
        m_timer->setObjectName(this->objectName() + ":m_timer");

        // Test whether we can lobby connect at all.
        const bool canLobbyConnect = m_lobbyClient->canLobbyConnect();

        // check connection
        QPointer<CSimulatorFs9Listener> myself(this);
        connect(m_timer, &QTimer::timeout, [=]() {
            if (!myself) { return; }
            this->checkConnection(canLobbyConnect);
        });
    }

    void CSimulatorFs9Listener::startImpl()
    {
        m_isStarted = false;
        m_timer->start();
    }

    void CSimulatorFs9Listener::stopImpl() { m_timer->stop(); }

    void CSimulatorFs9Listener::checkImpl()
    {
        if (this->isShuttingDown()) { return; }
        if (m_timer) { m_timer->start(); }

        QPointer<CSimulatorFs9Listener> myself(this);
        QTimer::singleShot(10, this, [=] {
            if (!myself) { return; }
            const bool canLobbyConnect = m_lobbyClient->canLobbyConnect();
            this->checkConnection(canLobbyConnect);
        });
    }

    bool CSimulatorFs9Listener::checkConnection(bool canLobbyConnect)
    {
        m_fsuipc->open();
        if (!m_fsuipc->isOpen()) { return false; }
        m_fsuipc->close();

        if (m_fs9Host->getHostAddress().isEmpty()) { return false; } // host not yet set up
        if (canLobbyConnect)
        {
            if (m_isConnecting || isOk(m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress())))
            {
                m_isConnecting = true;
                CLogMessage(this).info(u"swift is joining FS9 to the multiplayer session ...");
            }
        }

        if (!m_isStarted && m_fs9Host->isConnected())
        {
            m_isStarted = true;
            m_isConnecting = false;
            emit this->simulatorStarted(this->getPluginInfo());
        }
        return m_isConnecting;
    }

    static void cleanupFs9Host(CFs9Host *host) { delete host; }

    static void cleanupLobbyClient(CLobbyClient *lobbyClient) { delete lobbyClient; }

    CSimulatorFs9Factory::CSimulatorFs9Factory(QObject *parent)
        : QObject(parent), m_fs9Host(new CFs9Host, cleanupFs9Host), m_lobbyClient(new CLobbyClient, cleanupLobbyClient)
    {
        registerMetadata();

        /* After FS9 is disconnected, reset its data stored in the host */
        connect(m_lobbyClient.data(), &CLobbyClient::disconnected, m_fs9Host.data(), &CFs9Host::reset);
    }

    CSimulatorFs9Factory::~CSimulatorFs9Factory() {}

    ISimulator *CSimulatorFs9Factory::create(const CSimulatorPluginInfo &info,
                                             IOwnAircraftProvider *ownAircraftProvider,
                                             IRemoteAircraftProvider *remoteAircraftProvider,
                                             IClientProvider *clientProvider)
    {
        return new CSimulatorFs9(info, m_fs9Host, m_lobbyClient, ownAircraftProvider, remoteAircraftProvider,
                                 clientProvider, this);
    }

    ISimulatorListener *CSimulatorFs9Factory::createListener(const CSimulatorPluginInfo &info)
    {
        return new CSimulatorFs9Listener(info, m_fs9Host, m_lobbyClient);
    }
} // namespace swift::simplugin::fs9

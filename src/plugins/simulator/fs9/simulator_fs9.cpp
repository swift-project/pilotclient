/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "fs9.h"
#include "blacksimplugin_freefunctions.h"
#include "simulator_fs9.h"
#include "fs9_host.h"
#include "fs9_client.h"
#include "multiplayer_packets.h"
#include "multiplayer_packet_parser.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/project.h"
#include <QTimer>
#include <algorithm>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackSim;
using namespace BlackSimPlugin::Fs9;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        BlackCore::ISimulator *CSimulatorFs9Factory::create(QObject *parent)
        {
            registerMetadata();
            return new Fs9::CSimulatorFs9(parent);
        }

        BlackSim::CSimulatorInfo CSimulatorFs9Factory::getSimulatorInfo() const
        {
            return CSimulatorInfo::FS9();
        }

        CSimulatorFs9::CSimulatorFs9(QObject *parent) :
            ISimulator(parent),
            m_fs9Host(new CFs9Host),
            m_hostThread(this),
            m_lobbyClient(new CLobbyClient(this)),
            m_simulatorInfo(CSimulatorInfo::FS9()),
            m_fsuipc(new FsCommon::CFsuipc())
        {
            // We move the host thread already in the constructor
            m_fs9Host->moveToThread(&m_hostThread);
            connect(&m_hostThread, &QThread::started, m_fs9Host, &CFs9Host::init);
            connect(m_fs9Host, &CFs9Host::customPacketReceived, this, &CSimulatorFs9::ps_processFs9Message);
            connect(m_fs9Host, &CFs9Host::statusChanged, this, &CSimulatorFs9::ps_changeHostStatus);
            connect(&m_hostThread, &QThread::finished, m_fs9Host, &CFs9Host::deleteLater);
            connect(&m_hostThread, &QThread::finished, &m_hostThread, &QThread::deleteLater);
            m_hostThread.start();
        }

        CSimulatorFs9::~CSimulatorFs9()
        {
            disconnectFrom();
            m_hostThread.quit();
            m_hostThread.wait(1000);

            QList<QThread *> clientThreads = m_fs9ClientThreads.values();
            for (QThread *clientThread : clientThreads) clientThread->wait();
        }

        bool CSimulatorFs9::isConnected() const
        {
            return m_fs9Host->isConnected();
        }

        bool CSimulatorFs9::connectTo()
        {
            m_fsuipc->connect(); // connect FSUIPC too

            // If we are already hosting, connect FS0 through lobby connection
            if (m_isHosting) m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress());
            // If not, deferre connection until host is setup
            else m_startedLobbyConnection = true;

            return true;
        }

        void CSimulatorFs9::asyncConnectTo()
        {
            // Since we are running the host in its own thread, it is async anyway
            connectTo();
        }

        bool CSimulatorFs9::disconnectFrom()
        {
            disconnectAllClients();

            // We tell the host to terminate and stop the thread afterwards
            QMetaObject::invokeMethod(m_fs9Host, "stopHosting");
            emit statusChanged(ISimulator::Disconnected);
            m_fsuipc->disconnect();

            return false;
        }

        bool CSimulatorFs9::canConnect()
        {
            return true;
        }

        void CSimulatorFs9::addRemoteAircraft(const CCallsign &callsign, const BlackMisc::Aviation::CAircraftSituation &initialSituation)
        {
            if (m_hashFs9Clients.size() >= 20)
            {
                // Only add a maximum number of 20 clients.
                // FIXME: We need a smart method to get the 20 nearest aircrafts. If someone logs in
                // nearby we need to kick out the one with max distance.
                return;
            }

            // Create a new client thread, set update frequency to 25 ms and start it
            QThread *clientThread = new QThread(this);
            CFs9Client *client = new CFs9Client(callsign.toQString(), CTime(25, CTimeUnit::ms()));
            client->setHostAddress(m_fs9Host->getHostAddress());
            client->setPlayerUserId(m_fs9Host->getPlayerUserId());
            client->moveToThread(clientThread);

            connect(clientThread, &QThread::started, client, &CFs9Client::init);
            connect(client, &CFs9Client::clientTimedOut, this, &CSimulatorFs9::ps_removeAircraft);
            connect(client, &CFs9Client::statusChanged, this, &CSimulatorFs9::ps_changeClientStatus);
            m_fs9ClientThreads.insert(client, clientThread);
            m_hashFs9Clients.insert(callsign, client);
            clientThread->start();

            addAircraftSituation(callsign, initialSituation);
        }

        void CSimulatorFs9::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &situation)
        {
            // FIXME: should be a Q_ASSERT
            if (!m_hashFs9Clients.contains(callsign))
            {
                // Only add a maximum number of 20 clients.
                // FIXME: We need a smart method to get the 20 nearest aircrafts. If someone logs in
                // nearby we need to kick out the one with max distance.
                return;
            }

            CFs9Client *client = m_hashFs9Clients.value(callsign);
            if (!client)
                return;

            client->addAircraftSituation(situation);
        }

        void CSimulatorFs9::removeRemoteAircraft(const CCallsign &callsign)
        {
            ps_removeAircraft(callsign.toQString());
        }

        bool CSimulatorFs9::updateOwnSimulatorCockpit(const CAircraft &ownAircraft)
        {
            CComSystem newCom1 = ownAircraft.getCom1System();
            CComSystem newCom2 = ownAircraft.getCom2System();
            CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1 != this->m_ownAircraft.getCom1System())
            {
                if (newCom1.getFrequencyActive() != this->m_ownAircraft.getCom1System().getFrequencyActive())
                {

                }
                if (newCom1.getFrequencyStandby() != this->m_ownAircraft.getCom1System().getFrequencyStandby())
                {

                }

                this->m_ownAircraft.setCom1System(newCom1);
                changed = true;
            }

            if (newCom2 != this->m_ownAircraft.getCom2System())
            {
                if (newCom2.getFrequencyActive() != this->m_ownAircraft.getCom2System().getFrequencyActive())
                {

                }

                if (newCom2.getFrequencyStandby() != this->m_ownAircraft.getCom2System().getFrequencyStandby())
                {

                }

                this->m_ownAircraft.setCom2System(newCom2);
                changed = true;
            }

            if (newTransponder != this->m_ownAircraft.getTransponder())
            {
                if (newTransponder.getTransponderCode() != this->m_ownAircraft.getTransponder().getTransponderCode())
                {
                    changed = true;
                }
                this->m_ownAircraft.setTransponder(newTransponder);
            }

            // bye
            return changed;
        }

        CSimulatorInfo CSimulatorFs9::getSimulatorInfo() const
        {
            return this->m_simulatorInfo;
        }

        void CSimulatorFs9::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            QMetaObject::invokeMethod(m_fs9Host, "sendTextMessage", Q_ARG(QString, message.toQString()));
        }

        void CSimulatorFs9::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        CAirportList CSimulatorFs9::getAirportsInRange() const
        {
            return this->m_airportsInRange;
        }

        void CSimulatorFs9::setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime offset)
        {
            this->m_syncTime = enable;
            this->m_syncTimeOffset = offset;
        }

        void CSimulatorFs9::timerEvent(QTimerEvent * /* event */)
        {
            ps_dispatch();
        }

        void CSimulatorFs9::ps_dispatch()
        {
            if (m_fsuipc) m_fsuipc->process();
            updateOwnAircraftFromSim(m_fsuipc->getOwnAircraft());
        }

        void CSimulatorFs9::ps_processFs9Message(const QByteArray &message)
        {
            CFs9Sdk::MULTIPLAYER_PACKET_ID messageType = MultiPlayerPacketParser::readType(message);

            switch (messageType)
            {
                case CFs9Sdk::MULTIPLAYER_PACKET_ID_CHANGE_PLAYER_PLANE:
                {
                    MPChangePlayerPlane mpChangePlayerPlane;
                    MultiPlayerPacketParser::readMessage(message, mpChangePlayerPlane);
                    ps_changeOwnAircraftModel(mpChangePlayerPlane.aircraft_name);
                    break;
                }
                case CFs9Sdk::MULTIPLAYER_PACKET_ID_POSITION_VELOCITY:
                {
                    MPPositionVelocity mpPositionVelocity;
                    MultiPlayerPacketParser::readMessage(message, mpPositionVelocity);
                    m_ownAircraft.setSituation(aircraftSituationfromFS9(mpPositionVelocity));
                    break;
                }
                case CFs9Sdk::MPCHAT_PACKET_ID_CHAT_TEXT_SEND:
                {
                    MPChatText mpChatText;
                    MultiPlayerPacketParser::readMessage(message, mpChatText);
                    break;
                }

                default:
                    break;
            }
        }

        void CSimulatorFs9::ps_changeOwnAircraftModel(const QString &modelname)
        {
            m_aircraftModel.setQueriedModelString(modelname);
            emit aircraftModelChanged(m_aircraftModel);
        }

        void CSimulatorFs9::ps_changeHostStatus(BlackSimPlugin::Fs9::CFs9Host::HostStatus status)
        {
            switch (status)
            {
                case CFs9Host::Hosting:
                {
                    m_isHosting = true;
                    startTimer(50);
                    emit statusChanged(Connected);
                    if (m_startedLobbyConnection)
                    {
                        m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress());
                        m_startedLobbyConnection = false;
                    }
                    break;
                }
                case CFs9Host::Terminated:
                {
                    m_fs9Host->deleteLater();
                    qDebug() << "Quitting thread";
                    connect(&m_hostThread, &QThread::finished, &m_hostThread, &QThread::deleteLater);
                    m_hostThread.quit();
                    m_isHosting = false;
                    emit statusChanged(Disconnected);
                    break;
                }
                default:
                    break;
            }
        }

        void CSimulatorFs9::ps_changeClientStatus(const QString &callsign, CFs9Client::ClientStatus status)
        {
            switch (status)
            {
                case CFs9Client::Disconnected:
                {
                    CFs9Client *client = m_hashFs9Clients.value(callsign);
                    Q_ASSERT(m_fs9ClientThreads.contains(client));
                    QThread *clientThread = m_fs9ClientThreads.value(client);

                    // Cleanup
                    client->deleteLater();
                    connect(clientThread, &QThread::finished, clientThread, &QThread::deleteLater);
                    clientThread->quit();

                    m_fs9ClientThreads.remove(client);
                    m_hashFs9Clients.remove(callsign);
                    break;
                }
            default:
                break;
            }
        }

        void CSimulatorFs9::ps_removeAircraft(const QString &callsign)
        {
            if(!m_hashFs9Clients.contains(callsign)) return;

            CFs9Client *fs9Client = m_hashFs9Clients.value(callsign);

            // Send an async disconnect signal. When finished we will clean up
            QMetaObject::invokeMethod(fs9Client, "disconnectFrom");
        }

        void CSimulatorFs9::updateOwnAircraftFromSim(const CAircraft &ownAircraft)
        {
            m_ownAircraft.setCom1System(ownAircraft.getCom1System());
            m_ownAircraft.setCom2System(ownAircraft.getCom2System());
            m_ownAircraft.setTransponder(ownAircraft.getTransponder());
        }

        void CSimulatorFs9::disconnectAllClients()
        {
            // Stop all FS9 client threads
            for (auto fs9Client : m_hashFs9Clients.keys())
            {
                removeRemoteAircraft(fs9Client);
            }
        }
    }
}

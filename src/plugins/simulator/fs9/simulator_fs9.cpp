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
#include "blackmisc/propertyindexallclasses.h"
#include <QTimer>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
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
            m_fs9Host(new CFs9Host(this)),
            m_lobbyClient(new CLobbyClient(this)),
            m_simulatorInfo(CSimulatorInfo::FS9()),
            m_fsuipc(new FsCommon::CFsuipc())
        {
            connect(m_fs9Host.data(), &CFs9Host::customPacketReceived, this, &CSimulatorFs9::ps_processFs9Message);
            connect(m_fs9Host.data(), &CFs9Host::statusChanged, this, &CSimulatorFs9::ps_changeHostStatus);
            m_fs9Host->start();
        }

        CSimulatorFs9::~CSimulatorFs9()
        {
            Q_ASSERT(!m_isHosting);
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

            emit connectionStatusChanged(ISimulator::Disconnected);
            if (m_fs9Host) { m_fs9Host->quit(); }
            m_fsuipc->disconnect();

            m_isHosting = false;

            return true;
        }

        void CSimulatorFs9::addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            CCallsign callsign = remoteAircraft.getCallsign();
            CFs9Client *client = new CFs9Client(this, callsign.toQString(), CTime(25, CTimeUnit::ms()));
            client->setHostAddress(m_fs9Host->getHostAddress());
            client->setPlayerUserId(m_fs9Host->getPlayerUserId());

            client->start();
            m_hashFs9Clients.insert(callsign, client);
            m_remoteAircraft.replaceOrAdd(&CSimulatedAircraft::getCallsign, remoteAircraft.getCallsign(), remoteAircraft);
            addAircraftSituation(callsign, remoteAircraft.getSituation());
        }

        void CSimulatorFs9::addAircraftSituation(const CCallsign &callsign, const CAircraftSituation &situation)
        {
            Q_ASSERT(m_hashFs9Clients.contains(callsign));

            CFs9Client *client = m_hashFs9Clients.value(callsign);
            if (!client)
                return;

            client->addAircraftSituation(situation);
        }

        int CSimulatorFs9::removeRemoteAircraft(const CCallsign &callsign)
        {
            if (!m_hashFs9Clients.contains(callsign)) { return 0; }

            auto fs9Client = m_hashFs9Clients.value(callsign);
            fs9Client->quit();
            m_hashFs9Clients.remove(callsign);
            return m_remoteAircraft.removeIf(&CSimulatedAircraft::getCallsign, callsign);
        }

        int CSimulatorFs9::changeRemoteAircraft(const CSimulatedAircraft &changedAircraft, const CPropertyIndexVariantMap &changedValues)
        {
            return m_remoteAircraft.incrementalUpdateOrAdd(changedAircraft, changedValues);
            //! \todo really update aircraft in SIM
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
            if (message.getSeverity() != BlackMisc::CStatusMessage::SeverityDebug)
            {
                if (m_fs9Host)
                    QMetaObject::invokeMethod(m_fs9Host, "sendTextMessage", Q_ARG(QString, message.toQString()));
            }
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

        CPixmap CSimulatorFs9::iconForModel(const QString &modelString) const
        {
            Q_UNUSED(modelString);
            return CPixmap();
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
            case CFs9Sdk::MULTIPLAYER_PACKET_ID_PARAMS:
                {
                    break;
                }
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
            CAircraftModel model = m_ownAircraft.getModel();
            model.setModelString(modelname);
            m_ownAircraft.setModel(model);
            emit ownAircraftModelChanged(m_ownAircraft);
        }

        void CSimulatorFs9::ps_changeHostStatus(BlackSimPlugin::Fs9::CFs9Host::HostStatus status)
        {
            switch (status)
            {
            case CFs9Host::Hosting:
                {
                    m_isHosting = true;
                    startTimer(50);
                    emit connectionStatusChanged(Connected);
                    if (m_startedLobbyConnection)
                    {
                        m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress());
                        m_startedLobbyConnection = false;
                    }
                    break;
                }
            case CFs9Host::Terminated:
                {
                    m_isHosting = false;
                    emit connectionStatusChanged(Disconnected);
                    break;
                }
            default:
                break;
            }
        }

        void CSimulatorFs9::updateOwnAircraftFromSim(const CAircraft &ownAircraft)
        {
            m_ownAircraft.setCom1System(ownAircraft.getCom1System());
            m_ownAircraft.setCom2System(ownAircraft.getCom2System());
            m_ownAircraft.setTransponder(ownAircraft.getTransponder());
        }

        void CSimulatorFs9::disconnectAllClients()
        {
            // Stop all FS9 client tasks
            for (auto fs9Client : m_hashFs9Clients.keys())
            {
                removeRemoteAircraft(fs9Client);
            }
        }
    } // namespace
} // namespace

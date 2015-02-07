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
#include "blackmisc/logmessage.h"
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
using namespace BlackSimPlugin::FsCommon;


namespace BlackSimPlugin
{
    namespace Fs9
    {
        BlackCore::ISimulator *CSimulatorFs9Factory::create(
            IOwnAircraftProvider *ownAircraftProvider,
            IRenderedAircraftProvider *renderedAircraftProvider,
            QObject *parent)
        {
            registerMetadata();
            return new Fs9::CSimulatorFs9(ownAircraftProvider, renderedAircraftProvider, parent);
        }

        BlackSim::CSimulatorInfo CSimulatorFs9Factory::getSimulatorInfo() const
        {
            return CSimulatorInfo::FS9();
        }

        CSimulatorFs9::CSimulatorFs9(IOwnAircraftProvider *ownAircraftProvider, IRenderedAircraftProvider *renderedAircraftProvider, QObject *parent) :
            CSimulatorFsCommon(CSimulatorInfo::FS9(), ownAircraftProvider, renderedAircraftProvider, parent),
            m_fs9Host(new CFs9Host(this)), m_lobbyClient(new CLobbyClient(this))
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
            Q_ASSERT(m_fsuipc);
            m_fsuipc->connect(); // connect FSUIPC too

            // If we are already hosting, connect FS0 through lobby connection
            if (m_isHosting)
            {
                m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress());
            }
            // If not, deferre connection until host is setup
            else
            {
                m_startedLobbyConnection = true;
            }
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
            CSimulatorFsCommon::disconnectFrom();
            m_isHosting = false;
            return true;
        }

        bool CSimulatorFs9::addRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
        {
            CCallsign callsign = remoteAircraft.getCallsign();
            if (m_hashFs9Clients.contains(callsign))
            {
                // already exists, remove first
                this->removeRenderedAircraft(callsign);
            }

            CFs9Client *client = new CFs9Client(this->m_renderedAircraftProvider, this, callsign.toQString(), CTime(25, CTimeUnit::ms()));
            client->setHostAddress(m_fs9Host->getHostAddress());
            client->setPlayerUserId(m_fs9Host->getPlayerUserId());

            client->start();
            m_hashFs9Clients.insert(callsign, client);
            renderedAircraft().applyIfCallsign(callsign, CPropertyIndexVariantMap(CSimulatedAircraft::IndexRendered, CVariant::fromValue(true)));
            CLogMessage(this).info("FS9: Added aircraft %1") << callsign.toQString();
            return true;
        }

        bool CSimulatorFs9::removeRenderedAircraft(const CCallsign &callsign)
        {
            if (!m_hashFs9Clients.contains(callsign)) { return false; }

            auto fs9Client = m_hashFs9Clients.value(callsign);
            fs9Client->quit();
            m_hashFs9Clients.remove(callsign);
            renderedAircraft().applyIfCallsign(callsign, CPropertyIndexVariantMap(CSimulatedAircraft::IndexRendered, CVariant::fromValue(false)));
            CLogMessage(this).info("FS9: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        bool CSimulatorFs9::updateOwnSimulatorCockpit(const CAircraft &ownAircraft, const QString &originator)
        {
            if (originator == this->simulatorOriginator()) { return false; }
            if (!this->isSimulating()) { return false; }

            // actually those data should be the same as ownAircraft
            CComSystem newCom1 = ownAircraft.getCom1System();
            CComSystem newCom2 = ownAircraft.getCom2System();
            CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1.getFrequencyActive() != this->m_simCom1.getFrequencyActive())
            {
                // CFrequency newFreq = newCom1.getFrequencyActive();
                changed = true;

            }
            if (newCom1.getFrequencyStandby() != this->m_simCom1.getFrequencyStandby())
            {
                // CFrequency newFreq = newCom1.getFrequencyStandby();
                changed = true;
            }

            if (newCom2.getFrequencyActive() != this->m_simCom2.getFrequencyActive())
            {
                // CFrequency newFreq = newCom2.getFrequencyActive();
                changed = true;
            }
            if (newCom2.getFrequencyStandby() != this->m_simCom2.getFrequencyStandby())
            {
                // CFrequency newFreq = newCom2.getFrequencyStandby();
                changed = true;
            }

            if (newTransponder.getTransponderCode() != this->m_simTransponder.getTransponderCode())
            {
                changed = true;
            }

            if (newTransponder.getTransponderMode() != this->m_simTransponder.getTransponderMode())
            {
            }

            // avoid changes of cockpit back to old values due to an outdated read back value

            // bye
            return changed;
        }

        void CSimulatorFs9::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            if (message.getSeverity() != BlackMisc::CStatusMessage::SeverityDebug)
            {
                if (m_fs9Host)
                {
                    QMetaObject::invokeMethod(m_fs9Host, "sendTextMessage", Q_ARG(QString, message.toQString()));
                }
            }
        }

        void CSimulatorFs9::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        void CSimulatorFs9::timerEvent(QTimerEvent * /* event */)
        {
            ps_dispatch();
        }

        void CSimulatorFs9::ps_dispatch()
        {
            if (m_fsuipc)
            {
                CSimulatedAircraft fsuipcAircraft(ownAircraft());
                bool ok = m_fsuipc->read(fsuipcAircraft);
                if (ok)
                {
                    updateOwnAircraftFromSimulator(fsuipcAircraft);
                }
            }
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
                    setOwnAircraftModel(mpChangePlayerPlane.aircraft_name);
                    break;
                }
            case CFs9Sdk::MULTIPLAYER_PACKET_ID_POSITION_VELOCITY:
                {
                    MPPositionVelocity mpPositionVelocity;
                    MultiPlayerPacketParser::readMessage(message, mpPositionVelocity);
                    ownAircraft().setSituation(aircraftSituationfromFS9(mpPositionVelocity));
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

        void CSimulatorFs9::updateOwnAircraftFromSimulator(const CAircraft &simDataOwnAircraft)
        {
            this->providerUpdateCockpit(
                simDataOwnAircraft.getCom1System(),
                simDataOwnAircraft.getCom2System(),
                simDataOwnAircraft.getTransponder(),
                this->simulatorOriginator());
        }

        void CSimulatorFs9::disconnectAllClients()
        {
            // Stop all FS9 client tasks
            for (auto fs9Client : m_hashFs9Clients.keys())
            {
                removeRenderedAircraft(fs9Client);
            }
        }
    } // namespace
} // namespace

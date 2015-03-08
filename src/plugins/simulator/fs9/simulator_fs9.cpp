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
#include "blackcore/interpolator_linear.h"
#include "blacksim/simulatorinfo.h"
#include "blackmisc/logmessage.h"
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
        CSimulatorFs9Factory::CSimulatorFs9Factory(QObject *parent) :
            QObject(parent),
            m_fs9Host(new CFs9Host(this), [](CFs9Host* host){
                host->quit();
                host->deleteLater();
            }),
            m_lobbyClient(new CLobbyClient(this))
        {
            registerMetadata();
        }

        BlackCore::ISimulator *CSimulatorFs9Factory::create(
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *remoteAircraftProvider,
            QObject *parent)
        {
            return new Fs9::CSimulatorFs9(ownAircraftProvider, remoteAircraftProvider, parent);
        }
        
        BlackCore::ISimulatorListener *CSimulatorFs9Factory::createListener(QObject *parent)
        {
            return new CSimulatorFs9Listener(m_fs9Host, m_lobbyClient, parent);
        }

        BlackSim::CSimulatorInfo CSimulatorFs9Factory::getSimulatorInfo() const
        {
            return CSimulatorInfo::FS9();
        }

        CSimulatorFs9::CSimulatorFs9(IOwnAircraftProvider *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     const QSharedPointer<CFs9Host> &fs9Host,
                                     const QSharedPointer<CLobbyClient> &lobbyClient, QObject *parent) :
                CSimulatorFsCommon(CSimulatorInfo::FS9(), ownAircraftProvider, remoteAircraftProvider, parent),
                m_fs9Host(new CFs9Host(this)),
                m_lobbyClient(new CLobbyClient(this))
        {
            connect(m_lobbyClient.data(), &CLobbyClient::disconnected, this, std::bind(&CSimulatorFs9::simulatorStatusChanged, this, 0));
            connect(m_fs9Host.data(), &CFs9Host::customPacketReceived, this, &CSimulatorFs9::ps_processFs9Message);
            this->m_interpolator = new BlackCore::CInterpolatorLinear(remoteAircraftProvider, this);
            this->m_interpolator->start();
        }

        bool CSimulatorFs9::isConnected() const
        {
            return m_fs9Host->isConnected();
        }

        bool CSimulatorFs9::connectTo()
        {
            Q_ASSERT(m_fs9Host->isConnected());
            m_fsuipc->connect(); // connect FSUIPC too
            startTimer(50);
            emitSimulatorCombinedStatus();

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
            m_fsuipc->disconnect();
            return true;
        }

        bool CSimulatorFs9::addRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            CCallsign callsign = newRemoteAircraft.getCallsign();
            if (m_hashFs9Clients.contains(callsign))
            {
                // already exists, remove first
                this->removeRemoteAircraft(callsign);
            }

            CFs9Client *client = new CFs9Client(m_interpolator, this, callsign.toQString(), CTime(25, CTimeUnit::ms()));
            client->setHostAddress(m_fs9Host->getHostAddress());
            client->setPlayerUserId(m_fs9Host->getPlayerUserId());

            client->start();
            m_hashFs9Clients.insert(callsign, client);
            remoteAircraft().applyIfCallsign(callsign, CPropertyIndexVariantMap(CSimulatedAircraft::IndexRendered, CVariant::fromValue(true)));
            CLogMessage(this).info("FS9: Added aircraft %1") << callsign.toQString();
            return true;
        }

        bool CSimulatorFs9::removeRemoteAircraft(const CCallsign &callsign)
        {
            if (!m_hashFs9Clients.contains(callsign)) { return false; }

            auto fs9Client = m_hashFs9Clients.value(callsign);
            fs9Client->quit();
            m_hashFs9Clients.remove(callsign);
            remoteAircraft().setRendered(callsign, false);
            CLogMessage(this).info("FS9: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        void CSimulatorFs9::removeAllRemoteAircraft()
        {
            QList<CCallsign> callsigns(this->m_hashFs9Clients.keys());
            for (const CCallsign &cs : callsigns)
            {
                removeRemoteAircraft(cs);
            }
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
            /* Avoid errors from CDirectPlayPeer as it may end in infinite loop */
            if (message.getSeverity() == BlackMisc::CStatusMessage::SeverityError && message.isFromClass<CDirectPlayPeer>())
                return;

            if (message.getSeverity() != BlackMisc::CStatusMessage::SeverityDebug)
            {
                QMetaObject::invokeMethod(m_fs9Host.data(), "sendTextMessage", Q_ARG(QString, message.toQString()));
            }
        }

        void CSimulatorFs9::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        bool CSimulatorFs9::isRenderedAircraft(const CCallsign &callsign) const
        {
            return m_hashFs9Clients.contains(callsign);
        }

        void CSimulatorFs9::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
            ps_dispatch();
        }

        void CSimulatorFs9::ps_dispatch()
        {
            if (m_useFsuipc && m_fsuipc)
            {
                CSimulatedAircraft fsuipcAircraft(ownAircraft());
                bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
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

        void CSimulatorFs9::updateOwnAircraftFromSim(const CAircraft &ownAircraft)
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
                removeRemoteAircraft(fs9Client);
            }
        }

        CSimulatorFs9Listener::CSimulatorFs9Listener(const QSharedPointer<CFs9Host> &fs9Host,
                                                     const QSharedPointer<CLobbyClient> &lobbyClient,
                                                     QObject *parent) :
            BlackCore::ISimulatorListener(parent),
            m_timer(new QTimer(this)),
            m_fs9Host(fs9Host),
            m_lobbyClient(lobbyClient)
        {
            Q_CONSTEXPR int QueryInterval = 5 * 1000; // 5 seconds
            m_timer->setInterval(QueryInterval);

            connect(m_timer, &QTimer::timeout, [this]()
            {
                if (m_fs9Host->getHostAddress().isEmpty()) // host not yet set up
                    return;

                if (m_lobbyConnected || m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress()) == S_OK) {
                    m_lobbyConnected = true;
                     CLogMessage(this).info("Swift is joining FS9 to the multiplayer session...");
                }

                if (m_lobbyConnected && m_fs9Host->isConnected()) {
                    emit simulatorStarted(m_simulatorInfo);
                    m_lobbyConnected = false;
                }
            });

            m_fs9Host->start();

            // After FS9 is disconnected, reset its data stored in the host
            connect(m_lobbyClient.data(), &CLobbyClient::disconnected, m_fs9Host.data(), &CFs9Host::reset);
        }

        void CSimulatorFs9Listener::start()
        {
            m_timer->start();
        }

        void CSimulatorFs9Listener::stop()
        {
            m_timer->stop();
        }
    }
}

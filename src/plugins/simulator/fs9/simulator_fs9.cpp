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
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/project.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/propertyindexallclasses.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include <QTimer>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackSimPlugin::Fs9;
using namespace BlackSimPlugin::FsCommon;


namespace
{
    /* These instances should be global, as they are shared between all classes
     * this file contains. They are instantied by CFs9Factory. */
    QSharedPointer<CFs9Host> fs9Host;
    QSharedPointer<CLobbyClient> lobbyClient;
}

namespace BlackSimPlugin
{
    namespace Fs9
    {
        CSimulatorFs9::CSimulatorFs9(
            const CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *remoteAircraftProvider,
            IPluginStorageProvider *pluginStorageProvider,
            QObject *parent) :
            CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, pluginStorageProvider,
                               aircraftObjectsDir(), excludeDirectories(), parent)
        {
            connect(lobbyClient.data(), &CLobbyClient::disconnected, this, std::bind(&CSimulatorFs9::simulatorStatusChanged, this, 0));
            this->m_interpolator = new BlackCore::CInterpolatorLinear(remoteAircraftProvider, this);
            m_modelMatcher.setDefaultModel(CAircraftModel(
                                               "Boeing 737-400",
                                               CAircraftModel::TypeModelMatchingDefaultModel,
                                               "B737-400 default model",
                                               CAircraftIcaoData(CAircraftIcaoCode("B734", "L2J"), CAirlineIcaoCode(), "FFFFFF")
                                           ));
        }

        bool CSimulatorFs9::isConnected() const
        {
            return m_simConnected;
        }

        bool CSimulatorFs9::connectTo()
        {
            if (!fs9Host->isConnected()) { return false; } // host not available, we quit

            Q_ASSERT_X(m_fsuipc,  Q_FUNC_INFO, "No FSUIPC");
            m_connectionHostMessages = connect(fs9Host.data(), &CFs9Host::customPacketReceived, this, &CSimulatorFs9::ps_processFs9Message);

            if (m_useFsuipc)
            {
                m_fsuipc->connect(); // connect FSUIPC too
            }
            m_dispatchTimerId = startTimer(50);
            return true;
        }

        bool CSimulatorFs9::disconnectFrom()
        {
            if (!m_simConnected) { return true; }

            // Don't forward messages when disconnected
            disconnect(m_connectionHostMessages);
            killTimer(m_dispatchTimerId);
            m_dispatchTimerId = -1;
            disconnectAllClients();

            //  disconnect FSUIPC and status
            CSimulatorFsCommon::disconnectFrom();
            m_simConnected = false;
            emitSimulatorCombinedStatus();
            return true;
        }

        bool CSimulatorFs9::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            CCallsign callsign = newRemoteAircraft.getCallsign();
            if (m_hashFs9Clients.contains(callsign))
            {
                // already exists, remove first
                this->physicallyRemoveRemoteAircraft(callsign);
            }

            CSimulatedAircraft newRemoteAircraftCopy(newRemoteAircraft);
            // matched models
            CAircraftModel aircraftModel = getClosestMatch(newRemoteAircraftCopy);
            Q_ASSERT(newRemoteAircraft.getCallsign() == aircraftModel.getCallsign());
            updateAircraftModel(newRemoteAircraft.getCallsign(), aircraftModel, originator());
            updateAircraftRendered(newRemoteAircraft.getCallsign(), true, originator());
            CSimulatedAircraft aircraftAfterModelApplied(getAircraftInRangeForCallsign(newRemoteAircraft.getCallsign()));
            aircraftAfterModelApplied.setRendered(true);
            emit modelMatchingCompleted(aircraftAfterModelApplied);

            CFs9Client *client = new CFs9Client(callsign, aircraftModel.getModelString(), m_interpolator, CTime(25, CTimeUnit::ms()), this);
            client->setHostAddress(fs9Host->getHostAddress());
            client->setPlayerUserId(fs9Host->getPlayerUserId());

            client->start();
            m_hashFs9Clients.insert(callsign, client);
            updateAircraftRendered(callsign, true, this->originator());
            CLogMessage(this).info("FS9: Added aircraft %1") << callsign.toQString();
            return true;
        }

        bool CSimulatorFs9::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (!m_hashFs9Clients.contains(callsign)) { return false; }

            auto fs9Client = m_hashFs9Clients.value(callsign);
            fs9Client->quit();
            m_hashFs9Clients.remove(callsign);
            updateAircraftRendered(callsign, false, this->originator());
            CLogMessage(this).info("FS9: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        int CSimulatorFs9::physicallyRemoveAllRemoteAircraft()
        {
            if (this->m_hashFs9Clients.isEmpty()) { return 0; }
            QList<CCallsign> callsigns(this->m_hashFs9Clients.keys());
            int r = 0;
            for (const CCallsign &cs : callsigns)
            {
                if (physicallyRemoveRemoteAircraft(cs)) { r++; }
            }
            return r;

        }

        CCallsignSet CSimulatorFs9::physicallyRenderedAircraft() const
        {
            return CCollection<CCallsign>(this->m_hashFs9Clients.keys());
        }

        bool CSimulatorFs9::updateOwnSimulatorCockpit(const CAircraft &ownAircraft, const COriginator &originator)
        {
            if (originator == this->originator()) { return false; }
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
                QMetaObject::invokeMethod(fs9Host.data(), "sendTextMessage", Q_ARG(QString, message.toQString()));
            }
        }

        void CSimulatorFs9::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        bool CSimulatorFs9::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
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
                CSimulatedAircraft fsuipcAircraft(getOwnAircraft());
                bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
                if (ok)
                {
                    updateOwnAircraftFromSimulator(fsuipcAircraft);
                }
            }
        }

        void CSimulatorFs9::ps_processFs9Message(const QByteArray &message)
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
                    updateOwnSituation(aircraftSituationfromFS9(mpPositionVelocity));
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

        void CSimulatorFs9::updateOwnAircraftFromSimulator(const CAircraft &simDataOwnAircraft)
        {
            this->updateCockpit(
                simDataOwnAircraft.getCom1System(),
                simDataOwnAircraft.getCom2System(),
                simDataOwnAircraft.getTransponder(),
                this->originator());
        }

        void CSimulatorFs9::disconnectAllClients()
        {
            // Stop all FS9 client tasks
            for (auto fs9Client : m_hashFs9Clients.keys())
            {
                physicallyRemoveRemoteAircraft(fs9Client);
            }
        }

        QString CSimulatorFs9::aircraftObjectsDir()
        {
            QString dir = CFsCommonUtil::fs9AircraftDirFromRegistry();
            if (!dir.isEmpty()) { return dir; }
            return "C:/Flight Simulator 9/Aircraft";
        }

        const QStringList &CSimulatorFs9::excludeDirectories()
        {
            static const QStringList exclude
            {
            };
            return exclude;
        }

        CSimulatorFs9Listener::CSimulatorFs9Listener(const CSimulatorPluginInfo &info, QObject *parent) :
            BlackCore::ISimulatorListener(info, parent),
            m_timer(new QTimer(this))
        {
            Q_CONSTEXPR int QueryInterval = 5 * 1000; // 5 seconds
            m_timer->setInterval(QueryInterval);

            // Test whether we can lobby connect at all.
            bool canLobbyConnect = lobbyClient->canLobbyConnect();

            connect(m_timer, &QTimer::timeout, [this, canLobbyConnect]()
            {
                if (fs9Host->getHostAddress().isEmpty()) // host not yet set up
                    return;

                if (canLobbyConnect)
                {
                    if (m_isConnecting || lobbyClient->connectFs9ToHost(fs9Host->getHostAddress()) == S_OK)
                    {
                        m_isConnecting = true;
                        CLogMessage(this).info("Swift is joining FS9 to the multiplayer session...");
                    }
                }

                if (!m_isStarted && fs9Host->isConnected())
                {
                    emit simulatorStarted(getPluginInfo());
                    m_isStarted = true;
                    m_isConnecting = false;
                }
            });
        }

        void CSimulatorFs9Listener::start()
        {
            m_isStarted = false;
            m_timer->start();
        }

        void CSimulatorFs9Listener::stop()
        {
            m_timer->stop();
        }

        CSimulatorFs9Factory::CSimulatorFs9Factory(QObject *parent) :
            QObject(parent)
        {
            /* Nobody should have created the host before */
            Q_ASSERT(!fs9Host);

            registerMetadata();

            fs9Host.reset(new CFs9Host(this));
            lobbyClient.reset(new CLobbyClient(this));

            /* After FS9 is disconnected, reset its data stored in the host */
            connect(lobbyClient.data(), &CLobbyClient::disconnected, fs9Host.data(), &CFs9Host::reset);

            fs9Host->start();
        }

        CSimulatorFs9Factory::~CSimulatorFs9Factory()
        {
            fs9Host->quit();
        }

        BlackCore::ISimulator *CSimulatorFs9Factory::create(
            const CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *remoteAircraftProvider,
            IPluginStorageProvider *pluginStorageProvider)
        {
            return new CSimulatorFs9(info, ownAircraftProvider, remoteAircraftProvider, pluginStorageProvider, this);
        }

        BlackCore::ISimulatorListener *CSimulatorFs9Factory::createListener(const CSimulatorPluginInfo &info, QObject *parent)
        {
            return new CSimulatorFs9Listener(info, parent);
        }

    } // namespace
} // namespace

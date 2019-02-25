/* Copyright (C) 2014
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "fs9.h"
#include "directplayerror.h"
#include "simulatorfs9.h"
#include "fs9client.h"
#include "multiplayerpackets.h"
#include "multiplayerpacketparser.h"
#include "registermetadata.h"
#include "../fscommon/simulatorfscommonfunctions.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/simulation/fscommon/fscommonutil.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/propertyindexallclasses.h"
#include <QTimer>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::FsCommon;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackSimPlugin::FsCommon;

namespace BlackSimPlugin
{
    namespace Fs9
    {
        CAircraftSituation aircraftSituationFromFS9(const MPPositionVelocity &positionVelocity)
        {
            CAircraftSituation situation;

            double dHigh = positionVelocity.lat_i;
            double dLow = positionVelocity.lat_f;

            dLow = dLow / 65536.0;
            if (dHigh > 0)
            {
                dHigh = dHigh + dLow;
            }
            else
            {
                dHigh = dHigh - dLow;
            }

            CCoordinateGeodetic position;
            position.setLatitude(CLatitude(dHigh * 90.0 / 10001750.0, CAngleUnit::deg()));

            dHigh = positionVelocity.lon_hi;
            dLow = positionVelocity.lon_lo;

            dLow = dLow / 65536.0;
            if (dHigh > 0)
            {
                dHigh = dHigh + dLow;
            }
            else
            {
                dHigh = dHigh - dLow;
            }

            position.setLongitude(CLongitude(dHigh * 360.0 / (65536.0 * 65536.0), CAngleUnit::deg()));

            dHigh = positionVelocity.alt_i;
            dLow = positionVelocity.alt_f;

            dLow = dLow / 65536.0;

            situation.setPosition(position);
            situation.setAltitude(CAltitude(dHigh + dLow, CAltitude::MeanSeaLevel, CLengthUnit::m()));
            const double groundSpeed = positionVelocity.ground_velocity / 65536.0;
            situation.setGroundSpeed(CSpeed(groundSpeed, CSpeedUnit::m_s()));

            FS_PBH pbhstrct;
            pbhstrct.pbh = positionVelocity.pbh;
            int pitch = qRound(std::floor(pbhstrct.pitch / CFs9Sdk::pitchMultiplier()));
            if (pitch < -90 || pitch > 89) { CLogMessage().warning(u"FS9: Pitch value out of limits: %1") << pitch; }
            int bank = qRound(std::floor(pbhstrct.bank / CFs9Sdk::bankMultiplier()));

            // MSFS has inverted pitch and bank angles
            pitch = ~pitch;
            bank = ~bank;
            situation.setPitch(CAngle(pitch, CAngleUnit::deg()));
            situation.setBank(CAngle(bank, CAngleUnit::deg()));
            situation.setHeading(CHeading(pbhstrct.hdg / CFs9Sdk::headingMultiplier(), CHeading::Magnetic, CAngleUnit::deg()));

            return situation;
        }

        CSimulatorFs9::CSimulatorFs9(const CSimulatorPluginInfo &info,
                                     const QSharedPointer<CFs9Host>     &fs9Host,
                                     const QSharedPointer<CLobbyClient> &lobbyClient,
                                     IOwnAircraftProvider    *ownAircraftProvider,
                                     IRemoteAircraftProvider *remoteAircraftProvider,
                                     IWeatherGridProvider    *weatherGridProvider,
                                     IClientProvider         *clientProvider,
                                     QObject *parent) :
            CSimulatorFsCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent),
            m_fs9Host(fs9Host),
            m_lobbyClient(lobbyClient)
        {
            // disabled CG/elevation parts
            this->setSimulationProviderEnabled(false, false);

            //! \fixme KB 7/2017 change or remove comment when reviewed Could we just use: connect(lobbyClient.data(), &CLobbyClient::disconnected, this, &CSimulatorFs9::disconnectFrom);
            connect(lobbyClient.data(), &CLobbyClient::disconnected, this, [ = ]
            {
                this->emitSimulatorCombinedStatus();
            });

            this->setDefaultModel(
            {
                "Boeing 737-400", CAircraftModel::TypeModelMatchingDefaultModel,
                "B737-400 default model", CAircraftIcaoCode("B734", "L2J")
            });
        }

        bool CSimulatorFs9::isConnected() const
        {
            return m_simConnected;
        }

        bool CSimulatorFs9::connectTo()
        {
            Q_ASSERT_X(m_fs9Host, Q_FUNC_INFO, "No FS9 host");
            if (!m_fs9Host->isConnected()) { return false; } // host not available, we quit

            Q_ASSERT_X(m_fsuipc,  Q_FUNC_INFO, "No FSUIPC");
            m_connectionHostMessages = connect(m_fs9Host.data(), &CFs9Host::customPacketReceived, this, &CSimulatorFs9::processFs9Message);

            useFsuipc(true);
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

            //  disconnect FSUIPC and status
            CSimulatorFsCommon::disconnectFrom();
            m_simConnected = false;
            emitSimulatorCombinedStatus();
            return true;
        }

        bool CSimulatorFs9::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            const CCallsign callsign = newRemoteAircraft.getCallsign();
            if (m_hashFs9Clients.contains(callsign))
            {
                // already exists, remove first
                this->physicallyRemoveRemoteAircraft(callsign);
            }

            bool rendered = true;
            updateAircraftRendered(callsign, rendered);
            CFs9Client *client = new CFs9Client(callsign, newRemoteAircraft.getModelString(), CTime(25, CTimeUnit::ms()), &m_interpolationLogger, this);
            client->setHostAddress(m_fs9Host->getHostAddress());
            client->setPlayerUserId(m_fs9Host->getPlayerUserId());
            client->start();

            m_hashFs9Clients.insert(callsign, client);
            bool updated = updateAircraftRendered(callsign, rendered);
            CSimulatedAircraft remoteAircraftCopy(newRemoteAircraft);
            remoteAircraftCopy.setRendered(rendered);
            if (updated)
            {
                emit aircraftRenderingChanged(remoteAircraftCopy);
            }
            CLogMessage(this).info(u"FS9: Added aircraft %1") << callsign.toQString();
            return true;
        }

        bool CSimulatorFs9::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (!m_hashFs9Clients.contains(callsign)) { return false; }

            auto fs9Client = m_hashFs9Clients.value(callsign);
            delete fs9Client;
            m_hashFs9Clients.remove(callsign);
            updateAircraftRendered(callsign, false);
            CLogMessage(this).info(u"FS9: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        int CSimulatorFs9::physicallyRemoveAllRemoteAircraft()
        {
            resetHighlighting();
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
            const CComSystem newCom1 = ownAircraft.getCom1System();
            const CComSystem newCom2 = ownAircraft.getCom2System();
            const CTransponder newTransponder = ownAircraft.getTransponder();

            bool changed = false;
            if (newCom1.getFrequencyActive() != m_simCom1.getFrequencyActive()) { changed = true; }
            if (newCom1.getFrequencyStandby() != m_simCom1.getFrequencyStandby()) { changed = true; }
            if (newCom2.getFrequencyActive() != m_simCom2.getFrequencyActive()) { changed = true; }
            if (newCom2.getFrequencyStandby() != m_simCom2.getFrequencyStandby()) { changed = true; }
            if (newTransponder.getTransponderCode() != m_simTransponder.getTransponderCode()) { changed = true; }
            if (newTransponder.getTransponderMode() != m_simTransponder.getTransponderMode()) { changed = true; }

            //! \todo KB 8/2017 set FS9 cockpit values

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
                //! KB 2018-11 als we would need to send the value to FS9/FSX (currently we only deal with it on FS9/FSX level)
                m_selcal = selcal;
                changed = true;
            }

            return changed;
        }

        void CSimulatorFs9::displayStatusMessage(const CStatusMessage &message) const
        {
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
            this->displayStatusMessage(message.asStatusMessage(true, true));
        }

        CStatusMessageList CSimulatorFs9::getInterpolationMessages(const CCallsign &callsign) const
        {
            if (!m_hashFs9Clients.contains(callsign)) { return CStatusMessageList(); }
            const CFs9Client *client = m_hashFs9Clients[callsign].data();
            if (!client) { return CStatusMessageList(); }
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
            return client->getInterpolationMessages(setup.getInterpolatorMode());
        }

        bool CSimulatorFs9::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            return m_hashFs9Clients.contains(callsign);
        }

        void CSimulatorFs9::timerEvent(QTimerEvent *event)
        {
            Q_UNUSED(event);
            dispatch();
        }

        void CSimulatorFs9::dispatch()
        {
            if (m_useFsuipc && m_fsuipc && m_fsuipc->isOpened())
            {
                CSimulatedAircraft fsuipcAircraft(getOwnAircraft());
                const bool ok = m_fsuipc->read(fsuipcAircraft, true, true, true);
                if (ok)
                {
                    updateOwnAircraftFromSimulator(fsuipcAircraft);
                }
            }
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
                    MPPositionVelocity mpPositionVelocity;
                    MultiPlayerPacketParser::readMessage(message, mpPositionVelocity);
                    auto aircraftSituation = aircraftSituationFromFS9(mpPositionVelocity);
                    updateOwnSituation(aircraftSituation);

                    if (m_isWeatherActivated)
                    {
                        const auto currentPosition = CCoordinateGeodetic { aircraftSituation.latitude(), aircraftSituation.longitude() };
                        if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()))
                        {
                            if (m_lastWeatherPosition.isNull() ||
                                    calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
                            {
                                m_lastWeatherPosition = currentPosition;
                                const auto weatherGrid = CWeatherGrid { { "GLOB", currentPosition } };
                                requestWeatherGrid(weatherGrid, { this, &CSimulatorFs9::injectWeatherGrid });
                            }
                        }
                    }
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
            this->updateCockpit(
                simDataOwnAircraft.getCom1System(),
                simDataOwnAircraft.getCom2System(),
                simDataOwnAircraft.getTransponder(),
                this->identifier());
            this->updateOwnSituation(simDataOwnAircraft.getSituation());
            reverseLookupAndUpdateOwnAircraftModel(simDataOwnAircraft.getModelString());
        }

        void CSimulatorFs9::disconnectAllClients()
        {
            // Stop all FS9 client tasks
            const QList<CCallsign> callsigns(m_hashFs9Clients.keys());
            for (auto fs9Client : callsigns)
            {
                physicallyRemoveRemoteAircraft(fs9Client);
            }
        }

        void CSimulatorFs9::injectWeatherGrid(const Weather::CWeatherGrid &weatherGrid)
        {
            if (!m_useFsuipc || !m_fsuipc) { return; }
            if (!m_fsuipc->isOpened()) { return; }
            m_fsuipc->write(weatherGrid);
        }

        CSimulatorFs9Listener::CSimulatorFs9Listener(const CSimulatorPluginInfo &info,
                const QSharedPointer<CFs9Host> &fs9Host,
                const QSharedPointer<CLobbyClient> &lobbyClient) :
            ISimulatorListener(info),
            m_timer(new QTimer(this)),
            m_fs9Host(fs9Host),
            m_lobbyClient(lobbyClient)
        {
            const int QueryInterval = 5 * 1000; // 5 seconds
            m_timer->setInterval(QueryInterval);
            m_timer->setObjectName(this->objectName() + ":m_timer");

            // Test whether we can lobby connect at all.
            const bool canLobbyConnect = m_lobbyClient->canLobbyConnect();

            // check connection
            connect(m_timer, &QTimer::timeout, [ = ]()
            {
                this->checkConnection(canLobbyConnect);
            });
        }

        void CSimulatorFs9Listener::startImpl()
        {
            m_isStarted = false;
            m_timer->start();
        }

        void CSimulatorFs9Listener::stopImpl()
        {
            m_timer->stop();
        }

        void CSimulatorFs9Listener::checkImpl()
        {
            if (m_timer) { m_timer->start(); }
            if (this->isShuttingDown()) { return; }

            QPointer<CSimulatorFs9Listener> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                const bool canLobbyConnect = m_lobbyClient->canLobbyConnect();
                this->checkConnection(canLobbyConnect);
            });
        }

        bool CSimulatorFs9Listener::checkConnection(bool canLobbyConnect)
        {
            if (m_fs9Host->getHostAddress().isEmpty()) { return false; } // host not yet set up
            if (canLobbyConnect)
            {
                if (m_isConnecting || isOk(m_lobbyClient->connectFs9ToHost(m_fs9Host->getHostAddress())))
                {
                    m_isConnecting = true;
                    CLogMessage(this).info(u"swift is joining FS9 to the multiplayer session...");
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

        static void cleanupFs9Host(CFs9Host *host)
        {
            delete host;
        }

        CSimulatorFs9Factory::CSimulatorFs9Factory(QObject *parent) :
            QObject(parent),
            m_fs9Host(new CFs9Host(this), cleanupFs9Host),
            m_lobbyClient(new CLobbyClient(this))
        {
            registerMetadata();

            /* After FS9 is disconnected, reset its data stored in the host */
            connect(m_lobbyClient.data(), &CLobbyClient::disconnected, m_fs9Host.data(), &CFs9Host::reset);
        }

        CSimulatorFs9Factory::~CSimulatorFs9Factory()
        { }

        BlackCore::ISimulator *CSimulatorFs9Factory::create(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider    *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider    *weatherGridProvider,
                IClientProvider         *clientProvider)
        {
            return new CSimulatorFs9(info, m_fs9Host, m_lobbyClient, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
        }

        BlackCore::ISimulatorListener *CSimulatorFs9Factory::createListener(const CSimulatorPluginInfo &info)
        {
            return new CSimulatorFs9Listener(info, m_fs9Host, m_lobbyClient);
        }
    } // namespace
} // namespace

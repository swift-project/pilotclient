/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulator_xplane.h"
#include "xbus_service_proxy.h"
#include "xbus_traffic_proxy.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include <QDBusServiceWatcher>
#include <QTimer>
#include <QString>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;

namespace
{
    inline QString xbusServiceName()
    {
        return QStringLiteral("org.swift-project.xbus");
    }
}

namespace BlackSimPlugin
{
    namespace XPlane
    {

        CSimulatorXPlane::CSimulatorXPlane(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *remoteAircraftProvider,
            IPluginStorageProvider *pluginStorageProvider,
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, remoteAircraftProvider, pluginStorageProvider, parent)
        {
            m_watcher = new QDBusServiceWatcher(this);
            m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
            m_watcher->addWatchedService(xbusServiceName());
            m_watcher->setObjectName("QDBusServiceWatcher");
            connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, &CSimulatorXPlane::ps_serviceUnregistered);

            m_fastTimer = new QTimer(this);
            m_slowTimer = new QTimer(this);
            m_fastTimer->setObjectName(this->objectName().append(":m_fastTimer"));
            m_slowTimer->setObjectName(this->objectName().append(":m_slowTimer"));
            connect(m_fastTimer, &QTimer::timeout, this, &CSimulatorXPlane::ps_fastTimerTimeout);
            connect(m_slowTimer, &QTimer::timeout, this, &CSimulatorXPlane::ps_slowTimerTimeout);
            m_fastTimer->start(100);
            m_slowTimer->start(1000);
            resetData();
        }

        // convert xplane squawk mode to swift squawk mode
        BlackMisc::Aviation::CTransponder::TransponderMode xpdrMode(int xplaneMode, bool ident)
        {
            if (ident) { return BlackMisc::Aviation::CTransponder::StateIdent; }
            if (xplaneMode == 0 || xplaneMode == 1) { return BlackMisc::Aviation::CTransponder::StateStandby; }
            return BlackMisc::Aviation::CTransponder::ModeC;
        }

        // convert swift squawk mode to xplane squawk mode
        int xpdrMode(BlackMisc::Aviation::CTransponder::TransponderMode mode)
        {
            return mode == BlackMisc::Aviation::CTransponder::StateStandby ? 1 : 2;
        }

        void CSimulatorXPlane::ps_fastTimerTimeout()
        {
            if (isConnected())
            {
                m_service->getLatitudeAsync(&m_xplaneData.latitude);
                m_service->getLongitudeAsync(&m_xplaneData.longitude);
                m_service->getAltitudeMSLAsync(&m_xplaneData.altitude);
                m_service->getGroundSpeedAsync(&m_xplaneData.groundspeed);
                m_service->getPitchAsync(&m_xplaneData.pitch);
                m_service->getRollAsync(&m_xplaneData.roll);
                m_service->getTrueHeadingAsync(&m_xplaneData.trueHeading);
                m_service->getCom1ActiveAsync(&m_xplaneData.com1Active);
                m_service->getCom1StandbyAsync(&m_xplaneData.com1Standby);
                m_service->getCom2ActiveAsync(&m_xplaneData.com2Active);
                m_service->getCom2StandbyAsync(&m_xplaneData.com2Standby);
                m_service->getTransponderCodeAsync(&m_xplaneData.xpdrCode);
                m_service->getTransponderModeAsync(&m_xplaneData.xpdrMode);
                m_service->getTransponderIdentAsync(&m_xplaneData.xpdrIdent);
                m_service->getAllWheelsOnGroundAsync(&m_xplaneData.onGroundAll);

                Aviation::CAircraftSituation situation;
                situation.setPosition({ m_xplaneData.latitude, m_xplaneData.longitude, 0 });
                situation.setAltitude({ m_xplaneData.altitude, Aviation::CAltitude::MeanSeaLevel, CLengthUnit::m() });
                situation.setHeading({ m_xplaneData.trueHeading, Aviation::CHeading::True, CAngleUnit::deg() });
                situation.setPitch({ m_xplaneData.pitch, CAngleUnit::deg() });
                situation.setBank({ m_xplaneData.roll, CAngleUnit::deg() });
                situation.setGroundspeed({ m_xplaneData.groundspeed, CSpeedUnit::m_s() });

                // updates
                updateOwnIcaoData(Aviation::CAircraftIcaoData { m_xplaneData.aircraftIcaoCode });
                updateOwnSituation(situation);
                updateCockpit(
                    Aviation::CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() }),
                    Aviation::CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() }),
                    Aviation::CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent)),
                    simulatorOriginator()
                );
            }
        }

        void CSimulatorXPlane::ps_slowTimerTimeout()
        {
            if (isConnected())
            {
                m_service->getAircraftModelPathAsync(&m_xplaneData.aircraftModelPath);
                m_service->getAircraftIcaoCodeAsync(&m_xplaneData.aircraftIcaoCode);
                m_service->getBeaconLightsOnAsync(&m_xplaneData.beaconLightsOn);
                m_service->getLandingLightsOnAsync(&m_xplaneData.landingLightsOn);
                m_service->getNavLightsOnAsync(&m_xplaneData.navLightsOn);
                m_service->getStrobeLightsOnAsync(&m_xplaneData.strobeLightsOn);
                m_service->getTaxiLightsOnAsync(&m_xplaneData.taxiLightsOn);
                m_service->getFlapsDeployRatioAsync(&m_xplaneData.flapsReployRatio);
                m_service->getGearDeployRatioAsync(&m_xplaneData.gearReployRatio);
                m_service->getEngineN1PercentageAsync(&m_xplaneData.enginesN1Percentage);
                m_service->getSpeedBrakeRatioAsync(&m_xplaneData.speedBrakeRatio);

                CAircraftEngineList engines;
                for (int engineNumber = 0; engineNumber < m_xplaneData.enginesN1Percentage.size(); ++engineNumber)
                {
                    // Engine number start counting at 1
                    // We consider the engine running when N1 is bigger than 5 %
                    CAircraftEngine engine {engineNumber + 1, m_xplaneData.enginesN1Percentage.at(engineNumber) > 5.0};
                    engines.push_back(engine);
                }

                Aviation::CAircraftParts parts { {
                        m_xplaneData.strobeLightsOn, m_xplaneData.landingLightsOn, m_xplaneData.taxiLightsOn,
                        m_xplaneData.beaconLightsOn, m_xplaneData.navLightsOn, false
                    },
                    { m_xplaneData.gearReployRatio > 0 }, { static_cast<int>(m_xplaneData.flapsReployRatio * 100) },
                    { m_xplaneData.speedBrakeRatio > 0.5 }, engines, { m_xplaneData.onGroundAll }
                };
                updateOwnParts(parts);
            }
        }

        bool CSimulatorXPlane::isConnected() const
        {
            return m_service && m_traffic;
        }

        bool CSimulatorXPlane::connectTo()
        {
            if (isConnected())
            {
                qWarning("X-Plane already connected");
                return true;
            }

            m_conn = QDBusConnection::sessionBus(); // TODO make this configurable
            m_service = new CXBusServiceProxy(m_conn, this);
            m_traffic = new CXBusTrafficProxy(m_conn, this);

            if (m_service->isValid() && m_traffic->isValid() && m_traffic->initialize())
            {
                // FIXME duplication
                connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::ps_emitOwnAircraftModelChanged);
                connect(m_service, &CXBusServiceProxy::airportsInRangeUpdated, this, &CSimulatorXPlane::ps_setAirportsInRange);
                m_service->updateAirportsInRange();
                m_watcher->setConnection(m_conn);
                emitSimulatorCombinedStatus();
                return true;
            }
            else
            {
                disconnectFrom();
                return false;
            }
        }

        void CSimulatorXPlane::asyncConnectTo()
        {
            // TODO
            connectTo();
        }

        bool CSimulatorXPlane::disconnectFrom()
        {
            if (m_traffic)
            {
                m_traffic->cleanup();
            }

            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            delete m_traffic;
            m_service = nullptr;
            m_traffic = nullptr;
            emitSimulatorCombinedStatus();
            return true;
        }

        void CSimulatorXPlane::ps_serviceUnregistered()
        {
            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            delete m_traffic;
            m_service = nullptr;
            m_traffic = nullptr;
            emitSimulatorCombinedStatus();
        }

        void CSimulatorXPlane::ps_emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao)
        {
            //! \todo XP, change as appropriate
            // try to set correct model and ICAO values here
            // thy show up in GUI
            CAircraftModel model(getOwnAircraftModel());
            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
            model.setFileName(path + "/" + filename);
            CAircraftIcaoData aircraftIcao(icao);
            aircraftIcao.setLivery(livery);

            // updates
            updateOwnIcaoData(aircraftIcao);
            updateOwnModel(model);
            emit ownAircraftModelChanged(getOwnAircraft());
        }

        void CSimulatorXPlane::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            /* We do not assert here as status message may come because of network problems */
            if (!isConnected()) { return; }

            //! \todo XP driver, display text message: XPLMSpeakString()? http://www.xsquawkbox.net/xpsdk/mediawiki/XPLMSpeakString
            Q_UNUSED(message);
        }

        void CSimulatorXPlane::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            if (!isConnected()) { return; }

            //! \todo XP driver, display text message: XPLMSpeakString()? http://www.xsquawkbox.net/xpsdk/mediawiki/XPLMSpeakString
            Q_UNUSED(message);
        }

        BlackMisc::Simulation::CAircraftModelList CSimulatorXPlane::getInstalledModels() const
        {
            Q_ASSERT(isConnected());
            //! \todo XP driver, function not available
            CLogMessage(this).error("Function not avialable");
            return {};
        }

        void CSimulatorXPlane::reloadInstalledModels()
        {
            //! \todo XP driver, function not available
            CLogMessage(this).error("Function not avialable");
        }

        void CSimulatorXPlane::ps_setAirportsInRange(const QStringList &icaos, const QStringList &names, const BlackMisc::CSequence<double> &lats, const BlackMisc::CSequence<double> &lons, const BlackMisc::CSequence<double> &alts)
        {
            qDebug() << alts;
            m_airportsInRange.clear();
            auto icaoIt = icaos.begin();
            auto nameIt = names.begin();
            auto latIt = lats.begin();
            auto lonIt = lons.begin();
            auto altIt = alts.begin();
            for (; icaoIt != icaos.end() && nameIt != names.end() && latIt != lats.end() && lonIt != lons.end() && altIt != alts.end(); ++icaoIt, ++nameIt, ++latIt, ++lonIt, ++altIt)
            {
                using namespace BlackMisc::PhysicalQuantities;
                using namespace BlackMisc::Geo;

                m_airportsInRange.push_back({ *icaoIt, { CLatitude(*latIt, CAngleUnit::deg()), CLongitude(*lonIt, CAngleUnit::deg()), CLength(*altIt, CLengthUnit::ft()) }, *nameIt });
            }
            using namespace BlackMisc::Math;
        }

        BlackMisc::Aviation::CAirportList CSimulatorXPlane::getAirportsInRange() const
        {
            auto copy = m_airportsInRange;
            //! \todo XP driver: Check if units match, xPlaneData has now hints what the values are
            copy.sortByRange(CCoordinateGeodetic(m_xplaneData.latitude, m_xplaneData.longitude, 0), true);
            copy.truncate(20);
            return copy;
        }

        bool CSimulatorXPlane::setTimeSynchronization(bool enable, BlackMisc::PhysicalQuantities::CTime)
        {
            if (enable)
            {
                CLogMessage(this).info("X-Plane provides real time synchronization, use this on");
            }
            return false;
        }

        CPixmap CSimulatorXPlane::iconForModel(const QString &modelString) const
        {
            Q_UNUSED(modelString);
            return CPixmap();
        }

        bool CSimulatorXPlane::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            //! \todo XP implement isRenderedAircraft correctly. This work around, but not really telling me if callsign is really(!) visible in SIM
            return getAircraftInRangeForCallsign(callsign).isRendered();
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const COriginator &originator)
        {
            Q_ASSERT(isConnected());
            if (originator == this->simulatorOriginator()) { return false; }
            auto com1 = Aviation::CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() });
            auto com2 = Aviation::CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() });
            auto xpdr = Aviation::CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_xplaneData.com1Active = aircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com1Standby = aircraft.getCom1System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Active = aircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Standby = aircraft.getCom2System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.xpdrCode = aircraft.getTransponderCode();
                m_xplaneData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
                m_service->setCom1Active(m_xplaneData.com1Active);
                m_service->setCom1Standby(m_xplaneData.com1Standby);
                m_service->setCom2Active(m_xplaneData.com2Active);
                m_service->setCom2Standby(m_xplaneData.com2Standby);
                m_service->setTransponderCode(m_xplaneData.xpdrCode);
                m_service->setTransponderMode(m_xplaneData.xpdrMode);

                m_service->cancelAllPendingAsyncCalls(); // in case there is already a reply with some old data incoming
                return true;
            }
            return false;
        }

        bool CSimulatorXPlane::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            Q_ASSERT(isConnected());
            //! \todo XPlane driver check if already exists, how?
            //! \todo XPlane driver set correct return value
            // KB: from what I can see here all data are available
            // Is there any model matching required ????
            CAircraftIcaoData icao = newRemoteAircraft.getIcaoInfo();
            m_traffic->addPlane(newRemoteAircraft.getCallsign().asString(), icao.getAircraftDesignator(), icao.getAirlineDesignator(), icao.getLivery());
            updateAircraftRendered(newRemoteAircraft.getCallsign(), true, simulatorOriginator());
            CLogMessage(this).info("XP: Added aircraft %1") << newRemoteAircraft.getCallsign().toQString();
            return true;
        }

        void CSimulatorXPlane::ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
        {
            Q_ASSERT(isConnected());
            using namespace BlackMisc::PhysicalQuantities;
            m_traffic->setPlanePosition(situation.getCallsign().asString(),
                                        situation.latitude().value(CAngleUnit::deg()),
                                        situation.longitude().value(CAngleUnit::deg()),
                                        situation.getAltitude().value(CLengthUnit::ft()),
                                        situation.getPitch().value(CAngleUnit::deg()),
                                        situation.getBank().value(CAngleUnit::deg()),
                                        situation.getHeading().value(CAngleUnit::deg()));
        }

        void CSimulatorXPlane::ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts)
        {
            Q_ASSERT(isConnected());
            m_traffic->setPlaneSurfaces(parts.getCallsign().asString(), true, 0, 0, 0, 0, 0, 0, 0, 0, 0, true, true, true, true, 0); // TODO landing gear, lights, control surfaces
            m_traffic->setPlaneTransponder(parts.getCallsign().asString(), 2000, true, false); // TODO transponder
        }

        void CSimulatorXPlane::ps_remoteProviderRemovedAircraft(const CCallsign &callsign)
        {
            Q_UNUSED(callsign);
            //! \todo call removeRemoteAircraft or just let removeRemoteAircraft handle it?
        }

        bool CSimulatorXPlane::physicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign)
        {
            Q_ASSERT(isConnected());
            m_traffic->removePlane(callsign.asString());
            updateAircraftRendered(callsign, false, simulatorOriginator());
            CLogMessage(this).info("XP: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        int CSimulatorXPlane::physicallyRemoveAllRemoteAircraft()
        {
            //! \todo XP driver obtain number of removed aircraft
            int r = getAircraftInRangeCount();
            m_traffic->removeAllPlanes();
            updateMarkAllAsNotRendered(simulatorOriginator());
            CLogMessage(this).info("XP: Removed all aircraft");
            return r;
        }

        CCallsignSet CSimulatorXPlane::physicallyRenderedAircraft() const
        {
            //! \todo XP driver, return list of callsigns really present in the simulator
            return getAircraftInRange().findByRendered(true).getCallsigns(); // just a poor workaround
        }

        bool CSimulatorXPlane::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft, const COriginator &originator)
        {
            return this->changeRemoteAircraftEnabled(aircraft, originator);
        }

        CAircraftIcaoData CSimulatorXPlane::getIcaoForModelString(const QString &modelString) const
        {
            Q_UNUSED(modelString);
            return CAircraftIcaoData();
        }

        bool CSimulatorXPlane::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft, const COriginator &originator)
        {
            if (originator == simulatorOriginator()) { return false; }
            if (aircraft.isEnabled())
            {
                this->physicallyAddRemoteAircraft(aircraft);
            }
            else
            {
                this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
            }
            return true;
        }

        BlackCore::ISimulator *CSimulatorXPlaneFactory::create(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *renderedAircraftProvider,
            IPluginStorageProvider *pluginStorageProvider)
        {
            return new CSimulatorXPlane(info, ownAircraftProvider, renderedAircraftProvider, pluginStorageProvider, this);
        }

        CSimulatorXPlaneListener::CSimulatorXPlaneListener(QObject *parent): ISimulatorListener(parent)
        {

        }

        void CSimulatorXPlaneListener::start()
        {
            if (m_watcher) // already started
                return;

            m_conn = QDBusConnection::sessionBus(); // TODO make this configurable
            m_watcher = new QDBusServiceWatcher(xbusServiceName(), m_conn, QDBusServiceWatcher::WatchForRegistration, this);
            connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, &CSimulatorXPlaneListener::ps_serviceRegistered);
        }

        void CSimulatorXPlaneListener::stop()
        {
            if (m_watcher)
            {
                m_watcher->deleteLater();
                m_watcher = nullptr;
            }
        }

        void CSimulatorXPlaneListener::ps_serviceRegistered(const QString &serviceName)
        {
            if (serviceName == xbusServiceName())
                emit simulatorStarted();
        }

    } // namespace
} // namespace

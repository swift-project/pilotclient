/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorxplane.h"
#include "blackcore/aircraftmatcher.h"
#include "blackmisc/aviation/aircraftengine.h"
#include "blackmisc/aviation/aircraftenginelist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftparts.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/weather/cloudlayer.h"
#include "blackmisc/weather/cloudlayerlist.h"
#include "blackmisc/weather/gridpoint.h"
#include "blackmisc/weather/temperaturelayer.h"
#include "blackmisc/weather/temperaturelayerlist.h"
#include "blackmisc/weather/visibilitylayer.h"
#include "blackmisc/weather/visibilitylayerlist.h"
#include "blackmisc/weather/windlayer.h"
#include "blackmisc/weather/windlayerlist.h"
#include "qcompilerdetection.h"
#include "xswiftbusserviceproxy.h"
#include "xswiftbustrafficproxy.h"
#include "xswiftbusweatherproxy.h"

#include <QColor>
#include <QDBusServiceWatcher>
#include <QString>
#include <QTimer>
#include <QtGlobal>

using namespace BlackMisc;
using namespace Aviation;
using namespace Network;
using namespace PhysicalQuantities;
using namespace Simulation;
using namespace Geo;
using namespace Simulation;
using namespace Weather;

namespace
{
    inline const QString &xswiftbusServiceName()
    {
        static const QString name("org.swift-project.xswiftbus");
        return name;
    }
}

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CSimulatorXPlane::CSimulatorXPlane(const CSimulatorPluginInfo &info,
                                           IOwnAircraftProvider *ownAircraftProvider,
                                           IRemoteAircraftProvider *remoteAircraftProvider,
                                           IWeatherGridProvider *weatherGridProvider,
                                           IClientProvider *clientProvider,
                                           QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            m_watcher = new QDBusServiceWatcher(this);
            m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
            m_watcher->addWatchedService(xswiftbusServiceName());
            m_watcher->setObjectName("QDBusServiceWatcher");
            connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, &CSimulatorXPlane::serviceUnregistered);

            m_fastTimer.setObjectName(this->objectName().append(":m_fastTimer"));
            m_slowTimer.setObjectName(this->objectName().append(":m_slowTimer"));
            connect(&m_fastTimer, &QTimer::timeout, this, &CSimulatorXPlane::fastTimerTimeout);
            connect(&m_slowTimer, &QTimer::timeout, this, &CSimulatorXPlane::slowTimerTimeout);
            connect(&m_airportUpdater, &QTimer::timeout, this, &CSimulatorXPlane::updateAirportsInRange);
            m_fastTimer.start(100);
            m_slowTimer.start(1000);
            m_airportUpdater.start(60000);

            this->setDefaultModel({ "Jets A320_a A320_a_Austrian_Airlines A320_a_Austrian_Airlines", CAircraftModel::TypeModelMatchingDefaultModel,
                                    "A320 AUA", CAircraftIcaoCode("A320", "L2J")});
            this->resetXPlaneData();
        }

        void CSimulatorXPlane::unload()
        {
            CSimulatorCommon::unload();
            delete m_watcher;
            m_watcher = nullptr;
        }

        bool CSimulatorXPlane::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            //! \todo KB 2018-04 implement a function fetching the probe value (async) and write it back to provider
            return ISimulator::requestElevation(reference, callsign);
        }

        // convert xplane squawk mode to swift squawk mode
        CTransponder::TransponderMode xpdrMode(int xplaneMode, bool ident)
        {
            if (ident) { return CTransponder::StateIdent; }
            if (xplaneMode == 0 || xplaneMode == 1) { return CTransponder::StateStandby; }
            return CTransponder::ModeC;
        }

        // convert swift squawk mode to xplane squawk mode
        int xpdrMode(CTransponder::TransponderMode mode)
        {
            return mode == CTransponder::StateStandby ? 1 : 2;
        }

        void CSimulatorXPlane::fastTimerTimeout()
        {
            if (this->isConnected())
            {
                m_serviceProxy->getOwnAircraftSituationData(&m_xplaneData);
                m_serviceProxy->getCom1ActiveAsync(&m_xplaneData.com1Active);
                m_serviceProxy->getCom1StandbyAsync(&m_xplaneData.com1Standby);
                m_serviceProxy->getCom2ActiveAsync(&m_xplaneData.com2Active);
                m_serviceProxy->getCom2StandbyAsync(&m_xplaneData.com2Standby);
                m_serviceProxy->getTransponderCodeAsync(&m_xplaneData.xpdrCode);
                m_serviceProxy->getTransponderModeAsync(&m_xplaneData.xpdrMode);
                m_serviceProxy->getTransponderIdentAsync(&m_xplaneData.xpdrIdent);
                m_serviceProxy->getAllWheelsOnGroundAsync(&m_xplaneData.onGroundAll);

                CAircraftSituation situation;
                situation.setPosition({ m_xplaneData.latitude, m_xplaneData.longitude, 0 });
                CAltitude altitude { m_xplaneData.altitude, CAltitude::MeanSeaLevel, CLengthUnit::m() };
                situation.setAltitude({ m_xplaneData.altitude, CAltitude::MeanSeaLevel, CLengthUnit::m() });
                CPressure seaLevelPressure({ m_xplaneData.seaLeveLPressure, CPressureUnit::inHg() });
                CAltitude pressureAltitude(altitude.toPressureAltitude(seaLevelPressure));
                situation.setPressureAltitude(pressureAltitude);
                situation.setHeading({ m_xplaneData.trueHeading, CHeading::True, CAngleUnit::deg() });
                situation.setPitch({ m_xplaneData.pitch, CAngleUnit::deg() });
                situation.setBank({ m_xplaneData.roll, CAngleUnit::deg() });
                situation.setGroundSpeed({ m_xplaneData.groundspeed, CSpeedUnit::m_s() });

                // updates
                updateOwnIcaoCodes(m_xplaneData.aircraftIcaoCode, CAirlineIcaoCode());
                updateOwnSituation(situation);
                updateCockpit(
                    CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() }),
                    CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() }),
                    CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent)),
                    identifier()
                );

                if (m_isWeatherActivated)
                {
                    const auto currentPosition = CCoordinateGeodetic { situation.latitude(), situation.longitude(), {0} };
                    if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()) &&
                            calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
                    {
                        m_lastWeatherPosition = currentPosition;
                        const auto weatherGrid = CWeatherGrid { { "", currentPosition } };
                        requestWeatherGrid(weatherGrid, { this, &CSimulatorXPlane::injectWeatherGrid });
                    }
                }
            }
        }

        void CSimulatorXPlane::slowTimerTimeout()
        {
            if (isConnected())
            {
                m_serviceProxy->getAircraftModelPathAsync(&m_xplaneData.aircraftModelPath);
                m_serviceProxy->getAircraftIcaoCodeAsync(&m_xplaneData.aircraftIcaoCode);
                m_serviceProxy->getBeaconLightsOnAsync(&m_xplaneData.beaconLightsOn);
                m_serviceProxy->getLandingLightsOnAsync(&m_xplaneData.landingLightsOn);
                m_serviceProxy->getNavLightsOnAsync(&m_xplaneData.navLightsOn);
                m_serviceProxy->getStrobeLightsOnAsync(&m_xplaneData.strobeLightsOn);
                m_serviceProxy->getTaxiLightsOnAsync(&m_xplaneData.taxiLightsOn);
                m_serviceProxy->getFlapsDeployRatioAsync(&m_xplaneData.flapsReployRatio);
                m_serviceProxy->getGearDeployRatioAsync(&m_xplaneData.gearReployRatio);
                m_serviceProxy->getEngineN1PercentageAsync(&m_xplaneData.enginesN1Percentage);
                m_serviceProxy->getSpeedBrakeRatioAsync(&m_xplaneData.speedBrakeRatio);

                CAircraftEngineList engines;
                for (int engineNumber = 0; engineNumber < m_xplaneData.enginesN1Percentage.size(); ++engineNumber)
                {
                    // Engine number start counting at 1
                    // We consider the engine running when N1 is bigger than 5 %
                    CAircraftEngine engine {engineNumber + 1, m_xplaneData.enginesN1Percentage.at(engineNumber) > 5.0};
                    engines.push_back(engine);
                }

                CAircraftParts parts { {
                        m_xplaneData.strobeLightsOn, m_xplaneData.landingLightsOn, m_xplaneData.taxiLightsOn,
                        m_xplaneData.beaconLightsOn, m_xplaneData.navLightsOn, false
                    },
                    m_xplaneData.gearReployRatio > 0, static_cast<int>(m_xplaneData.flapsReployRatio * 100),
                    m_xplaneData.speedBrakeRatio > 0.5, engines, m_xplaneData.onGroundAll
                };

                this->updateOwnParts(parts);
                this->requestRemoteAircraftDataFromXPlane();
            }
        }

        bool CSimulatorXPlane::isConnected() const
        {
            return m_serviceProxy && m_trafficProxy && m_weatherProxy;
        }

        bool CSimulatorXPlane::connectTo()
        {
            if (isConnected()) { return true; }
            m_conn = QDBusConnection::sessionBus(); // TODO make this configurable
            m_serviceProxy = new CXSwiftBusServiceProxy(m_conn, this);
            m_trafficProxy = new CXSwiftBusTrafficProxy(m_conn, this);
            m_weatherProxy = new CXSwiftBusWeatherProxy(m_conn, this);

            if (m_serviceProxy->isValid() && m_trafficProxy->isValid() && m_weatherProxy->isValid() && m_trafficProxy->initialize())
            {
                emitOwnAircraftModelChanged(m_serviceProxy->getAircraftModelPath(), m_serviceProxy->getAircraftModelFilename(), m_serviceProxy->getAircraftLivery(),
                                            m_serviceProxy->getAircraftIcaoCode(), m_serviceProxy->getAircraftModelString(), m_serviceProxy->getAircraftName(), m_serviceProxy->getAircraftDescription());
                connect(m_serviceProxy, &CXSwiftBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::emitOwnAircraftModelChanged);
                connect(m_serviceProxy, &CXSwiftBusServiceProxy::airportsInRangeUpdated, this, &CSimulatorXPlane::setAirportsInRange);
                m_serviceProxy->updateAirportsInRange();
                connect(m_trafficProxy, &CXSwiftBusTrafficProxy::simFrame, this, &CSimulatorXPlane::updateRemoteAircraft);
                connect(m_trafficProxy, &CXSwiftBusTrafficProxy::remoteAircraftData, this, &CSimulatorXPlane::updateRemoteAircraftFromSimulator);
                if (m_watcher) { m_watcher->setConnection(m_conn); }
                this->loadCslPackages();
                this->emitSimulatorCombinedStatus();
                return true;
            }
            else
            {
                disconnectFrom();
                return false;
            }
        }

        bool CSimulatorXPlane::disconnectFrom()
        {
            if (!this->isConnected()) { return true; } // avoid emit if already disconnected
            if (m_trafficProxy)
            {
                m_trafficProxy->cleanup();
            }

            m_conn = QDBusConnection { "default" };
            if (m_watcher) { m_watcher->setConnection(m_conn); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            delete m_weatherProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            m_weatherProxy = nullptr;
            emitSimulatorCombinedStatus();
            return true;
        }

        void CSimulatorXPlane::serviceUnregistered()
        {
            m_conn = QDBusConnection { "default" };
            if (m_watcher) { m_watcher->setConnection(m_conn); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            delete m_weatherProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            m_weatherProxy = nullptr;
            emitSimulatorCombinedStatus();
        }

        void CSimulatorXPlane::emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                const QString &icao, const QString &modelString, const QString &name, const QString &description)
        {
            CAircraftModel model(modelString, CAircraftModel::TypeOwnSimulatorModel, CSimulatorInfo::XPLANE, name, description, icao);
            if (!livery.isEmpty()) { model.setModelString(model.getModelString() + " " + livery); }
            model.setFileName(path + "/" + filename);

            this->reverseLookupAndUpdateOwnAircraftModel(model);
        }

        void CSimulatorXPlane::displayStatusMessage(const CStatusMessage &message) const
        {
            // No assert here as status message may come because of network problems
            if (!isConnected()) { return; }

            // avoid infinite recursion in case this function is called due to a message caused by this very function
            static bool isInFunction = false;
            if (isInFunction) { return; }
            isInFunction = true;

            QColor color = "cyan";
            /* switch (message.getSeverity())
            {
            case CStatusMessage::SeverityDebug: color = "teal"; break;
            case CStatusMessage::SeverityInfo: color = "cyan"; break;
            case CStatusMessage::SeverityWarning: color = "orange"; break;
            case CStatusMessage::SeverityError: color = "red"; break;
            } */

            m_serviceProxy->addTextMessage("swift: " + message.getMessage(), color.redF(), color.greenF(), color.blueF());
            isInFunction = false;
        }

        void CSimulatorXPlane::displayTextMessage(const Network::CTextMessage &message) const
        {
            Q_ASSERT(isConnected());

            QColor color;
            if (message.isServerMessage()) { color = "orchid"; }
            else if (message.isSupervisorMessage()) { color = "yellow"; }
            else if (message.isPrivateMessage()) { color = "magenta"; }
            else { color = "lime"; }

            m_serviceProxy->addTextMessage(message.getSenderCallsign().toQString() + ": " + message.getMessage(), color.redF(), color.greenF(), color.blueF());
        }

        void CSimulatorXPlane::setAirportsInRange(const QStringList &icaos, const QStringList &names, const CSequence<double> &lats, const CSequence<double> &lons, const CSequence<double> &alts)
        {
            //! \todo restrict to maxAirportsInRange()
            m_airportsInRange.clear();
            auto icaoIt = icaos.begin();
            auto nameIt = names.begin();
            auto latIt = lats.begin();
            auto lonIt = lons.begin();
            auto altIt = alts.begin();
            for (; icaoIt != icaos.end() && nameIt != names.end() && latIt != lats.end() && lonIt != lons.end() && altIt != alts.end(); ++icaoIt, ++nameIt, ++latIt, ++lonIt, ++altIt)
            {
                using namespace PhysicalQuantities;
                using namespace Geo;

                m_airportsInRange.push_back({ *icaoIt, { CLatitude(*latIt, CAngleUnit::deg()), CLongitude(*lonIt, CAngleUnit::deg()), CAltitude(*altIt, CLengthUnit::m()) }, *nameIt });
            }
        }

        CAirportList CSimulatorXPlane::getAirportsInRange() const
        {
            return m_airportsInRange;
        }

        bool CSimulatorXPlane::setTimeSynchronization(bool enable, const PhysicalQuantities::CTime &offset)
        {
            Q_UNUSED(offset);
            if (enable)
            {
                CLogMessage(this).info("X-Plane provides real time synchronization, use this one");
            }
            return false;
        }

        QDBusConnection CSimulatorXPlane::connectionFromString(const QString &str)
        {
            if (str == CDBusServer::sessionBusAddress()) { return QDBusConnection::sessionBus(); }
            if (str == CDBusServer::systemBusAddress())  { return QDBusConnection::systemBus(); }

            Q_UNREACHABLE();
            return QDBusConnection("NO CONNECTION");
        }

        bool CSimulatorXPlane::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            return m_xplaneAircraftObjects.contains(callsign);
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const Simulation::CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_ASSERT(this->isConnected());
            if (originator == this->identifier()) { return false; }
            auto com1 = CComSystem::getCom1System({ m_xplaneData.com1Active, CFrequencyUnit::kHz() }, { m_xplaneData.com1Standby, CFrequencyUnit::kHz() });
            auto com2 = CComSystem::getCom2System({ m_xplaneData.com2Active, CFrequencyUnit::kHz() }, { m_xplaneData.com2Standby, CFrequencyUnit::kHz() });
            auto xpdr = CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_xplaneData.com1Active = aircraft.getCom1System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com1Standby = aircraft.getCom1System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Active = aircraft.getCom2System().getFrequencyActive().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.com2Standby = aircraft.getCom2System().getFrequencyStandby().valueRounded(CFrequencyUnit::kHz(), 0);
                m_xplaneData.xpdrCode = aircraft.getTransponderCode();
                m_xplaneData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
                m_serviceProxy->setCom1Active(m_xplaneData.com1Active);
                m_serviceProxy->setCom1Standby(m_xplaneData.com1Standby);
                m_serviceProxy->setCom2Active(m_xplaneData.com2Active);
                m_serviceProxy->setCom2Standby(m_xplaneData.com2Standby);
                m_serviceProxy->setTransponderCode(m_xplaneData.xpdrCode);
                m_serviceProxy->setTransponderMode(m_xplaneData.xpdrMode);

                m_serviceProxy->cancelAllPendingAsyncCalls(); // in case there is already a reply with some old data incoming
                return true;
            }
            return false;
        }

        bool CSimulatorXPlane::updateOwnSimulatorSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            Q_ASSERT(this->isConnected());
            if (originator == this->identifier()) { return false; }
            Q_UNUSED(selcal);

            //! \fixme KB 8/2017 use SELCAL??
            return false;
        }

        void CSimulatorXPlane::loadCslPackages()
        {
            struct Prefix { QString s; };
            struct PrefixComparator
            {
                bool operator()(const Prefix &a, const QString &b) const { return QStringRef(&a.s) < b.leftRef(a.s.size()); }
                bool operator()(const QString &a, const Prefix &b) const { return a.leftRef(b.s.size()) < QStringRef(&b.s); }
            };
            QList<Prefix> packages;

            Q_ASSERT(isConnected());
            for (const auto &model : m_modelSet.getThreadLocal())
            {
                const QString &modelFile = model.getFileName();
                if (modelFile.isEmpty() || ! QFile::exists(modelFile)) { continue; }
                auto it = std::lower_bound(packages.begin(), packages.end(), modelFile, PrefixComparator());
                if (it != packages.end() && modelFile.startsWith(it->s)) { continue; }
                QString package = findCslPackage(modelFile);
                if (package.isEmpty()) { continue; }
                packages.insert(it, { package.append('/') });
            }
            for (auto &package : packages)
            {
                Q_ASSERT(package.s.endsWith('/'));
                package.s.chop(1);
                m_trafficProxy->loadPlanesPackage(package.s);
            }
        }

        QString CSimulatorXPlane::findCslPackage(const QString &modelFile)
        {
            //! \todo KB 2018-02 KB when I have removed the CSL dir (acciedently) there was no warning here
            const QFileInfo info(modelFile);
            QDir dir = info.isDir() ? QDir(modelFile) : info.dir();
            do
            {
                if (dir.exists(QStringLiteral("xsb_aircraft.txt")))
                {
                    if (dir.cdUp()) { return dir.path(); }
                }
            }
            while (dir.cdUp());
            CLogMessage(this).warning("Failed to find CSL package for %1") << modelFile;
            return {};
        }

        bool CSimulatorXPlane::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            Q_ASSERT(isConnected());
            // entry checks
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!newRemoteAircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "empty callsign");
            Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

            m_xplaneAircraftObjects.insert(newRemoteAircraft.getCallsign(), CXPlaneMPAircraft(newRemoteAircraft, this, &m_interpolationLogger));
            CAircraftModel aircraftModel = newRemoteAircraft.getModel();
            QString livery = aircraftModel.getLivery().getCombinedCode(); //! \todo livery resolution for XP
            m_trafficProxy->addPlane(newRemoteAircraft.getCallsign().asString(), aircraftModel.getModelString(),
                                     newRemoteAircraft.getAircraftIcaoCode().getDesignator(),
                                     newRemoteAircraft.getAirlineIcaoCode().getDesignator(),
                                     livery);

            CLogMessage(this).info("XP: Added aircraft %1") << newRemoteAircraft.getCallsign().toQString();

            bool rendered = true;
            updateAircraftRendered(newRemoteAircraft.getCallsign(), rendered);

            CSimulatedAircraft remoteAircraftCopy(newRemoteAircraft);
            remoteAircraftCopy.setRendered(rendered);
            emit this->aircraftRenderingChanged(remoteAircraftCopy);
            return true;
        }

        bool CSimulatorXPlane::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            Q_ASSERT(isConnected());

            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "wrong thread");
            if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

            // really remove from simulator
            if (!m_xplaneAircraftObjects.contains(callsign)) { return false; } // already fully removed or not yet added

            // mark in provider
            const bool updated = this->updateAircraftRendered(callsign, false);
            if (updated)
            {
                Q_ASSERT_X(m_xplaneAircraftObjects.contains(callsign), Q_FUNC_INFO, "Aircraft removed");
                const CXPlaneMPAircraft &xplaneAircraft = m_xplaneAircraftObjects[callsign];
                CSimulatedAircraft aircraft(xplaneAircraft.getAircraft());
                aircraft.setRendered(false);
                emit this->aircraftRenderingChanged(aircraft);
            }

            m_trafficProxy->removePlane(callsign.asString());
            m_xplaneAircraftObjects.remove(callsign);

            // bye
            return true;
        }

        int CSimulatorXPlane::physicallyRemoveAllRemoteAircraft()
        {
            Q_ASSERT(isConnected());
            //! \todo XP driver obtain number of removed aircraft
            resetHighlighting();

            // remove one by one
            int r = 0;
            const CCallsignSet callsigns = m_xplaneAircraftObjects.getAllCallsigns();
            for (const CCallsign &cs : callsigns)
            {
                if (this->physicallyRemoveRemoteAircraft(cs)) { r++; }
            }
            return r;
        }

        CCallsignSet CSimulatorXPlane::physicallyRenderedAircraft() const
        {
            //! \todo XP driver, return list of callsigns really present in the simulator
            return this->getAircraftInRange().findByRendered(true).getCallsigns(); // just a poor workaround
        }

        bool CSimulatorXPlane::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            // remove upfront, and then enable / disable again
            auto callsign = aircraft.getCallsign();
            if (!this->isPhysicallyRenderedAircraft(callsign)) { return false; }
            this->physicallyRemoveRemoteAircraft(callsign);
            return this->changeRemoteAircraftEnabled(aircraft);
        }

        bool CSimulatorXPlane::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
        {
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

        void CSimulatorXPlane::injectWeatherGrid(const Weather::CWeatherGrid &weatherGrid)
        {
            Q_ASSERT(isConnected());
            m_weatherProxy->setUseRealWeather(false);

            // todo: find the closest
            CGridPoint gridPoint = weatherGrid.front();

            // todo: find the closest
            auto visibilityLayers = gridPoint.getVisibilityLayers();
            visibilityLayers.sortBy(&CVisibilityLayer::getBase);
            const CVisibilityLayer visibilityLayer = visibilityLayers.frontOrDefault();
            m_weatherProxy->setVisibility(visibilityLayer.getVisibility().value(CLengthUnit::m()));

            CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers();
            temperatureLayers.sortBy(&CTemperatureLayer::getLevel);
            const CTemperatureLayer temperatureLayer = temperatureLayers.frontOrDefault();
            m_weatherProxy->setTemperature(temperatureLayer.getTemperature().value(CTemperatureUnit::C()));
            m_weatherProxy->setDewPoint(temperatureLayer.getDewPoint().value(CTemperatureUnit::C()));
            m_weatherProxy->setQNH(gridPoint.getSurfacePressure().value(CPressureUnit::inHg()));

            int layerNumber = 0;
            CCloudLayerList cloudLayers = gridPoint.getCloudLayers();
            auto numberOfLayers = cloudLayers.size();
            // Fill cloud layers if less then 3
            while (numberOfLayers < 3)
            {
                cloudLayers.push_back(CCloudLayer());
                numberOfLayers++;
            }
            cloudLayers.sortBy(&CCloudLayer::getBase);
            // todo: Instead of truncate, find the 3 vertical closest cloud layers
            cloudLayers.truncate(3);
            for (const auto &cloudLayer : cloudLayers)
            {
                int base = cloudLayer.getBase().value(CLengthUnit::m());
                int top = cloudLayer.getTop().value(CLengthUnit::m());

                int coverage = 0;
                switch (cloudLayer.getCoverage())
                {
                case CCloudLayer::None: coverage = 0; break;
                case CCloudLayer::Few: coverage = 2; break;
                case CCloudLayer::Scattered: coverage = 3; break;
                case CCloudLayer::Broken: coverage = 4; break;
                case CCloudLayer::Overcast: coverage = 6; break;
                default: coverage = 0;
                }

                // Clear = 0, High Cirrus = 1, Scattered = 2, Broken = 3, Overcast = 4, Stratus = 5
                int type = 0;
                switch (cloudLayer.getClouds())
                {
                case CCloudLayer::NoClouds: type = 0; break;
                case CCloudLayer::Cirrus: type = 1; break;
                case CCloudLayer::Stratus: type = 5; break;
                default: type = 0;
                }

                m_weatherProxy->setCloudLayer(layerNumber, base, top, type, coverage);
                layerNumber++;
            }

            layerNumber = 0;
            CWindLayerList windLayers = gridPoint.getWindLayers();
            numberOfLayers = windLayers.size();
            // Fill cloud layers if less then 3
            while (numberOfLayers < 3)
            {
                windLayers.push_back(CWindLayer());
                numberOfLayers++;
            }
            windLayers.sortBy(&CWindLayer::getLevel);
            // todo: Instead of truncate, find the 3 vertical closest cloud layers
            windLayers.truncate(3);
            for (const auto &windLayer : windLayers)
            {
                const int altitudeMeter = windLayer.getLevel().value(CLengthUnit::m());
                const double directionDeg = windLayer.getDirection().value(CAngleUnit::deg());
                const int speedKts = windLayer.getSpeed().value(CSpeedUnit::kts());
                m_weatherProxy->setWindLayer(layerNumber, altitudeMeter, directionDeg, speedKts, 0, 0, 0);
                layerNumber++;
            }

            m_weatherProxy->setPrecipitationRatio(cloudLayers.frontOrDefault().getPrecipitationRate());
            m_weatherProxy->setThunderstormRatio(0.0);
        }

        void CSimulatorXPlane::updateRemoteAircraft()
        {
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "thread");

            const int remoteAircraftNo = this->getAircraftInRangeCount();
            if (remoteAircraftNo < 1) { m_interpolationRequest = 0;  return; }

            // interpolate and send to simulator
            m_interpolationRequest++;
            const CCallsignSet aircraftWithParts = this->remoteAircraftSupportingParts(); // optimization, fetch all parts supporting aircraft in one step (one lock)

            // values used for position and parts
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

            // interpolation for all remote aircraft
            const QList<CXPlaneMPAircraft> xplaneAircraftList(m_xplaneAircraftObjects.values());
            for (const CXPlaneMPAircraft &xplaneAircraft : xplaneAircraftList)
            {
                const CCallsign callsign(xplaneAircraft.getCallsign());
                Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign);

                // interpolated situation/parts
                const CInterpolationResult result = xplaneAircraft.getInterpolation(currentTimestamp, setup);
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    const CAircraftSituation interpolatedSituation(result);

                    // update situation
                    if (!xplaneAircraft.isSameAsSent(interpolatedSituation))
                    {
                        m_xplaneAircraftObjects[xplaneAircraft.getCallsign()].setSituationAsSent(interpolatedSituation);
                        m_trafficProxy->setPlanePosition(interpolatedSituation.getCallsign().asString(),
                                                         interpolatedSituation.latitude().value(CAngleUnit::deg()),
                                                         interpolatedSituation.longitude().value(CAngleUnit::deg()),
                                                         interpolatedSituation.getAltitude().value(CLengthUnit::ft()),
                                                         interpolatedSituation.getPitch().value(CAngleUnit::deg()),
                                                         interpolatedSituation.getBank().value(CAngleUnit::deg()),
                                                         interpolatedSituation.getHeading().value(CAngleUnit::deg()));
                    }
                }
                else
                {
                    CLogMessage(this).warning(this->getInvalidSituationLogMessage(callsign, result.getInterpolationStatus()));
                }

                this->updateRemoteAircraftParts(xplaneAircraft, result);

            } // all callsigns

            const qint64 dt = QDateTime::currentMSecsSinceEpoch() - currentTimestamp;
            m_statsUpdateAircraftTimeTotalMs += dt;
            m_statsUpdateAircraftCountMs++;
            m_statsUpdateAircraftTimeAvgMs = m_statsUpdateAircraftTimeTotalMs / m_statsUpdateAircraftCountMs;
        }

        bool CSimulatorXPlane::updateRemoteAircraftParts(const CXPlaneMPAircraft &xplaneAircraft, const CInterpolationResult &result)
        {
            if (!result.getPartsStatus().isSupportingParts()) { return false; }
            return this->sendRemoteAircraftPartsToSimulator(xplaneAircraft, result);
        }

        bool CSimulatorXPlane::sendRemoteAircraftPartsToSimulator(const CXPlaneMPAircraft &xplaneAircraft, const CAircraftParts &parts)
        {
            // same as in simulator or same as already send to simulator?
            if (xplaneAircraft.getPartsAsSent() == parts) { return true; }

            m_trafficProxy->setPlaneSurfaces(xplaneAircraft.getCallsign().asString(),
                                             parts.isGearDown() ? 1 : 0,
                                             parts.getFlapsPercent() / 100.0,
                                             parts.isSpoilersOut() ? 1 : 0,
                                             parts.isSpoilersOut() ? 1 : 0,
                                             parts.getFlapsPercent() / 100.0,
                                             0, parts.isAnyEngineOn() ? 0 : 0.75,
                                             0, 0, 0,
                                             parts.getLights().isLandingOn(), parts.getLights().isBeaconOn(), parts.getLights().isStrobeOn(), parts.getLights().isNavOn(),
                                             0, parts.isOnGround());
            return true;
        }

        void CSimulatorXPlane::requestRemoteAircraftDataFromXPlane()
        {
            if (!isConnected()) { return; }
            m_trafficProxy->requestRemoteAircraftData();
        }

        void CSimulatorXPlane::updateRemoteAircraftFromSimulator(const QString &callsign, double latitudeDeg, double longitudeDeg, double elevationMeters, double modelVerticalOffsetMeters)
        {
            // we skip if we are not near ground
            const CCallsign cs(callsign);
            if (!m_xplaneAircraftObjects.contains(cs)) { return; }
            if (m_xplaneAircraftObjects[cs].getSituationAsSent().canLikelySkipNearGroundInterpolation()) { return; }

            CElevationPlane elevation(CLatitude(latitudeDeg, CAngleUnit::deg()), CLongitude(longitudeDeg, CAngleUnit::deg()), CAltitude(elevationMeters, CLengthUnit::m()));
            elevation.setSinglePointRadius();
            this->rememberElevationAndCG(callsign, elevation, CLength(modelVerticalOffsetMeters, CLengthUnit::m()));
        }

        void CSimulatorXPlane::updateAirportsInRange()
        {
            if (this->isConnected()) { m_serviceProxy->updateAirportsInRange(); }
        }

        BlackCore::ISimulator *CSimulatorXPlaneFactory::create(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider,
                IClientProvider *clientProvider)
        {
            return new CSimulatorXPlane(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
        }

        CSimulatorXPlaneListener::CSimulatorXPlaneListener(const CSimulatorPluginInfo &info): ISimulatorListener(info)
        { }

        void CSimulatorXPlaneListener::startImpl()
        {
            if (m_watcher) { return; } // already started
            if (isXSwiftBusRunning())
            {
                emit simulatorStarted(getPluginInfo());
            }
            else
            {
                CLogMessage(this).debug() << "Watching XSwiftBus on" << m_xswiftbusServerSetting.getThreadLocal();
                m_conn = CSimulatorXPlane::connectionFromString(m_xswiftbusServerSetting.getThreadLocal());
                m_watcher = new QDBusServiceWatcher(xswiftbusServiceName(), m_conn, QDBusServiceWatcher::WatchForRegistration, this);
                connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, &CSimulatorXPlaneListener::serviceRegistered);
            }
        }

        void CSimulatorXPlaneListener::stopImpl()
        {
            if (m_watcher)
            {
                delete m_watcher;
                m_watcher = nullptr;
            }
        }

        bool CSimulatorXPlaneListener::isXSwiftBusRunning() const
        {
            QDBusConnection conn = CSimulatorXPlane::connectionFromString(m_xswiftbusServerSetting.getThreadLocal());
            CXSwiftBusServiceProxy *service = new CXSwiftBusServiceProxy(conn);
            CXSwiftBusTrafficProxy *traffic = new CXSwiftBusTrafficProxy(conn);

            bool result = service->isValid() && traffic->isValid();

            service->deleteLater();
            traffic->deleteLater();

            return result;
        }

        void CSimulatorXPlaneListener::serviceRegistered(const QString &serviceName)
        {
            if (serviceName == xswiftbusServiceName())
            {
                emit simulatorStarted(getPluginInfo());
            }
        }

        void CSimulatorXPlaneListener::xSwiftBusServerSettingChanged()
        {
            // user changed settings, restart the listener
            if (m_watcher)
            {
                stop();
                start();
            }
        }
    } // namespace
} // namespace

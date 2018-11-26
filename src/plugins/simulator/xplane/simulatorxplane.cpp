/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorxplane.h"
#include "qcompilerdetection.h"
#include "xswiftbusserviceproxy.h"
#include "xswiftbustrafficproxy.h"
#include "xswiftbusweatherproxy.h"
#include "blackcore/aircraftmatcher.h"
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
#include "blackmisc/network/textmessage.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/temperature.h"
#include "blackmisc/verify.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbusserver.h"
#include "blackmisc/iterator.h"
#include "blackmisc/logmessage.h"
#include "blackconfig/buildconfig.h"

#include "dbus/dbus.h"

#include <QColor>
#include <QDBusServiceWatcher>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <QPointer>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;

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
            CSimulatorPluginCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            m_watcher = new QDBusServiceWatcher(this);
            m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
            m_watcher->addWatchedService(xswiftbusServiceName());
            m_watcher->setObjectName("QDBusServiceWatcher");
            connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, &CSimulatorXPlane::serviceUnregistered);

            m_fastTimer.setObjectName(this->objectName().append(":m_fastTimer"));
            m_slowTimer.setObjectName(this->objectName().append(":m_slowTimer"));
            m_pendingAddedTimer.setObjectName(this->objectName().append(":m_pendingAddedTimer"));
            connect(&m_fastTimer, &QTimer::timeout, this, &CSimulatorXPlane::fastTimerTimeout);
            connect(&m_slowTimer, &QTimer::timeout, this, &CSimulatorXPlane::slowTimerTimeout);
            connect(&m_airportUpdater, &QTimer::timeout, this, &CSimulatorXPlane::updateAirportsInRange);
            connect(&m_pendingAddedTimer, &QTimer::timeout, this, &CSimulatorXPlane::addNextPendingAircraft);
            m_fastTimer.start(100);
            m_slowTimer.start(1000);
            m_airportUpdater.start(60 * 1000);
            m_pendingAddedTimer.start(5000);

            this->setDefaultModel({ "Jets A320_a A320_a_Austrian_Airlines A320_a_Austrian_Airlines", CAircraftModel::TypeModelMatchingDefaultModel,
                                    "A320 AUA", CAircraftIcaoCode("A320", "L2J")});
            this->resetXPlaneData();
        }

        CSimulatorXPlane::~CSimulatorXPlane()
        {
            this->unload();
        }

        void CSimulatorXPlane::unload()
        {
            if (!this->isConnected()) { return; }

            // will call disconnect from
            CSimulatorPluginCommon::unload();
            delete m_watcher;
            m_watcher = nullptr;
        }

        QString CSimulatorXPlane::getStatisticsSimulatorSpecific() const
        {
            static const QString s("Add-time: %1ms/%2ms");
            return s.arg(m_statsAddCurrentTimeMs).arg(m_statsAddMaxTimeMs);
        }

        void CSimulatorXPlane::resetAircraftStatistics()
        {
            m_statsAddMaxTimeMs = -1;
            m_statsAddCurrentTimeMs = -1;
        }

        CStatusMessageList CSimulatorXPlane::getInterpolationMessages(const CCallsign &callsign) const
        {
            if (!m_xplaneAircraftObjects.contains(callsign)) { return CStatusMessageList(); }
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, false);
            return m_xplaneAircraftObjects[callsign].getInterpolationMessages(setup.getInterpolatorMode());
        }

        void CSimulatorXPlane::clearAllRemoteAircraftData()
        {
            m_aircraftAddedFailed.clear();
            CSimulatorPluginCommon::clearAllRemoteAircraftData();
        }

        bool CSimulatorXPlane::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
        {
            if (this->isShuttingDown()) { return false; }
            if (reference.isNull()) { return false; }

            CCoordinateGeodetic pos(reference);
            if (!pos.hasMSLGeodeticHeight())
            {
                // testing showed: height has an influence on the returned result
                // - the most accurate value seems to be returned if the height is close to the elevation
                // - in normal scenarios there is no much difference of the results if 0 is used
                // - in Lukla (9200ft MSL) the difference between 0 and 9200 is around 1ft
                // - in the LOWW scenario using 50000ft MSL results in around 3ft too low elevation
                static const CAltitude alt(0, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                pos.setGeodeticHeight(alt);
            }

            using namespace std::placeholders;
            auto callback = std::bind(&CSimulatorXPlane::callbackReceivedRequestedElevation, this, _1, _2);

            // Request
            m_trafficProxy->getElevationAtPosition(callsign,
                                                   pos.latitude().value(CAngleUnit::deg()),
                                                   pos.longitude().value(CAngleUnit::deg()),
                                                   pos.geodeticHeight().value(CLengthUnit::m()),
                                                   callback);
            emit this->requestedElevation(callsign);
            return true;
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
                m_serviceProxy->getCom1ActiveKhzAsync(&m_xplaneData.com1ActiveKhz);
                m_serviceProxy->getCom1StandbyKhzAsync(&m_xplaneData.com1StandbyKhz);
                m_serviceProxy->getCom2ActiveKhzAsync(&m_xplaneData.com2ActiveKhz);
                m_serviceProxy->getCom2StandbyKhzAsync(&m_xplaneData.com2StandbyKhz);
                m_serviceProxy->getTransponderCodeAsync(&m_xplaneData.xpdrCode);
                m_serviceProxy->getTransponderModeAsync(&m_xplaneData.xpdrMode);
                m_serviceProxy->getTransponderIdentAsync(&m_xplaneData.xpdrIdent);
                m_serviceProxy->getAllWheelsOnGroundAsync(&m_xplaneData.onGroundAll);

                CAircraftSituation situation;
                situation.setPosition({ m_xplaneData.latitudeDeg, m_xplaneData.longitudeDeg, 0 });
                CAltitude altitude { m_xplaneData.altitudeM, CAltitude::MeanSeaLevel, CLengthUnit::m() };
                situation.setAltitude({ m_xplaneData.altitudeM, CAltitude::MeanSeaLevel, CLengthUnit::m() });
                CPressure seaLevelPressure({ m_xplaneData.seaLevelPressureInHg, CPressureUnit::inHg() });
                CAltitude pressureAltitude(altitude.toPressureAltitude(seaLevelPressure));
                situation.setPressureAltitude(pressureAltitude);
                situation.setHeading({ m_xplaneData.trueHeadingDeg, CHeading::True, CAngleUnit::deg() });
                situation.setPitch({ m_xplaneData.pitchDeg, CAngleUnit::deg() });
                situation.setBank({ m_xplaneData.rollDeg, CAngleUnit::deg() });
                situation.setGroundSpeed({ m_xplaneData.groundspeedMs, CSpeedUnit::m_s() });

                // updates
                updateOwnIcaoCodes(m_xplaneData.aircraftIcaoCode, CAirlineIcaoCode());
                updateOwnSituation(situation);

                // defaults
                CSimulatedAircraft myAircraft(getOwnAircraft());
                CComSystem com1(myAircraft.getCom1System()); // set defaults
                CComSystem com2(myAircraft.getCom2System());
                CTransponder transponder(myAircraft.getTransponder());

                // updates
                com1.setFrequencyActive(CFrequency(m_xplaneData.com1ActiveKhz, CFrequencyUnit::kHz()));
                com1.setFrequencyStandby(CFrequency(m_xplaneData.com1StandbyKhz, CFrequencyUnit::kHz()));
                const bool changedCom1 = myAircraft.getCom1System() != com1;

                com2.setFrequencyActive(CFrequency(m_xplaneData.com2ActiveKhz, CFrequencyUnit::kHz()));
                com2.setFrequencyStandby(CFrequency(m_xplaneData.com2StandbyKhz, CFrequencyUnit::kHz()));
                const bool changedCom2 = myAircraft.getCom2System() != com2;

                transponder = CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
                const bool changedXpr = (myAircraft.getTransponderCode() != transponder.getTransponderCode());

                if (changedCom1 || changedCom2 || changedXpr)
                {
                    this->updateCockpit(com1, com2, transponder, identifier());
                }

                if (m_isWeatherActivated)
                {
                    const auto currentPosition = CCoordinateGeodetic { situation.latitude(), situation.longitude() };
                    if (CWeatherScenario::isRealWeatherScenario(m_weatherScenarioSettings.get()))
                    {
                        if (m_lastWeatherPosition.isNull() ||
                                calculateGreatCircleDistance(m_lastWeatherPosition, currentPosition).value(CLengthUnit::mi()) > 20)
                        {
                            m_lastWeatherPosition = currentPosition;
                            const auto weatherGrid = CWeatherGrid { { "GLOB", currentPosition } };
                            requestWeatherGrid(weatherGrid, { this, &CSimulatorXPlane::injectWeatherGrid });
                        }
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
            QString dbusAddress = m_xswiftbusServerSetting.getThreadLocal();

            if (BlackMisc::CDBusServer::isSessionOrSystemAddress(dbusAddress))
            {
                m_dBusConnection = connectionFromString(dbusAddress);
                m_dbusMode = Session;
            }
            else if (BlackMisc::CDBusServer::isQtDBusAddress(dbusAddress))
            {
                m_dBusConnection = QDBusConnection::connectToPeer(dbusAddress, "xswiftbus");
                if (! m_dBusConnection.isConnected()) { return false; }
                m_dbusMode = P2P;
            }

            m_serviceProxy = new CXSwiftBusServiceProxy(m_dBusConnection, this);
            m_trafficProxy = new CXSwiftBusTrafficProxy(m_dBusConnection, this);
            m_weatherProxy = new CXSwiftBusWeatherProxy(m_dBusConnection, this);

            bool s = m_dBusConnection.connect(QString(), DBUS_PATH_LOCAL, DBUS_INTERFACE_LOCAL,
                                              "Disconnected", this, SLOT(serviceUnregistered()));
            Q_ASSERT(s);
            if (!m_serviceProxy->isValid() || !m_trafficProxy->isValid() || !m_weatherProxy->isValid())
            {
                this->disconnectFrom();
                return false;
            }

            emitOwnAircraftModelChanged(m_serviceProxy->getAircraftModelPath(), m_serviceProxy->getAircraftModelFilename(), m_serviceProxy->getAircraftLivery(),
                                        m_serviceProxy->getAircraftIcaoCode(), m_serviceProxy->getAircraftModelString(), m_serviceProxy->getAircraftName(), m_serviceProxy->getAircraftDescription());
            QString xplaneVersion = QString("%1.%2").arg(m_serviceProxy->getXPlaneVersionMajor()).arg(m_serviceProxy->getXPlaneVersionMinor());
            setSimulatorDetails("X-Plane", {}, xplaneVersion);
            connect(m_serviceProxy, &CXSwiftBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::emitOwnAircraftModelChanged);
            connect(m_serviceProxy, &CXSwiftBusServiceProxy::airportsInRangeUpdated, this, &CSimulatorXPlane::setAirportsInRange);
            m_serviceProxy->updateAirportsInRange();
            connect(m_trafficProxy, &CXSwiftBusTrafficProxy::simFrame, this, &CSimulatorXPlane::updateRemoteAircraft);
            connect(m_trafficProxy, &CXSwiftBusTrafficProxy::remoteAircraftAdded, this, &CSimulatorXPlane::onRemoteAircraftAdded);
            connect(m_trafficProxy, &CXSwiftBusTrafficProxy::remoteAircraftAddingFailed, this, &CSimulatorXPlane::onRemoteAircraftAddingFailed);
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            m_trafficProxy->removeAllPlanes();
            this->loadCslPackages();
            this->emitSimulatorCombinedStatus();

            this->initSimulatorInternals();
            return true;
        }

        bool CSimulatorXPlane::disconnectFrom()
        {
            if (!this->isConnected()) { return true; } // avoid emit if already disconnected
            this->disconnectFromDBus();
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            delete m_weatherProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            m_weatherProxy = nullptr;
            this->emitSimulatorCombinedStatus();
            return true;
        }

        void CSimulatorXPlane::serviceUnregistered()
        {
            if (m_dbusMode == P2P) { m_dBusConnection.disconnectFromPeer(m_dBusConnection.name()); }
            m_dBusConnection = QDBusConnection { "default" };
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            delete m_weatherProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            m_weatherProxy = nullptr;
            this->emitSimulatorCombinedStatus();
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
                m_airportsInRange.push_back({ *icaoIt, { CLatitude(*latIt, CAngleUnit::deg()), CLongitude(*lonIt, CAngleUnit::deg()), CAltitude(*altIt, CLengthUnit::m()) }, *nameIt });
            }
        }

        CAirportList CSimulatorXPlane::getAirportsInRange(bool recalculateDistance) const
        {
            if (!recalculateDistance) { return m_airportsInRange; }
            CAirportList airports(m_airportsInRange);
            airports.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition());
            return airports;
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
            auto com1 = CComSystem::getCom1System({ m_xplaneData.com1ActiveKhz, CFrequencyUnit::kHz() }, { m_xplaneData.com1StandbyKhz, CFrequencyUnit::kHz() });
            auto com2 = CComSystem::getCom2System({ m_xplaneData.com2ActiveKhz, CFrequencyUnit::kHz() }, { m_xplaneData.com2StandbyKhz, CFrequencyUnit::kHz() });
            auto xpdr = CTransponder::getStandardTransponder(m_xplaneData.xpdrCode, xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_xplaneData.com1ActiveKhz  = aircraft.getCom1System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
                m_xplaneData.com1StandbyKhz = aircraft.getCom1System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
                m_xplaneData.com2ActiveKhz  = aircraft.getCom2System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
                m_xplaneData.com2StandbyKhz = aircraft.getCom2System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
                m_xplaneData.xpdrCode = aircraft.getTransponderCode();
                m_xplaneData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
                m_serviceProxy->setCom1ActiveKhz(m_xplaneData.com1ActiveKhz);
                m_serviceProxy->setCom1StandbyKhz(m_xplaneData.com1StandbyKhz);
                m_serviceProxy->setCom2ActiveKhz(m_xplaneData.com2ActiveKhz);
                m_serviceProxy->setCom2StandbyKhz(m_xplaneData.com2StandbyKhz);
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
            const CAircraftModelList models = m_modelSet.getThreadLocal();
            for (const auto &model : models)
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

            // crosscheck if still a valid aircraft
            // it can happen that aircraft has been removed, timed out ...
            if (!this->isAircraftInRange(newRemoteAircraft.getCallsign()))
            {
                // next cycle will be called by callbacks or timer
                CLogMessage(this).warning("Aircraft '%1' no longer in range, will not add") << newRemoteAircraft.getCallsign();
                return false;
            }

            if (this->canAddAircraft())
            {
                // no aircraft pending, add
                CLogMessage(this).info("Adding '%1' to XPlane") << newRemoteAircraft.getCallsign();
                const qint64 now = QDateTime::currentMSecsSinceEpoch();
                m_addingInProgressAircraft.insert(newRemoteAircraft.getCallsign(), now);
                const QString callsign = newRemoteAircraft.getCallsign().asString();
                const CAircraftModel aircraftModel = newRemoteAircraft.getModel();
                const QString livery = aircraftModel.getLivery().getCombinedCode(); //! \todo livery resolution for XP
                m_trafficProxy->addPlane(callsign, aircraftModel.getModelString(),
                                         newRemoteAircraft.getAircraftIcaoCode().getDesignator(),
                                         newRemoteAircraft.getAirlineIcaoCode().getDesignator(),
                                         livery);
            }
            else
            {
                // add in queue
                m_pendingToBeAddedAircraft.replaceOrAdd(newRemoteAircraft);
            }
            return true;
        }

        bool CSimulatorXPlane::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            Q_ASSERT(isConnected());

            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "wrong thread");
            if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

            // really remove from simulator
            if (!m_xplaneAircraftObjects.contains(callsign) && !m_pendingToBeAddedAircraft.containsCallsign(callsign) && !m_addingInProgressAircraft.contains(callsign))
            {
                // not existing aircraft
                return false;
            }

            // mark in provider
            const bool updated = this->updateAircraftRendered(callsign, false);
            if (updated)
            {
                if (m_xplaneAircraftObjects.contains(callsign))
                {
                    const CXPlaneMPAircraft &xplaneAircraft = m_xplaneAircraftObjects[callsign];
                    CSimulatedAircraft aircraft(xplaneAircraft.getAircraft());
                    aircraft.setRendered(false);
                    emit this->aircraftRenderingChanged(aircraft);
                }
                else if (m_pendingToBeAddedAircraft.containsCallsign(callsign))
                {
                    CSimulatedAircraft aircraft = m_pendingToBeAddedAircraft.findFirstByCallsign(callsign);
                    aircraft.setRendered(false);
                    emit this->aircraftRenderingChanged(aircraft);
                }
                else if (m_addingInProgressAircraft.contains(callsign))
                {
                    // we are just about to add that aircraft
                    QPointer<CSimulatorXPlane> myself(this);
                    QTimer::singleShot(TimeoutAdding, this, [ = ]
                    {
                        if (!myself) { return; }
                        m_addingInProgressAircraft.remove(callsign); // remove as "in progress"
                        this->physicallyRemoveRemoteAircraft(callsign); // and remove from sim. if it was added in the mean time
                    });
                }
            }

            m_trafficProxy->removePlane(callsign.asString());
            m_xplaneAircraftObjects.remove(callsign);
            m_pendingToBeAddedAircraft.removeByCallsign(callsign);

            // bye
            return true;
        }

        int CSimulatorXPlane::physicallyRemoveAllRemoteAircraft()
        {
            if (!this->isConnected()) { return 0; }
            m_pendingToBeAddedAircraft.clear();
            m_addingInProgressAircraft.clear();
            return CSimulatorPluginCommon::physicallyRemoveAllRemoteAircraft();
        }

        CCallsignSet CSimulatorXPlane::physicallyRenderedAircraft() const
        {
            //! \todo XP driver, return list of callsigns really present in the simulator
            return this->getAircraftInRange().findByRendered(true).getCallsigns(); // just a poor workaround
        }

        bool CSimulatorXPlane::followAircraft(const CCallsign &callsign)
        {
            if (! m_trafficProxy || ! m_trafficProxy->isValid()) { return false; }
            m_trafficProxy->setFollowedAircraft(callsign.toQString());
            return true;
        }

        void CSimulatorXPlane::injectWeatherGrid(const Weather::CWeatherGrid &weatherGrid)
        {
            Q_ASSERT(isConnected());
            m_weatherProxy->setUseRealWeather(false);

            //! TODO: find the closest
            if (weatherGrid.isEmpty()) { return; }
            const CGridPoint gridPoint = weatherGrid.front();

            // todo: find the closest
            auto visibilityLayers = gridPoint.getVisibilityLayers();
            visibilityLayers.sortBy(&CVisibilityLayer::getBase);
            const CVisibilityLayer visibilityLayer = visibilityLayers.frontOrDefault();
            m_weatherProxy->setVisibility(visibilityLayer.getVisibility().value(CLengthUnit::m()));

            CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers();
            temperatureLayers.sortBy(&CTemperatureLayer::getLevel);
            const CTemperatureLayer temperatureLayer = temperatureLayers.frontOrDefault();
            m_weatherProxy->setTemperature(temperatureLayer.getTemperature().valueInteger(CTemperatureUnit::C()));
            m_weatherProxy->setDewPoint(temperatureLayer.getDewPoint().valueInteger(CTemperatureUnit::C()));
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
            for (const auto &cloudLayer : as_const(cloudLayers))
            {
                const int base = cloudLayer.getBase().valueInteger(CLengthUnit::m());
                const int top = cloudLayer.getTop().valueInteger(CLengthUnit::m());

                int coverage = 0;
                switch (cloudLayer.getCoverage())
                {
                case CCloudLayer::None: coverage = 0; break;
                case CCloudLayer::Few: coverage = 2; break;
                case CCloudLayer::Scattered: coverage = 3; break;
                case CCloudLayer::Broken: coverage = 4; break;
                case CCloudLayer::Overcast: coverage = 6; break;
                default: coverage = 0; break;
                }

                // Clear = 0, High Cirrus = 1, Scattered = 2, Broken = 3, Overcast = 4, Stratus = 5
                int type = 0;
                switch (cloudLayer.getClouds())
                {
                case CCloudLayer::NoClouds: type = 0; break;
                case CCloudLayer::Cirrus: type = 1; break;
                case CCloudLayer::Stratus: type = 5; break;
                default: type = 0; break;
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
                const int altitudeMeter = windLayer.getLevel().valueInteger(CLengthUnit::m());
                const double directionDeg = windLayer.getDirection().value(CAngleUnit::deg());
                const int speedKts = windLayer.getSpeed().valueInteger(CSpeedUnit::kts());
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
            if (remoteAircraftNo < 1) { return; }

            // values used for position and parts
            m_updateRemoteAircraftInProgress = true;
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

            // interpolation for all remote aircraft
            const QList<CXPlaneMPAircraft> xplaneAircraftList(m_xplaneAircraftObjects.values());

            PlanesPositions planesPositions;
            PlanesSurfaces planesSurfaces;
            PlanesTransponders planesTransponders;

            int aircraftNumber = 0;
            const bool updateAllAircraft = this->isUpdateAllRemoteAircraft(currentTimestamp);
            for (const CXPlaneMPAircraft &xplaneAircraft : xplaneAircraftList)
            {
                const CCallsign callsign(xplaneAircraft.getCallsign());
                Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "missing callsign");

                planesTransponders.callsigns.push_back(callsign.asString());
                planesTransponders.codes.push_back(xplaneAircraft.getAircraft().getTransponderCode());
                CTransponder::TransponderMode transponderMode = xplaneAircraft.getAircraft().getTransponderMode();
                planesTransponders.idents.push_back(transponderMode == CTransponder::StateIdent);
                planesTransponders.modeCs.push_back(transponderMode == CTransponder::ModeC);

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, updateAllAircraft);

                // interpolated situation/parts
                const CInterpolationResult result = xplaneAircraft.getInterpolation(currentTimestamp, setup, aircraftNumber++);
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    const CAircraftSituation interpolatedSituation(result);

                    // update situation
                    if (updateAllAircraft || !this->isEqualLastSent(interpolatedSituation))
                    {
                        this->rememberLastSent(interpolatedSituation);
                        planesPositions.callsigns.push_back(interpolatedSituation.getCallsign().asString());
                        planesPositions.latitudesDeg.push_back(interpolatedSituation.latitude().value(CAngleUnit::deg()));
                        planesPositions.longitudesDeg.push_back(interpolatedSituation.longitude().value(CAngleUnit::deg()));
                        planesPositions.altitudesFt.push_back(interpolatedSituation.getAltitude().value(CLengthUnit::ft()));
                        planesPositions.pitchesDeg.push_back(interpolatedSituation.getPitch().value(CAngleUnit::deg()));
                        planesPositions.rollsDeg.push_back(interpolatedSituation.getBank().value(CAngleUnit::deg()));
                        planesPositions.headingsDeg.push_back(interpolatedSituation.getHeading().value(CAngleUnit::deg()));
                        planesPositions.onGrounds.push_back(interpolatedSituation.getOnGround() == CAircraftSituation::OnGround);
                    }
                }
                else
                {
                    CLogMessage(this).warning(this->getInvalidSituationLogMessage(callsign, result.getInterpolationStatus()));
                }

                const CAircraftParts parts(result);
                if (result.getPartsStatus().isSupportingParts() || parts.getPartsDetails() == CAircraftParts::GuessedParts)
                {
                    if (updateAllAircraft || !this->isEqualLastSent(parts, callsign))
                    {
                        this->rememberLastSent(parts, callsign);
                        planesSurfaces.callsigns.push_back(xplaneAircraft.getCallsign().asString());
                        planesSurfaces.gears.push_back(parts.isGearDown() ? 1 : 0);
                        planesSurfaces.flaps.push_back(parts.getFlapsPercent() / 100.0);
                        planesSurfaces.spoilers.push_back(parts.isSpoilersOut() ? 1 : 0);
                        planesSurfaces.speedBrakes.push_back(parts.isSpoilersOut() ? 1 : 0);
                        planesSurfaces.slats.push_back(parts.getFlapsPercent() / 100.0);
                        planesSurfaces.wingSweeps.push_back(0.0);
                        planesSurfaces.thrusts.push_back(parts.isAnyEngineOn() ? 0 : 0.75);
                        planesSurfaces.elevators.push_back(0.0);
                        planesSurfaces.rudders.push_back(0.0);
                        planesSurfaces.ailerons.push_back(0.0);
                        planesSurfaces.landLights.push_back(parts.getLights().isLandingOn());
                        planesSurfaces.beaconLights.push_back(parts.getLights().isBeaconOn());
                        planesSurfaces.strobeLights.push_back(parts.getLights().isStrobeOn());
                        planesSurfaces.navLights.push_back(parts.getLights().isNavOn());
                        planesSurfaces.lightPatterns.push_back(0);
                    }
                }

            } // all callsigns

            if (!planesTransponders.isEmpty())
            {
                m_trafficProxy->setPlanesTransponders(planesTransponders);
            }

            if (!planesPositions.isEmpty())
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    Q_ASSERT_X(planesPositions.hasSameSizes(), Q_FUNC_INFO, "Mismatching sizes");
                }
                m_trafficProxy->setPlanesPositions(planesPositions);
            }

            if (! planesSurfaces.isEmpty())
            {
                m_trafficProxy->setPlanesSurfaces(planesSurfaces);
            }

            // stats
            this->finishUpdateRemoteAircraftAndSetStatistics(currentTimestamp);
        }

        void CSimulatorXPlane::requestRemoteAircraftDataFromXPlane()
        {
            if (!isConnected()) { return; }

            // It is not required to request all elevations and CGs, but only for aircraft "near ground relevant"
            // - we could use the elevation cache and CG cache to decide if we need to request
            // - if an aircraft is on ground but not moving, we do not need to request elevation if we already have it (it will not change
            CCallsignSet callsigns = m_xplaneAircraftObjects.getAllCallsigns();
            const CCallsignSet remove = this->getLastSentCanLikelySkipNearGroundInterpolation().getCallsigns();
            callsigns.remove(remove);
            if (!callsigns.isEmpty()) { this->requestRemoteAircraftDataFromXPlane(callsigns); }
        }

        void CSimulatorXPlane::requestRemoteAircraftDataFromXPlane(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return; }
            if (this->isShuttingDown()) { return; }
            const QStringList csStrings = callsigns.getCallsignStrings();
            m_trafficProxy->getRemoteAircraftData(csStrings, [ = ](const QStringList & callsigns, const QDoubleList & latitudesDeg, const QDoubleList & longitudesDeg, const QDoubleList & elevationsMeters, const QDoubleList & verticalOffsetsMeters)
            {
                this->updateRemoteAircraftFromSimulator(callsigns, latitudesDeg, longitudesDeg, elevationsMeters, verticalOffsetsMeters);
            });
        }

        void CSimulatorXPlane::triggerRequestRemoteAircraftDataFromXPlane(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return; }
            QPointer<CSimulatorXPlane> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->requestRemoteAircraftDataFromXPlane(callsigns);
            });
        }

        void CSimulatorXPlane::updateRemoteAircraftFromSimulator(
            const QStringList &callsigns, const QDoubleList &latitudesDeg, const QDoubleList &longitudesDeg,
            const QDoubleList &elevationsMeters, const QDoubleList &verticalOffsetsMeters)
        {
            const int size = callsigns.size();

            // we skip if we are not near ground
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                Q_ASSERT_X(elevationsMeters.size() == size, Q_FUNC_INFO, "Wrong elevations");
                Q_ASSERT_X(latitudesDeg.size() == size, Q_FUNC_INFO, "Wrong latitudesDeg");
                Q_ASSERT_X(longitudesDeg.size() == size, Q_FUNC_INFO, "Wrong longitudesDeg");
                Q_ASSERT_X(verticalOffsetsMeters.size() == size, Q_FUNC_INFO, "Wrong CG");
            }

            const CCallsignSet logCallsigns = this->getLogCallsigns();
            for (int i = 0; i < size; i++)
            {
                const CCallsign cs(callsigns[i]);
                if (!m_xplaneAircraftObjects.contains(cs)) { continue; }
                const CXPlaneMPAircraft xpAircraft = m_xplaneAircraftObjects[cs];

                const CAltitude elevationAlt(elevationsMeters[i], CLengthUnit::m(), CLengthUnit::ft());
                const CElevationPlane elevation(CLatitude(latitudesDeg[i], CAngleUnit::deg()), CLongitude(longitudesDeg[i], CAngleUnit::deg()), elevationAlt, CElevationPlane::singlePointRadius());
                const CLength cg(verticalOffsetsMeters[i], CLengthUnit::m(), CLengthUnit::ft());
                this->rememberElevationAndCG(cs, xpAircraft.getAircraftModelString(), elevation, cg);

                // loopback
                if (logCallsigns.contains(cs))
                {
                    this->addLoopbackSituation(cs, elevation, cg);
                }
            }
        }

        void CSimulatorXPlane::disconnectFromDBus()
        {
            if (m_dBusConnection.isConnected())
            {
                if (m_trafficProxy) { m_trafficProxy->cleanup(); }

                if (m_dbusMode == P2P) { QDBusConnection::disconnectFromPeer(m_dBusConnection.name()); }
                else { QDBusConnection::disconnectFromBus(m_dBusConnection.name()); }
            }
            m_dBusConnection = QDBusConnection { "default" };
        }

        void CSimulatorXPlane::updateAirportsInRange()
        {
            if (this->isConnected()) { m_serviceProxy->updateAirportsInRange(); }
        }

        void CSimulatorXPlane::onRemoteAircraftAdded(const QString &callsign)
        {
            const CCallsign cs(callsign);
            CSimulatedAircraft addedRemoteAircraft = this->getAircraftInRangeForCallsign(cs);

            // statistics
            bool wasPending = false;
            if (m_addingInProgressAircraft.contains(cs))
            {
                wasPending = true;
                const qint64 wasStartedMs = m_addingInProgressAircraft.value(cs);
                const qint64 deltaTimeMs = QDateTime::currentMSecsSinceEpoch() - wasStartedMs;
                m_statsAddCurrentTimeMs = deltaTimeMs;
                if (deltaTimeMs > m_statsAddMaxTimeMs) { m_statsAddMaxTimeMs = deltaTimeMs; }
                m_addingInProgressAircraft.remove(cs);
            }

            if (!addedRemoteAircraft.hasCallsign())
            {
                CLogMessage(this).warning("Aircraft '%1' no longer in range, will be removed") << callsign;
                this->triggerRemoveAircraft(cs, TimeoutAdding);
                return;
            }

            CLogMessage(this).info("Added aircraft '%1'") << callsign;
            if (!wasPending)
            {
                // timeout?
                // slow adding?
                CLogMessage(this).warning("Added callsign '%1' was not in progress anymore. Timeout?") << callsign;
            }

            const bool rendered = true;
            addedRemoteAircraft.setRendered(rendered);
            this->updateAircraftRendered(cs, rendered);
            this->triggerRequestRemoteAircraftDataFromXPlane(cs);
            this->triggerAddNextPendingAircraft();

            m_xplaneAircraftObjects.insert(addedRemoteAircraft.getCallsign(), CXPlaneMPAircraft(addedRemoteAircraft, this, &m_interpolationLogger));
            emit this->aircraftRenderingChanged(addedRemoteAircraft);
        }

        void CSimulatorXPlane::onRemoteAircraftAddingFailed(const QString &callsign)
        {

            const CCallsign cs(callsign);
            CSimulatedAircraft failedRemoteAircraft = this->getAircraftInRangeForCallsign(cs);

            if (failedRemoteAircraft.hasCallsign())
            {
                CLogMessage(this).warning("Adding aircraft failed: '%1'") << callsign;
                failedRemoteAircraft.setRendered(false);
            }
            else
            {
                CLogMessage(this).warning("Adding '%1' failed, but aircraft no longer in range, will be removed") << callsign;
            }

            const bool wasPending = (m_addingInProgressAircraft.remove(cs) > 0);
            Q_UNUSED(wasPending);

            if (failedRemoteAircraft.hasCallsign() && !m_aircraftAddedFailed.containsCallsign(cs))
            {
                m_aircraftAddedFailed.push_back(failedRemoteAircraft);
                m_pendingToBeAddedAircraft.replaceOrAdd(failedRemoteAircraft); // try a second time
            }
            this->triggerAddNextPendingAircraft();
        }

        void CSimulatorXPlane::addNextPendingAircraft()
        {
            if (m_pendingToBeAddedAircraft.isEmpty()) { return; } // no more pending

            // housekeeping
            this->detectTimeoutAdding();

            // check if can add
            if (!this->canAddAircraft()) { return; }

            // next add cycle
            const CSimulatedAircraft newRemoteAircraft = m_pendingToBeAddedAircraft.front();
            m_pendingToBeAddedAircraft.pop_front();
            CLogMessage(this).info("Adding next pending aircraft '%1', pending %2, in progress %3") << newRemoteAircraft.getCallsignAsString() << m_pendingToBeAddedAircraft.size() << m_addingInProgressAircraft.size();
            this->physicallyAddRemoteAircraft(newRemoteAircraft);
        }

        void CSimulatorXPlane::triggerAddNextPendingAircraft()
        {
            QPointer<CSimulatorXPlane> myself(this);
            QTimer::singleShot(100, this, [ = ]
            {
                if (!myself) { return; }
                this->addNextPendingAircraft();
            });
        }

        int CSimulatorXPlane::detectTimeoutAdding()
        {
            if (m_addingInProgressAircraft.isEmpty()) { return 0; }
            const qint64 timeout = QDateTime::currentMSecsSinceEpoch() + TimeoutAdding;
            CCallsignSet timeoutCallsigns;
            const QList<CCallsign> addingCallsigns = m_addingInProgressAircraft.keys();
            for (const CCallsign &cs : addingCallsigns)
            {
                if (m_addingInProgressAircraft.value(cs) < timeout) { continue; }
                timeoutCallsigns.push_back(cs);
            }

            for (const CCallsign &cs : as_const(timeoutCallsigns))
            {
                m_addingInProgressAircraft.remove(cs);
                CLogMessage(this).warning("Adding for '%1' timed out") << cs.asString();
            }

            return timeoutCallsigns.size();
        }

        void CSimulatorXPlane::triggerRemoveAircraft(const CCallsign &callsign, qint64 deferMs)
        {
            QPointer<CSimulatorXPlane> myself(this);
            QTimer::singleShot(deferMs, this, [ = ]
            {
                if (!myself) { return; }
                this->physicallyRemoveRemoteAircraft(callsign);
            });
        }

        QPair<qint64, qint64> CSimulatorXPlane::minMaxTimestampsAddInProgress() const
        {
            static const QPair<qint64, qint64> empty(-1, -1);
            if (m_addingInProgressAircraft.isEmpty()) { return empty; }
            const QList<qint64> ts = m_addingInProgressAircraft.values();
            const auto mm = std::minmax_element(ts.constBegin(), ts.constEnd());
            return QPair<qint64, qint64>(*mm.first, *mm.second);
        }

        bool CSimulatorXPlane::canAddAircraft() const
        {
            if (m_addingInProgressAircraft.isEmpty()) { return true; }

            // check
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const QPair<qint64, qint64> tsMM = this->minMaxTimestampsAddInProgress();
            const qint64 deltaLatest = now - tsMM.second;
            const bool canAdd = (deltaLatest > TimeoutAdding);
            return canAdd;
        }

        ISimulator *CSimulatorXPlaneFactory::create(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider,
                IClientProvider *clientProvider)
        {
            return new CSimulatorXPlane(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
        }

        CSimulatorXPlaneListener::CSimulatorXPlaneListener(const CSimulatorPluginInfo &info): ISimulatorListener(info)
        {
            constexpr int QueryInterval = 5 * 1000; // 5 seconds
            m_timer.setInterval(QueryInterval);
            m_timer.setObjectName(this->objectName().append(":m_timer"));
            connect(&m_timer, &QTimer::timeout, this, &CSimulatorXPlaneListener::checkConnection);
        }

        void CSimulatorXPlaneListener::startImpl()
        {
            m_timer.start();
        }

        void CSimulatorXPlaneListener::stopImpl()
        {
            m_timer.stop();
        }

        void CSimulatorXPlaneListener::checkImpl()
        {
            if (!m_timer.isActive()) { return; }
            if (this->isShuttingDown()) { return; }

            m_timer.start(); // restart because we will check just now
            QPointer<CSimulatorXPlaneListener> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                checkConnection();
            });
        }

        void CSimulatorXPlaneListener::checkConnection()
        {
            if (this->isShuttingDown()) { return; }
            Q_ASSERT_X(!CThreadUtils::isCurrentThreadApplicationThread(), Q_FUNC_INFO, "Expect to run in background");

            QString dbusAddress = m_xswiftbusServerSetting.getThreadLocal();
            if (CDBusServer::isSessionOrSystemAddress(dbusAddress))
            {
                checkConnectionViaBus(dbusAddress);
            }
            else if (CDBusServer::isQtDBusAddress(dbusAddress))
            {
                checkConnectionViaPeer(dbusAddress);
            }
        }

        void CSimulatorXPlaneListener::checkConnectionViaBus(const QString &address)
        {
            m_conn = CSimulatorXPlane::connectionFromString(address);
            if (!m_conn.isConnected())
            {
                m_conn.disconnectFromBus(m_conn.name());
                return;
            }
            checkConnectionCommon();
            m_conn.disconnectFromBus(m_conn.name());
        }

        void CSimulatorXPlaneListener::checkConnectionViaPeer(const QString &address)
        {
            m_conn = QDBusConnection::connectToPeer(address, "xswiftbus");
            if (!m_conn.isConnected())
            {
                // This is required to cleanup the connection in QtDBus
                m_conn.disconnectFromPeer(m_conn.name());
                return;
            }
            checkConnectionCommon();
            m_conn.disconnectFromPeer(m_conn.name());
        }

        void CSimulatorXPlaneListener::checkConnectionCommon()
        {
            CXSwiftBusServiceProxy service(m_conn);
            CXSwiftBusTrafficProxy traffic(m_conn);
            CXSwiftBusWeatherProxy weather(m_conn);

            bool result = service.isValid() && traffic.isValid() && weather.isValid();
            if (! result) { return; }

            QString swiftVersion = BlackConfig::CBuildConfig::getVersionString();
            QString xswiftbusVersion = service.getVersionNumber();
            if (! swiftVersion.contains(xswiftbusVersion))
            {
                CLogMessage(this).error("You are using an incorrect version of XSwiftBus. Make sure to install %1 into X-Plane plugins!") << xswiftbusVersion;
                return;
            }

            if(! traffic.initialize())
            {
                CLogMessage(this).error("Connection to XSwiftBus successful, but could not initialize XSwiftBus. Check X-Plane Log.txt.");
                return;
            }

            MultiplayerAcquireInfo info = traffic.acquireMultiplayerPlanes();
            if (! info.hasAcquired)
            {
                CLogMessage(this).error("Connection to XSwiftBus successful, but could not acquire multiplayer planes. %1 has acquired them already. Disable %2 or remove it if not required and reload XSwiftBus.") << info.owner << info.owner;
                return;
            }

            emit simulatorStarted(getPluginInfo());
        }

        void CSimulatorXPlaneListener::serviceRegistered(const QString &serviceName)
        {
            if (serviceName == xswiftbusServiceName())
            {
                emit simulatorStarted(getPluginInfo());
            }
            m_conn.disconnectFromBus(m_conn.name());
        }

        void CSimulatorXPlaneListener::xSwiftBusServerSettingChanged()
        {
            this->stop();
            this->start();
        }
    } // namespace
} // namespace

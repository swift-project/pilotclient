/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorflightgear.h"
#include "qcompilerdetection.h"
#include "fgswiftbusserviceproxy.h"
#include "fgswiftbustrafficproxy.h"
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
#include "blackmisc/mixin/mixincompare.h"
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
#include <math.h>

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
    inline const QString &fgswiftbusServiceName()
    {
        static const QString name("org.swift-project.fgswiftbus");
        return name;
    }
}

namespace BlackSimPlugin
{
    namespace Flightgear
    {
        int FGSWIFTBUS_API_VERSION = -1;
        QList<int> incompatibleVersions = {1,2};
        CSimulatorFlightgear::CSimulatorFlightgear(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider,
                IClientProvider *clientProvider,
                QObject *parent) :
            CSimulatorPluginCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            m_watcher = new QDBusServiceWatcher(this);
            m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
            m_watcher->addWatchedService(fgswiftbusServiceName());
            m_watcher->setObjectName("QDBusServiceWatcher");
            connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this, &CSimulatorFlightgear::onDBusServiceUnregistered, Qt::QueuedConnection);

            m_fastTimer.setObjectName(this->objectName().append(":m_fastTimer"));
            m_slowTimer.setObjectName(this->objectName().append(":m_slowTimer"));
            m_pendingAddedTimer.setObjectName(this->objectName().append(":m_pendingAddedTimer"));
            connect(&m_fastTimer, &QTimer::timeout, this, &CSimulatorFlightgear::fastTimerTimeout);
            connect(&m_slowTimer, &QTimer::timeout, this, &CSimulatorFlightgear::slowTimerTimeout);
            connect(&m_pendingAddedTimer, &QTimer::timeout, this, &CSimulatorFlightgear::addNextPendingAircraft);
            m_fastTimer.start(100);
            m_slowTimer.start(1000);
            m_airportUpdater.start(60 * 1000);
            m_pendingAddedTimer.start(5000);

            this->setDefaultModel({ "FG c172p", CAircraftModel::TypeModelMatchingDefaultModel,
                                    "C172", CAircraftIcaoCode("C172", "L1P")});
            this->resetFlightgearData();
        }

        CSimulatorFlightgear::~CSimulatorFlightgear()
        {
            this->unload();
        }

        void CSimulatorFlightgear::unload()
        {
            if (!this->isConnected()) { return; }

            // will call disconnect from
            CSimulatorPluginCommon::unload();
            delete m_watcher;
            m_watcher = nullptr;
        }

        QString CSimulatorFlightgear::getStatisticsSimulatorSpecific() const
        {
            return QStringLiteral("Add-time: %1ms/%2ms").arg(m_statsAddCurrentTimeMs).arg(m_statsAddMaxTimeMs);
        }

        void CSimulatorFlightgear::resetAircraftStatistics()
        {
            m_statsAddMaxTimeMs = -1;
            m_statsAddCurrentTimeMs = -1;
        }

        CStatusMessageList CSimulatorFlightgear::getInterpolationMessages(const CCallsign &callsign) const
        {
            if (callsign.isEmpty() || !m_flightgearAircraftObjects.contains(callsign)) { return CStatusMessageList(); }
            const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, false);
            return m_flightgearAircraftObjects[callsign].getInterpolationMessages(setup.getInterpolatorMode());
        }

        bool CSimulatorFlightgear::testSendSituationAndParts(const CCallsign &callsign, const CAircraftSituation &situation, const CAircraftParts &parts)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }
            if (!m_trafficProxy) { return false; }
            if (!m_flightgearAircraftObjects.contains(callsign)) { return false; }

            int u = 0;
            if (!situation.isNull())
            {
                PlanesPositions planesPositions;
                planesPositions.push_back(situation);
                m_trafficProxy->setPlanesPositions(planesPositions);
                u++;
            }

            if (parts.isNull() && Flightgear::FGSWIFTBUS_API_VERSION >= 2)
            {
                PlanesSurfaces surfaces;
                surfaces.push_back(callsign, parts);
                m_trafficProxy->setPlanesSurfaces(surfaces);
                u++;
            }
            return u > 0;
        }

        void CSimulatorFlightgear::clearAllRemoteAircraftData()
        {
            m_aircraftAddedFailed.clear();
            CSimulatorPluginCommon::clearAllRemoteAircraftData();
        }

        // convert flightgear squawk mode to swift squawk mode
        CTransponder::TransponderMode xpdrMode(int transponderMode, bool ident)
        {
            if (ident) { return CTransponder::StateIdent; }
            if (transponderMode == 0 || transponderMode == 1 || transponderMode == 2) { return CTransponder::StateStandby; }
            return CTransponder::ModeC;
        }

        // convert swift squawk mode to flightgear squawk mode
        int xpdrMode(CTransponder::TransponderMode mode)
        {
            return mode == CTransponder::StateStandby ? 1 : 4;
        }

        void CSimulatorFlightgear::fastTimerTimeout()
        {
            if (!this->isShuttingDownOrDisconnected())
            {
                m_serviceProxy->getOwnAircraftSituationData(&m_flightgearData);
                m_serviceProxy->getOwnAircraftVelocityData(&m_flightgearData);
                m_serviceProxy->getCom1ActiveKhzAsync(&m_flightgearData.com1ActiveKhz);
                m_serviceProxy->getCom1StandbyKhzAsync(&m_flightgearData.com1StandbyKhz);
                m_serviceProxy->getCom2ActiveKhzAsync(&m_flightgearData.com2ActiveKhz);
                m_serviceProxy->getCom2StandbyKhzAsync(&m_flightgearData.com2StandbyKhz);
                m_serviceProxy->getTransponderCodeAsync(&m_flightgearData.xpdrCode);
                m_serviceProxy->getTransponderModeAsync(&m_flightgearData.xpdrMode);
                m_serviceProxy->getTransponderIdentAsync(&m_flightgearData.xpdrIdent);
                m_serviceProxy->getAllWheelsOnGroundAsync(&m_flightgearData.onGroundAll);
                m_serviceProxy->getGroundElevationAsync(&m_flightgearData.groundElevation);

                CAircraftSituation situation;
                situation.setPosition({ m_flightgearData.latitudeDeg, m_flightgearData.longitudeDeg, 0 });
                situation.setAltitude({ m_flightgearData.altitudeFt, CAltitude::MeanSeaLevel, CLengthUnit::ft() });
                situation.setPressureAltitude({m_flightgearData.pressureAltitudeFt, CAltitude::MeanSeaLevel, CAltitude::PressureAltitude, CLengthUnit::ft()});
                situation.setHeading({ m_flightgearData.trueHeadingDeg, CHeading::True, CAngleUnit::deg() });
                situation.setPitch({ m_flightgearData.pitchDeg, CAngleUnit::deg() });
                situation.setBank({ m_flightgearData.rollDeg, CAngleUnit::deg() });
                situation.setGroundSpeed({ m_flightgearData.groundspeedKts, CSpeedUnit::kts() });
                situation.setGroundElevation(CAltitude(m_flightgearData.groundElevation, CAltitude::MeanSeaLevel, CLengthUnit::m()), CAircraftSituation::FromProvider);
                situation.setVelocity({ m_flightgearData.velocityXMs, m_flightgearData.velocityYMs, m_flightgearData.velocityZMs,
                    CSpeedUnit::m_s(), m_flightgearData.pitchRateRadPerSec, m_flightgearData.rollRateRadPerSec, m_flightgearData.yawRateRadPerSec,
                    CAngleUnit::rad(), CTimeUnit::s()});

                // Updates
                // Do not update ICAO codes, as this overrides reverse lookups
                // updateOwnIcaoCodes(m_flightgearData.aircraftIcaoCode, CAirlineIcaoCode());
                this->updateOwnSituationAndGroundElevation(situation);

                // defaults
                CSimulatedAircraft myAircraft(getOwnAircraft());
                CComSystem com1(myAircraft.getCom1System()); // set defaults
                CComSystem com2(myAircraft.getCom2System());
                CTransponder transponder(myAircraft.getTransponder());

                // updates
                com1.setFrequencyActive(CFrequency(m_flightgearData.com1ActiveKhz, CFrequencyUnit::kHz()));
                com1.setFrequencyStandby(CFrequency(m_flightgearData.com1StandbyKhz, CFrequencyUnit::kHz()));
                const bool changedCom1 = myAircraft.getCom1System() != com1;

                com2.setFrequencyActive(CFrequency(m_flightgearData.com2ActiveKhz, CFrequencyUnit::kHz()));
                com2.setFrequencyStandby(CFrequency(m_flightgearData.com2StandbyKhz, CFrequencyUnit::kHz()));
                const bool changedCom2 = myAircraft.getCom2System() != com2;

                transponder = CTransponder::getStandardTransponder(m_flightgearData.xpdrCode, xpdrMode(m_flightgearData.xpdrMode, m_flightgearData.xpdrIdent));
                const bool changedXpr = (myAircraft.getTransponder() != transponder);

                if (changedCom1 || changedCom2 || changedXpr)
                {
                    this->updateCockpit(com1, com2, transponder, identifier());
                }
            }
        }

        void CSimulatorFlightgear::slowTimerTimeout()
        {
            if (!this->isShuttingDownOrDisconnected())
            {
                m_serviceProxy->getAircraftModelPathAsync(&m_flightgearData.aircraftModelPath); // this is NOT the model string
                m_serviceProxy->getAircraftIcaoCodeAsync(&m_flightgearData.aircraftIcaoCode);
                m_serviceProxy->getBeaconLightsOnAsync(&m_flightgearData.beaconLightsOn);
                m_serviceProxy->getLandingLightsOnAsync(&m_flightgearData.landingLightsOn);
                m_serviceProxy->getNavLightsOnAsync(&m_flightgearData.navLightsOn);
                m_serviceProxy->getStrobeLightsOnAsync(&m_flightgearData.strobeLightsOn);
                m_serviceProxy->getTaxiLightsOnAsync(&m_flightgearData.taxiLightsOn);
                m_serviceProxy->getFlapsDeployRatioAsync(&m_flightgearData.flapsReployRatio);
                m_serviceProxy->getGearDeployRatioAsync(&m_flightgearData.gearReployRatio);
                m_serviceProxy->getEngineN1PercentageAsync(&m_flightgearData.enginesN1Percentage);
                m_serviceProxy->getSpeedBrakeRatioAsync(&m_flightgearData.speedBrakeRatio);

                CAircraftEngineList engines;
                for (int engineNumber = 0; engineNumber < m_flightgearData.enginesN1Percentage.size(); ++engineNumber)
                {
                    // Engine number start counting at 1
                    // We consider the engine running when N1 is bigger than 5 %
                    CAircraftEngine engine {engineNumber + 1, m_flightgearData.enginesN1Percentage.at(engineNumber) > 5.0};
                    engines.push_back(engine);
                }

                CAircraftParts parts { {
                        m_flightgearData.strobeLightsOn, m_flightgearData.landingLightsOn, m_flightgearData.taxiLightsOn,
                        m_flightgearData.beaconLightsOn, m_flightgearData.navLightsOn, false
                    },
                    m_flightgearData.gearReployRatio > 0, static_cast<int>(m_flightgearData.flapsReployRatio * 100),
                    m_flightgearData.speedBrakeRatio > 0.5, engines, m_flightgearData.onGroundAll
                };

                this->updateOwnParts(parts);
                this->requestRemoteAircraftDataFromFlightgear();

                CCallsignSet invalid;
                for (CFlightgearMPAircraft &flightgearAircraft : m_flightgearAircraftObjects)
                {
                    // Update remote aircraft to have the latest transponder modes, codes etc.
                    const CCallsign cs = flightgearAircraft.getCallsign();
                    const CSimulatedAircraft simulatedAircraft = this->getAircraftInRangeForCallsign(cs);
                    if (!simulatedAircraft.hasCallsign())
                    {
                        if (!cs.isEmpty()) { invalid.insert(cs); }
                        continue;
                    }
                    flightgearAircraft.setSimulatedAircraft(simulatedAircraft);
                }

                int i = 0;

                for (const CCallsign &cs : invalid)
                {
                    this->triggerRemoveAircraft(cs, ++i * 100);
                }
            }
        }

        bool CSimulatorFlightgear::isConnected() const
        {
            return m_serviceProxy && m_trafficProxy;
        }

        bool CSimulatorFlightgear::connectTo()
        {
            if (isConnected()) { return true; }
            QString dbusAddress = m_fgswiftbusServerSetting.getThreadLocal();

            if (CDBusServer::isSessionOrSystemAddress(dbusAddress))
            {
                m_dBusConnection = QDBusConnection::sessionBus();
                m_dbusMode = Session;
            }
            else if (CDBusServer::isQtDBusAddress(dbusAddress))
            {
                m_dBusConnection = QDBusConnection::connectToPeer(dbusAddress, "fgswiftbus");
                if (! m_dBusConnection.isConnected()) { return false; }
                m_dbusMode = P2P;
            }

            m_serviceProxy = new CFGSwiftBusServiceProxy(m_dBusConnection, this);
            m_trafficProxy = new CFGSwiftBusTrafficProxy(m_dBusConnection, this);

            const bool s = m_dBusConnection.connect(QString(), DBUS_PATH_LOCAL, DBUS_INTERFACE_LOCAL,
                                                    "Disconnected", this, SLOT(onDBusServiceUnregistered()));
            Q_ASSERT(s);
            if (!m_serviceProxy->isValid() || !m_trafficProxy->isValid())
            {
                this->disconnectFrom();
                return false;
            }

            emitOwnAircraftModelChanged(m_serviceProxy->getAircraftModelPath(), m_serviceProxy->getAircraftModelFilename(), m_serviceProxy->getAircraftLivery(),
                                        m_serviceProxy->getAircraftIcaoCode(), m_serviceProxy->getAircraftModelString(), m_serviceProxy->getAircraftName(), m_serviceProxy->getAircraftDescription());
            setSimulatorDetails("Flightgear", {}, "");
            connect(m_serviceProxy, &CFGSwiftBusServiceProxy::aircraftModelChanged, this, &CSimulatorFlightgear::emitOwnAircraftModelChanged);
            connect(m_serviceProxy, &CFGSwiftBusServiceProxy::airportsInRangeUpdated, this, &CSimulatorFlightgear::setAirportsInRange);
            connect(m_trafficProxy, &CFGSwiftBusTrafficProxy::simFrame, this, &CSimulatorFlightgear::updateRemoteAircraft);
            connect(m_trafficProxy, &CFGSwiftBusTrafficProxy::remoteAircraftAdded, this, &CSimulatorFlightgear::onRemoteAircraftAdded);
            connect(m_trafficProxy, &CFGSwiftBusTrafficProxy::remoteAircraftAddingFailed, this, &CSimulatorFlightgear::onRemoteAircraftAddingFailed);
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            m_trafficProxy->removeAllPlanes();
            this->emitSimulatorCombinedStatus();

            this->initSimulatorInternals();
            return true;
        }

        bool CSimulatorFlightgear::disconnectFrom()
        {
            if (!this->isConnected()) { return true; } // avoid emit if already disconnected
            this->disconnectFromDBus();
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            this->emitSimulatorCombinedStatus();
            return true;
        }

        void CSimulatorFlightgear::onDBusServiceUnregistered()
        {
            if (!m_serviceProxy) { return; }
            CLogMessage(this).info(u"FG DBus service unregistered");

            if (m_dbusMode == P2P) { m_dBusConnection.disconnectFromPeer(m_dBusConnection.name()); }
            m_dBusConnection = QDBusConnection { "default" };
            if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
            delete m_serviceProxy;
            delete m_trafficProxy;
            m_serviceProxy = nullptr;
            m_trafficProxy = nullptr;
            this->emitSimulatorCombinedStatus();
        }

        void CSimulatorFlightgear::emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery,
                const QString &icao, const QString &modelString, const QString &name, const QString &description)
        {
            CAircraftModel model(modelString, CAircraftModel::TypeOwnSimulatorModel, CSimulatorInfo::XPLANE, name, description, icao);
            if (!livery.isEmpty()) { model.setModelString(model.getModelString()); }
            model.setFileName(path + "/" + filename);

            this->reverseLookupAndUpdateOwnAircraftModel(model);
        }

        void CSimulatorFlightgear::displayStatusMessage(const CStatusMessage &message) const
        {
            // No assert here as status message may come because of network problems
            if (this->isShuttingDownOrDisconnected()) { return; }

            // avoid infinite recursion in case this function is called due to a message caused by this very function
            static bool isInFunction = false;
            if (isInFunction) { return; }
            isInFunction = true;

            m_serviceProxy->addTextMessage("swift: " + message.getMessage());
            isInFunction = false;
        }

        void CSimulatorFlightgear::displayTextMessage(const Network::CTextMessage &message) const
        {
            if (this->isShuttingDownOrDisconnected()) { return; }
            m_serviceProxy->addTextMessage(message.getSenderCallsign().toQString() + ": " + message.getMessage());
        }

        void CSimulatorFlightgear::setAirportsInRange(const QStringList &icaos, const QStringList &names, const CSequence<double> &lats, const CSequence<double> &lons, const CSequence<double> &alts)
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

        CAirportList CSimulatorFlightgear::getAirportsInRange(bool recalculateDistance) const
        {
            if (!recalculateDistance) { return m_airportsInRange; }
            CAirportList airports(m_airportsInRange);
            airports.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition());
            return airports;
        }

        bool CSimulatorFlightgear::setTimeSynchronization(bool enable, const PhysicalQuantities::CTime &offset)
        {
            Q_UNUSED(offset)
            if (enable)
            {
                CLogMessage(this).info(u"Flightgear provides real time synchronization, use this one");
            }
            return false;
        }

        bool CSimulatorFlightgear::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            return m_flightgearAircraftObjects.contains(callsign);
        }

        bool CSimulatorFlightgear::updateOwnSimulatorCockpit(const Simulation::CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (originator == this->identifier())     { return false; }
            if (this->isShuttingDownOrDisconnected()) { return false; }

            auto com1 = CComSystem::getCom1System({ m_flightgearData.com1ActiveKhz, CFrequencyUnit::kHz() }, { m_flightgearData.com1StandbyKhz, CFrequencyUnit::kHz() });
            auto com2 = CComSystem::getCom2System({ m_flightgearData.com2ActiveKhz, CFrequencyUnit::kHz() }, { m_flightgearData.com2StandbyKhz, CFrequencyUnit::kHz() });
            auto xpdr = CTransponder::getStandardTransponder(m_flightgearData.xpdrCode, xpdrMode(m_flightgearData.xpdrMode, m_flightgearData.xpdrIdent));
            if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
            {
                m_flightgearData.com1ActiveKhz  = aircraft.getCom1System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
                m_flightgearData.com1StandbyKhz = aircraft.getCom1System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
                m_flightgearData.com2ActiveKhz  = aircraft.getCom2System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
                m_flightgearData.com2StandbyKhz = aircraft.getCom2System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
                m_flightgearData.xpdrCode = aircraft.getTransponderCode();
                m_flightgearData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
                m_serviceProxy->setCom1ActiveKhz(m_flightgearData.com1ActiveKhz);
                m_serviceProxy->setCom1StandbyKhz(m_flightgearData.com1StandbyKhz);
                m_serviceProxy->setCom2ActiveKhz(m_flightgearData.com2ActiveKhz);
                m_serviceProxy->setCom2StandbyKhz(m_flightgearData.com2StandbyKhz);
                m_serviceProxy->setTransponderCode(m_flightgearData.xpdrCode);
                m_serviceProxy->setTransponderMode(m_flightgearData.xpdrMode);

                m_serviceProxy->cancelAllPendingAsyncCalls(); // in case there is already a reply with some old data incoming
                return true;
            }
            return false;
        }

        bool CSimulatorFlightgear::updateOwnSimulatorSelcal(const CSelcal &selcal, const CIdentifier &originator)
        {
            if (originator == this->identifier()) { return false; }
            if (this->isShuttingDownOrDisconnected()) { return false; }

            //! \fixme KB 8/2017 use SELCAL??
            Q_UNUSED(selcal)

            return false;
        }

        bool CSimulatorFlightgear::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }

            // entry checks
            Q_ASSERT_X(CThreadUtils::isInThisThread(this),  Q_FUNC_INFO, "thread");
            Q_ASSERT_X(!newRemoteAircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "empty callsign");
            Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

            // crosscheck if still a valid aircraft
            // it can happen that aircraft has been removed, timed out ...
            if (!this->isAircraftInRangeOrTestMode(newRemoteAircraft.getCallsign()))
            {
                // next cycle will be called by callbacks or timer
                CLogMessage(this).warning(u"Aircraft '%1' no longer in range, will not add") << newRemoteAircraft.getCallsign();
                return false;
            }

            if (this->canAddAircraft())
            {
                // no aircraft pending, add
                this->logAddingAircraftModel(newRemoteAircraft);
                const qint64 now = QDateTime::currentMSecsSinceEpoch();
                m_addingInProgressAircraft.insert(newRemoteAircraft.getCallsign(), now);
                const QString callsign = newRemoteAircraft.getCallsign().asString();
                CAircraftModel aircraftModel = newRemoteAircraft.getModel();
                if (aircraftModel.getCallsign() != newRemoteAircraft.getCallsign())
                {
                    CLogMessage(this).warning(u"Model for '%1' has no callsign, maybe using a default model") << callsign;
                    aircraftModel.setCallsign(callsign);
                }
                const QString livery = aircraftModel.getLivery().getCombinedCode(); //! \todo livery resolution for XP
                m_trafficProxy->addPlane(callsign, aircraftModel.getFileName(),
                                         newRemoteAircraft.getAircraftIcaoCode().getDesignator(),
                                         newRemoteAircraft.getAirlineIcaoCode().getDesignator(),
                                         livery);

                PlanesPositions pos;
                pos.push_back(newRemoteAircraft.getSituation());
                m_trafficProxy->setPlanesPositions(pos);
                if(Flightgear::FGSWIFTBUS_API_VERSION >= 2)
                {
                    PlanesSurfaces surfaces;
                    surfaces.push_back(newRemoteAircraft.getCallsign(), newRemoteAircraft.getParts());
                    m_trafficProxy->setPlanesSurfaces(surfaces);
                }

            }
            else
            {
                // add in queue
                m_pendingToBeAddedAircraft.replaceOrAdd(newRemoteAircraft);
            }
            return true;
        }

        bool CSimulatorFlightgear::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }

            // only remove from sim
            Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "wrong thread");
            if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

            // really remove from simulator
            if (!m_flightgearAircraftObjects.contains(callsign) && !m_pendingToBeAddedAircraft.containsCallsign(callsign) && !m_addingInProgressAircraft.contains(callsign))
            {
                // not existing aircraft
                return false;
            }

            // mark in provider
            const bool updated = this->updateAircraftRendered(callsign, false);
            if (updated)
            {
                if (m_flightgearAircraftObjects.contains(callsign))
                {
                    const CFlightgearMPAircraft &flightgearAircraft = m_flightgearAircraftObjects[callsign];
                    CSimulatedAircraft aircraft(flightgearAircraft.getAircraft());
                    aircraft.setRendered(false);
                    emit this->aircraftRenderingChanged(aircraft);
                }
                else if (m_pendingToBeAddedAircraft.containsCallsign(callsign))
                {
                    CSimulatedAircraft aircraft = m_pendingToBeAddedAircraft.findFirstByCallsign(callsign);
                    aircraft.setRendered(false);
                    emit this->aircraftRenderingChanged(aircraft);
                }
            }

            if (m_addingInProgressAircraft.contains(callsign))
            {
                // we are just about to add that aircraft
                QPointer<CSimulatorFlightgear> myself(this);
                QTimer::singleShot(TimeoutAdding, this, [ = ]
                {
                    if (!myself) { return; }
                    m_addingInProgressAircraft.remove(callsign); // remove as "in progress"
                    this->physicallyRemoveRemoteAircraft(callsign); // and remove from sim. if it was added in the mean time
                });
                return false;
            }

            m_trafficProxy->removePlane(callsign.asString());
            m_flightgearAircraftObjects.remove(callsign);
            m_pendingToBeAddedAircraft.removeByCallsign(callsign);

            // bye
            return CSimulatorPluginCommon::physicallyRemoveRemoteAircraft(callsign);
        }

        int CSimulatorFlightgear::physicallyRemoveAllRemoteAircraft()
        {
            if (!this->isConnected()) { return 0; }
            m_pendingToBeAddedAircraft.clear();
            m_addingInProgressAircraft.clear();
            return CSimulatorPluginCommon::physicallyRemoveAllRemoteAircraft();
        }

        CCallsignSet CSimulatorFlightgear::physicallyRenderedAircraft() const
        {
            return this->getAircraftInRange().findByRendered(true).getCallsigns(); // just a poor workaround
        }

        void CSimulatorFlightgear::updateRemoteAircraft()
        {
            Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "thread");

            const int remoteAircraftNo = this->getAircraftInRangeCount();
            if (remoteAircraftNo < 1) { return; }

            // values used for position and parts
            m_updateRemoteAircraftInProgress = true;
            const qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

            // interpolation for all remote aircraft
            PlanesPositions planesPositions;
            PlanesSurfaces planesSurfaces;
            PlanesTransponders planesTransponders;

            int aircraftNumber = 0;
            const bool updateAllAircraft = this->isUpdateAllRemoteAircraft(currentTimestamp);
            const CCallsignSet callsignsInRange = this->getAircraftInRangeCallsigns();
            for (const CFlightgearMPAircraft &flightgearAircraft : m_flightgearAircraftObjects)
            {
                const CCallsign callsign(flightgearAircraft.getCallsign());
                const bool hasCallsign = !callsign.isEmpty();
                if (!hasCallsign)
                {
                    // does not make sense to continue here
                    BLACK_VERIFY_X(false, Q_FUNC_INFO, "missing callsign");
                    continue;
                }

                // skip no longer in range
                if (!callsignsInRange.contains(callsign)) { continue; }

                planesTransponders.callsigns.push_back(callsign.asString());
                planesTransponders.codes.push_back(flightgearAircraft.getAircraft().getTransponderCode());
                CTransponder::TransponderMode transponderMode = flightgearAircraft.getAircraft().getTransponderMode();
                planesTransponders.idents.push_back(transponderMode == CTransponder::StateIdent);
                planesTransponders.modeCs.push_back(transponderMode == CTransponder::ModeC);

                // setup
                const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupConsolidated(callsign, updateAllAircraft);

                // interpolated situation/parts
                const CInterpolationResult result = flightgearAircraft.getInterpolation(currentTimestamp, setup, aircraftNumber++);
                if (result.getInterpolationStatus().hasValidSituation())
                {
                    const CAircraftSituation interpolatedSituation(result);

                    // update situation
                    if (updateAllAircraft || !this->isEqualLastSent(interpolatedSituation))
                    {
                        this->rememberLastSent(interpolatedSituation);
                        planesPositions.push_back(interpolatedSituation);
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
                        planesSurfaces.push_back(flightgearAircraft.getCallsign(), parts);
                    }
                }

            } // all callsigns

            if (!planesTransponders.isEmpty() && Flightgear::FGSWIFTBUS_API_VERSION >= 2)
            {
                m_trafficProxy->setPlanesTransponders(planesTransponders);
            }

            if (!planesPositions.isEmpty())
            {
                if (CBuildConfig::isLocalDeveloperDebugBuild())
                {
                    BLACK_VERIFY_X(planesPositions.hasSameSizes(), Q_FUNC_INFO, "Mismatching sizes");
                }
                m_trafficProxy->setPlanesPositions(planesPositions);
            }

            if (! planesSurfaces.isEmpty() && Flightgear::FGSWIFTBUS_API_VERSION >= 2)
            {
                m_trafficProxy->setPlanesSurfaces(planesSurfaces);
            }

            // stats
            this->finishUpdateRemoteAircraftAndSetStatistics(currentTimestamp);
        }

        void CSimulatorFlightgear::requestRemoteAircraftDataFromFlightgear()
        {
            if (this->isShuttingDownOrDisconnected()) { return; }

            // It is not required to request all elevations and CGs, but only for aircraft "near ground relevant"
            // - we could use the elevation cache and CG cache to decide if we need to request
            // - if an aircraft is on ground but not moving, we do not need to request elevation if we already have it (it will not change
            CCallsignSet callsigns = m_flightgearAircraftObjects.getAllCallsigns();
            const CCallsignSet remove = this->getLastSentCanLikelySkipNearGroundInterpolation().getCallsigns();
            callsigns.remove(remove);
            if (!callsigns.isEmpty()) { this->requestRemoteAircraftDataFromFlightgear(callsigns); }
        }

        void CSimulatorFlightgear::requestRemoteAircraftDataFromFlightgear(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return; }
            if (!m_trafficProxy || this->isShuttingDown()) { return; }
            const QStringList csStrings = callsigns.getCallsignStrings();
            QPointer<CSimulatorFlightgear> myself(this);
            m_trafficProxy->getRemoteAircraftData(csStrings, [ = ](const QStringList & callsigns, const QDoubleList & latitudesDeg, const QDoubleList & longitudesDeg, const QDoubleList & elevationsMeters, const QDoubleList & verticalOffsetsMeters)
            {
                if (!myself) { return; }
                this->updateRemoteAircraftFromSimulator(callsigns, latitudesDeg, longitudesDeg, elevationsMeters, verticalOffsetsMeters);
            });
        }

        void CSimulatorFlightgear::triggerRequestRemoteAircraftDataFromFlightgear(const CCallsignSet &callsigns)
        {
            if (callsigns.isEmpty()) { return; }
            QPointer<CSimulatorFlightgear> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->requestRemoteAircraftDataFromFlightgear(callsigns);
            });
        }

        void CSimulatorFlightgear::updateRemoteAircraftFromSimulator(
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
                const bool emptyCs = callsigns[i].isEmpty();
                BLACK_VERIFY_X(!emptyCs, Q_FUNC_INFO, "Need callsign");
                if (emptyCs) { continue; }
                const CCallsign cs(callsigns[i]);
                if (!m_flightgearAircraftObjects.contains(cs)) { continue; }
                const CFlightgearMPAircraft fgAircraft = m_flightgearAircraftObjects[cs];
                BLACK_VERIFY_X(fgAircraft.hasCallsign(), Q_FUNC_INFO, "Need callsign");
                if (!fgAircraft.hasCallsign()) { continue; }

                CElevationPlane elevation = CElevationPlane::null();
                if (!std::isnan(elevationsMeters[i]))
                {
                    const CAltitude elevationAlt = CAltitude(elevationsMeters[i], CLengthUnit::m(), CLengthUnit::ft());
                    elevation = CElevationPlane(CLatitude(latitudesDeg[i], CAngleUnit::deg()), CLongitude(longitudesDeg[i], CAngleUnit::deg()), elevationAlt, CElevationPlane::singlePointRadius());
                }

                const double cgValue = verticalOffsetsMeters[i]; // XP offset is swift CG
                const CLength cg = std::isnan(cgValue) ?
                                   CLength::null() :
                                   CLength(cgValue, CLengthUnit::m(), CLengthUnit::ft());

                // if we knew "on ground" here we could set it as parameter of rememberElevationAndSimulatorCG
                this->rememberElevationAndSimulatorCG(cs, fgAircraft.getAircraftModel(), false, elevation, cg);

                // loopback
                if (logCallsigns.contains(cs))
                {
                    this->addLoopbackSituation(cs, elevation, cg);
                }
            }
        }

        void CSimulatorFlightgear::disconnectFromDBus()
        {
            if (m_dBusConnection.isConnected())
            {
                if (m_trafficProxy) { m_trafficProxy->cleanup(); }

                if (m_dbusMode == P2P) { QDBusConnection::disconnectFromPeer(m_dBusConnection.name()); }
                else { QDBusConnection::disconnectFromBus(m_dBusConnection.name()); }
            }
            m_dBusConnection = QDBusConnection { "default" };
        }

        void CSimulatorFlightgear::onRemoteAircraftAdded(const QString &callsign)
        {
            BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
            if (callsign.isEmpty()) { return; }
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
                CLogMessage(this).warning(u"Aircraft '%1' no longer in range, will be removed") << callsign;
                this->triggerRemoveAircraft(cs, TimeoutAdding);
                return;
            }

            CLogMessage(this).info(u"Added aircraft '%1'") << callsign;
            if (!wasPending)
            {
                // timeout?
                // slow adding?
                CLogMessage(this).warning(u"Added callsign '%1' was not in progress anymore. Timeout?") << callsign;
            }

            const bool rendered = true;
            addedRemoteAircraft.setRendered(rendered);
            this->updateAircraftRendered(cs, rendered);
            this->triggerRequestRemoteAircraftDataFromFlightgear(cs);
            this->triggerAddNextPendingAircraft();

            Q_ASSERT_X(addedRemoteAircraft.hasCallsign(), Q_FUNC_INFO, "No callsign");
            Q_ASSERT_X(addedRemoteAircraft.getCallsign() == cs, Q_FUNC_INFO, "No callsign");
            m_flightgearAircraftObjects.insert(cs, CFlightgearMPAircraft(addedRemoteAircraft, this, &m_interpolationLogger));
            emit this->aircraftRenderingChanged(addedRemoteAircraft);
        }

        bool CSimulatorFlightgear::requestElevation(const BlackMisc::Geo::ICoordinateGeodetic &reference, const BlackMisc::Aviation::CCallsign &callsign)
        {
            if (this->isShuttingDownOrDisconnected()) { return false; }
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
            auto callback = std::bind(&CSimulatorFlightgear::callbackReceivedRequestedElevation, this, _1, _2, false);

            // Request
            m_trafficProxy->getElevationAtPosition(callsign,
                                                   pos.latitude().value(CAngleUnit::deg()),
                                                   pos.longitude().value(CAngleUnit::deg()),
                                                   pos.geodeticHeight().value(CLengthUnit::m()),
                                                   callback);
            emit this->requestedElevation(callsign);
            return true;
        }

        void CSimulatorFlightgear::onRemoteAircraftAddingFailed(const QString &callsign)
        {
            BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
            if (callsign.isEmpty()) { return; }
            const CCallsign cs(callsign);
            CSimulatedAircraft failedRemoteAircraft = this->getAircraftInRangeForCallsign(cs);

            if (failedRemoteAircraft.hasCallsign())
            {
                CLogMessage(this).warning(u"Adding aircraft failed: '%1'") << callsign;
                failedRemoteAircraft.setRendered(false);
            }
            else
            {
                CLogMessage(this).warning(u"Adding '%1' failed, but aircraft no longer in range, will be removed") << callsign;
            }

            const bool wasPending = (m_addingInProgressAircraft.remove(cs) > 0);
            Q_UNUSED(wasPending)

            if (failedRemoteAircraft.hasCallsign() && !m_aircraftAddedFailed.containsCallsign(cs))
            {
                m_aircraftAddedFailed.push_back(failedRemoteAircraft);
                m_pendingToBeAddedAircraft.replaceOrAdd(failedRemoteAircraft); // try a second time
            }
            this->triggerAddNextPendingAircraft();
        }

        void CSimulatorFlightgear::addNextPendingAircraft()
        {
            if (m_pendingToBeAddedAircraft.isEmpty()) { return; } // no more pending

            // housekeeping
            this->detectTimeoutAdding();

            // check if can add
            if (!this->canAddAircraft()) { return; }

            // next add cycle
            const CSimulatedAircraft newRemoteAircraft = m_pendingToBeAddedAircraft.front();
            m_pendingToBeAddedAircraft.pop_front();
            CLogMessage(this).info(u"Adding next pending aircraft '%1', pending %2, in progress %3") << newRemoteAircraft.getCallsignAsString() << m_pendingToBeAddedAircraft.size() << m_addingInProgressAircraft.size();
            this->physicallyAddRemoteAircraft(newRemoteAircraft);
        }

        void CSimulatorFlightgear::triggerAddNextPendingAircraft()
        {
            QPointer<CSimulatorFlightgear> myself(this);
            QTimer::singleShot(100, this, [ = ]
            {
                if (!myself) { return; }
                this->addNextPendingAircraft();
            });
        }

        int CSimulatorFlightgear::detectTimeoutAdding()
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
                CLogMessage(this).warning(u"Adding for '%1' timed out") << cs.asString();
            }

            return timeoutCallsigns.size();
        }

        void CSimulatorFlightgear::triggerRemoveAircraft(const CCallsign &callsign, qint64 deferMs)
        {
            QPointer<CSimulatorFlightgear> myself(this);
            QTimer::singleShot(deferMs, this, [ = ]
            {
                if (!myself) { return; }
                this->physicallyRemoveRemoteAircraft(callsign);
            });
        }

        QPair<qint64, qint64> CSimulatorFlightgear::minMaxTimestampsAddInProgress() const
        {
            static const QPair<qint64, qint64> empty(-1, -1);
            if (m_addingInProgressAircraft.isEmpty()) { return empty; }
            const QList<qint64> ts = m_addingInProgressAircraft.values();
            const auto mm = std::minmax_element(ts.constBegin(), ts.constEnd());
            return QPair<qint64, qint64>(*mm.first, *mm.second);
        }

        bool CSimulatorFlightgear::canAddAircraft() const
        {
            if (this->getModelSet().isEmpty()) { return false; }
            if (m_addingInProgressAircraft.isEmpty()) { return true; }

            // check
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const QPair<qint64, qint64> tsMM = this->minMaxTimestampsAddInProgress();
            const qint64 deltaLatest = now - tsMM.second;
            const bool canAdd = (deltaLatest > TimeoutAdding);
            return canAdd;
        }

        ISimulator *CSimulatorFlightgearFactory::create(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider,
                IClientProvider *clientProvider)
        {
            return new CSimulatorFlightgear(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, this);
        }

        CSimulatorFlightgearListener::CSimulatorFlightgearListener(const CSimulatorPluginInfo &info): ISimulatorListener(info)
        {
            constexpr int QueryInterval = 5 * 1000; // 5 seconds
            m_timer.setInterval(QueryInterval);
            m_timer.setObjectName(this->objectName().append(":m_timer"));
            connect(&m_timer, &QTimer::timeout, this, &CSimulatorFlightgearListener::checkConnection);
        }

        void CSimulatorFlightgearListener::startImpl()
        {
            m_timer.start();
        }

        void CSimulatorFlightgearListener::stopImpl()
        {
            m_timer.stop();
        }

        void CSimulatorFlightgearListener::checkImpl()
        {
            if (!m_timer.isActive()) { return; }
            if (this->isShuttingDown()) { return; }

            m_timer.start(); // restart because we will check just now
            QPointer<CSimulatorFlightgearListener> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                checkConnection();
            });
        }

        void CSimulatorFlightgearListener::checkConnection()
        {
            if (this->isShuttingDown()) { return; }
            Q_ASSERT_X(!CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Expect to run in background");

            QString dbusAddress = m_fgSswiftBusServerSetting.getThreadLocal();
            if (CDBusServer::isSessionOrSystemAddress(dbusAddress))
            {
                checkConnectionViaSessionBus();
            }
            else if (CDBusServer::isQtDBusAddress(dbusAddress))
            {
                checkConnectionViaPeer(dbusAddress);
            }
        }

        void CSimulatorFlightgearListener::checkConnectionViaSessionBus()
        {
            m_conn = QDBusConnection::sessionBus();
            if (!m_conn.isConnected())
            {
                m_conn.disconnectFromBus(m_conn.name());
                return;
            }
            checkConnectionCommon();
            m_conn.disconnectFromBus(m_conn.name());
        }

        void CSimulatorFlightgearListener::checkConnectionViaPeer(const QString &address)
        {
            m_conn = QDBusConnection::connectToPeer(address, "fgswiftbus");
            if (!m_conn.isConnected())
            {
                // This is required to cleanup the connection in QtDBus
                m_conn.disconnectFromPeer(m_conn.name());
                return;
            }
            checkConnectionCommon();
            m_conn.disconnectFromPeer(m_conn.name());
        }

        void CSimulatorFlightgearListener::checkConnectionCommon()
        {
            CFGSwiftBusServiceProxy service(m_conn);
            CFGSwiftBusTrafficProxy traffic(m_conn);

            bool result = service.isValid() && traffic.isValid();
            if (! result) { return; }

            Flightgear::FGSWIFTBUS_API_VERSION = service.getVersionNumber();

            if (Flightgear::incompatibleVersions.contains(Flightgear::FGSWIFTBUS_API_VERSION))
            {
                CLogMessage(this).error(u"This version of swift is not compatible with this Flightgear version. For further information check http://wiki.flightgear.org/Swift.");
                return;
            }
            if (!traffic.initialize())
            {
                CLogMessage(this).error(u"Connection to FGSwiftBus successful, but could not initialize FGSwiftBus.");
                return;
            }

            const MultiplayerAcquireInfo info = traffic.acquireMultiplayerPlanes();
            if (!info.hasAcquired)
            {
                const QString owner = info.owner.trimmed().isEmpty() ? QStringLiteral("Some/this plugin/application") : info.owner.trimmed();
                CLogMessage(this).error(u"Connection to FGSwiftBus successful, but could not acquire multiplayer planes. '%1' has acquired them already. Disable '%2' or remove it if not required and reload FGSwiftBus.") << owner << owner.toLower();
                return;
            }

            emit simulatorStarted(getPluginInfo());
        }

        void CSimulatorFlightgearListener::serviceRegistered(const QString &serviceName)
        {
            if (serviceName == fgswiftbusServiceName())
            {
                emit simulatorStarted(getPluginInfo());
            }
            m_conn.disconnectFromBus(m_conn.name());
        }

        void CSimulatorFlightgearListener::fgSwiftBusServerSettingChanged()
        {
            this->stop();
            this->start();
        }
    } // namespace
} // namespace

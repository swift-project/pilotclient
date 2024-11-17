// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorxplane.h"

#include <cmath>

#include <QColor>
#include <QDBusServiceWatcher>
#include <QElapsedTimer>
#include <QPointer>
#include <QString>
#include <QTimer>
#include <QtGlobal>

#include "dbus/dbus.h"
#include "qcompilerdetection.h"

#include "config/buildconfig.h"
#include "core/aircraftmatcher.h"
#include "misc/aviation/aircraftengine.h"
#include "misc/aviation/aircraftenginelist.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircraftparts.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/altitude.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/heading.h"
#include "misc/aviation/livery.h"
#include "misc/aviation/transponder.h"
#include "misc/dbusserver.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/iterator.h"
#include "misc/logmessage.h"
#include "misc/mixin/mixincompare.h"
#include "misc/network/textmessage.h"
#include "misc/pq/angle.h"
#include "misc/pq/frequency.h"
#include "misc/pq/length.h"
#include "misc/pq/pressure.h"
#include "misc/pq/speed.h"
#include "misc/pq/temperature.h"
#include "misc/setbuilder.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/settings/xswiftbussettingsqtfree.inc"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/verify.h"
#include "misc/weather/cloudlayer.h"
#include "misc/weather/cloudlayerlist.h"
#include "misc/weather/windlayer.h"
#include "misc/weather/windlayerlist.h"
#include "xswiftbusserviceproxy.h"
#include "xswiftbustrafficproxy.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::settings;
using namespace swift::misc::weather;
using namespace swift::core;

namespace
{
    const QString &xswiftbusServiceName()
    {
        static const QString name("org.swift-project.xswiftbus");
        return name;
    }
    const QString &commitHash()
    {
        static const QString hash(XSWIFTBUS_COMMIT);
        return hash;
    }
} // namespace

namespace swift::simplugin::xplane
{
    CSimulatorXPlane::CSimulatorXPlane(const CSimulatorPluginInfo &info, IOwnAircraftProvider *ownAircraftProvider,
                                       IRemoteAircraftProvider *remoteAircraftProvider, IClientProvider *clientProvider,
                                       QObject *parent)
        : CSimulatorPluginCommon(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, parent)
    {
        m_watcher = new QDBusServiceWatcher(this);
        m_watcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
        m_watcher->addWatchedService(xswiftbusServiceName());
        m_watcher->setObjectName("QDBusServiceWatcher");
        connect(m_watcher, &QDBusServiceWatcher::serviceUnregistered, this,
                &CSimulatorXPlane::onDBusServiceUnregistered, Qt::QueuedConnection);

        m_fastTimer.setObjectName(this->objectName().append(":m_fastTimer"));
        m_slowTimer.setObjectName(this->objectName().append(":m_slowTimer"));
        m_pendingAddedTimer.setObjectName(this->objectName().append(":m_pendingAddedTimer"));
        connect(&m_fastTimer, &QTimer::timeout, this, &CSimulatorXPlane::fastTimerTimeout);
        connect(&m_slowTimer, &QTimer::timeout, this, &CSimulatorXPlane::slowTimerTimeout);
        connect(&m_pendingAddedTimer, &QTimer::timeout, this, &CSimulatorXPlane::addNextPendingAircraft);
        m_fastTimer.start(100);
        m_slowTimer.start(1000);
        m_pendingAddedTimer.start(5000);

        this->setDefaultModel({ "Jets A320_a A320_a_Austrian_Airlines A320_a_Austrian_Airlines",
                                CAircraftModel::TypeModelMatchingDefaultModel, "A320 AUA",
                                CAircraftIcaoCode("A320", "L2J") });
        this->resetXPlaneData();
    }

    CSimulatorXPlane::~CSimulatorXPlane() { this->unload(); }

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
        return QStringLiteral("Add-time: %1ms/%2ms").arg(m_statsAddCurrentTimeMs).arg(m_statsAddMaxTimeMs);
    }

    void CSimulatorXPlane::resetAircraftStatistics()
    {
        m_statsAddMaxTimeMs = -1;
        m_statsAddCurrentTimeMs = -1;
    }

    CStatusMessageList CSimulatorXPlane::getInterpolationMessages(const CCallsign &callsign) const
    {
        if (callsign.isEmpty() || !m_xplaneAircraftObjects.contains(callsign)) { return {}; }
        const CInterpolationAndRenderingSetupPerCallsign setup =
            this->getInterpolationSetupConsolidated(callsign, false);
        return m_xplaneAircraftObjects[callsign].getInterpolationMessages(setup.getInterpolatorMode());
    }

    bool CSimulatorXPlane::testSendSituationAndParts(const CCallsign &callsign, const CAircraftSituation &situation,
                                                     const CAircraftParts &parts)
    {
        if (this->isShuttingDownOrDisconnected()) { return false; }
        if (!m_trafficProxy) { return false; }
        if (!m_xplaneAircraftObjects.contains(callsign)) { return false; }

        int u = 0;
        if (!situation.isNull())
        {
            PlanesPositions planesPositions;
            planesPositions.push_back(situation);
            m_trafficProxy->setPlanesPositions(planesPositions);
            u++;
        }

        if (!parts.isNull())
        {
            PlanesSurfaces surfaces;
            surfaces.push_back(callsign, parts);
            m_trafficProxy->setPlanesSurfaces(surfaces);
            u++;
        }
        return u > 0;
    }

    bool CSimulatorXPlane::handleProbeValue(const CElevationPlane &plane, const CCallsign &callsign, bool waterFlag,
                                            const QString &hint, bool ignoreOutsideRange)
    {
        // XPlane specific checks for T778
        // https://discordapp.com/channels/539048679160676382/577213275184562176/696780159969132626
        if (!plane.hasMSLGeodeticHeight()) { return false; }
        if (isSuspiciousTerrainValue(plane))
        {
            // ignore values up to +- 1.0meters, those most likely mean no scenery
            const CLength distance = this->getDistanceToOwnAircraft(plane);
            if (ignoreOutsideRange && distance > maxTerrainRequestDistance()) { return false; }

            static const CLength threshold = maxTerrainRequestDistance() * 0.50;
            if (distance > threshold)
            {
                // we expect scenery to be loaded within threshold range
                // outside that range we assue a suspicous value "represents no scenery"
                // of course this can be wrong, but in that case we would geth those values
                // once we get inside range
                this->setMinTerrainProbeDistance(distance);
                CLogMessage(this).debug(
                    u"Suspicous XPlane probe [%1] value %2 for '%3' ignored, distance: %4 min.disance: %5 water: %6")
                    << hint << plane.getAltitude().valueRoundedAsString(CLengthUnit::m(), 4) << callsign.asString()
                    << distance.valueRoundedAsString(CLengthUnit::NM(), 2)
                    << m_minSuspicousTerrainProbe.valueRoundedAsString(CLengthUnit::NM(), 2) << boolToYesNo(waterFlag);
                return false;
            }
        }
        return true;
    }

    void CSimulatorXPlane::callbackReceivedRequestedElevation(const CElevationPlane &plane, const CCallsign &callsign,
                                                              bool isWater)
    {
        static const QString hint("probe callback");
        if (!this->handleProbeValue(plane, callsign, isWater, hint, false))
        {
            this->removePendingElevationRequest(callsign);
            return;
        }
        CSimulatorPluginCommon::callbackReceivedRequestedElevation(plane, callsign, isWater);
    }

    void CSimulatorXPlane::setFlightNetworkConnected(bool connected)
    {
        if (connected && !this->isShuttingDownOrDisconnected()) { m_serviceProxy->resetFrameTotals(); }
        m_serviceProxy->setFlightNetworkConnected(connected);
        CSimulatorPluginCommon::setFlightNetworkConnected(connected);
    }

    bool CSimulatorXPlane::isSuspiciousTerrainValue(const CElevationPlane &elevation)
    {
        if (!elevation.hasMSLGeodeticHeight()) { return true; }
        const double valueFt = qAbs(elevation.getAltitudeValue(CLengthUnit::ft()));
        return valueFt < 1.0;
    }

    const CLength &CSimulatorXPlane::maxTerrainRequestDistance()
    {
        static const CLength d(70.0, CLengthUnit::NM());
        return d;
    }

    void CSimulatorXPlane::clearAllRemoteAircraftData()
    {
        m_aircraftAddedFailed.clear();
        CSimulatorPluginCommon::clearAllRemoteAircraftData();
        m_minSuspicousTerrainProbe.setNull();
    }

    bool CSimulatorXPlane::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
    {
        if (this->isShuttingDownOrDisconnected()) { return false; }
        if (reference.isNull()) { return false; }

        // avoid requests for NON exising aircraft (based on LINUX crashes)
        if (callsign.isEmpty()) { return false; }
        if (!this->isAircraftInRange(callsign)) { return false; }

        const CLength d = this->getDistanceToOwnAircraft(reference);
        if (!d.isNull() && d > maxTerrainRequestDistance())
        {
            // no request, too far away
            return false;
        }

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
        auto callback = [this](auto &&plane, auto &&callsign, auto &&isWater) {
            callbackReceivedRequestedElevation(std::forward<decltype(plane)>(plane),
                                               std::forward<decltype(callsign)>(callsign),
                                               std::forward<decltype(isWater)>(isWater));
        };

        // Request
        m_trafficProxy->getElevationAtPosition(callsign, pos.latitude().value(CAngleUnit::deg()),
                                               pos.longitude().value(CAngleUnit::deg()),
                                               pos.geodeticHeight().value(CLengthUnit::m()), callback);
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
    int xpdrMode(CTransponder::TransponderMode mode) { return mode == CTransponder::StateStandby ? 1 : 2; }

    void CSimulatorXPlane::fastTimerTimeout()
    {
        if (!this->isShuttingDownOrDisconnected())
        {
            m_fastTimerCalls++;

            m_serviceProxy->getOwnAircraftSituationDataAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftVelocityDataAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftCom1DataAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftCom2DataAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftXpdrAsync(&m_xplaneData);
            m_serviceProxy->getAllWheelsOnGroundAsync(&m_xplaneData.onGroundAll);
            m_serviceProxy->getHeightAglMAsync(&m_xplaneData.heightAglM);
            m_serviceProxy->getPressureAltitudeFtAsync(&m_xplaneData.pressureAltitudeFt);

            CAircraftSituation situation;
            situation.setPosition({ m_xplaneData.latitudeDeg, m_xplaneData.longitudeDeg, 0 });
            const CAltitude altitude { m_xplaneData.altitudeM, CAltitude::MeanSeaLevel, CLengthUnit::m() };
            const CPressure seaLevelPressure({ m_xplaneData.seaLevelPressureInHg, CPressureUnit::inHg() });
            const CAltitude pressureAltitude(altitude.toPressureAltitude(seaLevelPressure));
            if (std::isnan(m_xplaneData.pressureAltitudeFt))
            {
                m_altitudeDelta = {};
                situation.setAltitude(altitude);
                situation.setPressureAltitude(pressureAltitude);
            }
            else
            {
                const CAltitude pressureAltitudeXP12(m_xplaneData.pressureAltitudeFt, CAltitude::MeanSeaLevel,
                                                     CAltitude::PressureAltitude, CLengthUnit::ft());
                m_altitudeDelta = pressureAltitude - pressureAltitudeXP12;

                situation.setAltitude({ altitude - m_altitudeDelta, CAltitude::MeanSeaLevel });
                situation.setPressureAltitude(pressureAltitudeXP12);
            }
            situation.setHeading({ m_xplaneData.trueHeadingDeg, CHeading::True, CAngleUnit::deg() });
            situation.setPitch({ m_xplaneData.pitchDeg, CAngleUnit::deg() });
            situation.setBank({ m_xplaneData.rollDeg, CAngleUnit::deg() });
            situation.setGroundSpeed({ m_xplaneData.groundspeedMs, CSpeedUnit::m_s() });
            const CAltitude elevation { m_xplaneData.altitudeM - m_xplaneData.heightAglM, CAltitude::MeanSeaLevel,
                                        CLengthUnit::m() };
            situation.setGroundElevation(elevation, CAircraftSituation::FromProvider);
            situation.setVelocity({ m_xplaneData.localXVelocityMs, m_xplaneData.localYVelocityMs,
                                    m_xplaneData.localZVelocityMs, CSpeedUnit::m_s(), m_xplaneData.pitchRadPerSec,
                                    m_xplaneData.rollRadPerSec, m_xplaneData.headingRadPerSec, CAngleUnit::rad(),
                                    CTimeUnit::s() });

            // Updates
            // Do not update ICAO codes, as this overrides reverse lookups
            // updateOwnIcaoCodes(m_xplaneData.aircraftIcaoCode, CAirlineIcaoCode());
            this->updateOwnSituationAndGroundElevation(situation);

            // defaults
            CSimulatedAircraft myAircraft(getOwnAircraft());
            CComSystem com1(myAircraft.getCom1System()); // set defaults
            CComSystem com2(myAircraft.getCom2System());
            CTransponder transponder(myAircraft.getTransponder());

            // updates
            com1.setFrequencyActive(CFrequency(m_xplaneData.com1ActiveKhz, CFrequencyUnit::kHz()));
            com1.setFrequencyStandby(CFrequency(m_xplaneData.com1StandbyKhz, CFrequencyUnit::kHz()));
            const int v1 = qRound(m_xplaneData.com1Volume * 100);
            com1.setVolumeReceive(v1);
            com1.setReceiveEnabled(m_xplaneData.isCom1Receiving);
            com1.setTransmitEnabled(m_xplaneData.isCom1Transmitting);
            const bool changedCom1 = myAircraft.getCom1System() != com1;

            com2.setFrequencyActive(CFrequency(m_xplaneData.com2ActiveKhz, CFrequencyUnit::kHz()));
            com2.setFrequencyStandby(CFrequency(m_xplaneData.com2StandbyKhz, CFrequencyUnit::kHz()));
            const int v2 = qRound(m_xplaneData.com2Volume * 100);
            com2.setVolumeReceive(v2);
            com2.setReceiveEnabled(m_xplaneData.isCom2Receiving);
            com2.setTransmitEnabled(m_xplaneData.isCom2Transmitting);
            const bool changedCom2 = myAircraft.getCom2System() != com2;

            transponder = CTransponder::getStandardTransponder(m_xplaneData.xpdrCode,
                                                               xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
            const bool changedXpr = (myAircraft.getTransponder() != transponder);

            if (changedCom1 || changedCom2 || changedXpr)
            {
                this->updateCockpit(com1, com2, transponder, identifier());
            }
        }
    }

    void CSimulatorXPlane::slowTimerTimeout()
    {
        if (!this->isShuttingDownOrDisconnected())
        {
            m_slowTimerCalls++;

            // own aircraft data
            m_serviceProxy->getOwnAircraftModelDataAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftLightsAsync(&m_xplaneData);
            m_serviceProxy->getOwnAircraftPartsAsync(&m_xplaneData);

            CAircraftEngineList engines;
            for (int engineNumber = 0; engineNumber < m_xplaneData.enginesN1Percentage.size(); ++engineNumber)
            {
                // Engine number start counting at 1
                // We consider the engine running when N1 is bigger than 5 %
                const CAircraftEngine engine { engineNumber + 1,
                                               m_xplaneData.enginesN1Percentage.at(engineNumber) > 5.0 };
                engines.push_back(engine);
            }

            const CAircraftLights lights(m_xplaneData.strobeLightsOn, m_xplaneData.landingLightsOn,
                                         m_xplaneData.taxiLightsOn, m_xplaneData.beaconLightsOn,
                                         m_xplaneData.navLightsOn, false);

            const CAircraftParts parts { lights,
                                         m_xplaneData.gearDeployRatio > 0,
                                         qRound(m_xplaneData.flapsDeployRatio * 100.0),
                                         m_xplaneData.speedBrakeRatio > 0.5,
                                         engines,
                                         m_xplaneData.onGroundAll };

            this->updateOwnParts(parts);

            CCallsignSet invalid;
            for (CXPlaneMPAircraft &xplaneAircraft : m_xplaneAircraftObjects)
            {
                // Update remote aircraft to have the latest transponder modes, codes etc.
                const CCallsign cs = xplaneAircraft.getCallsign();
                const CSimulatedAircraft simulatedAircraft = this->getAircraftInRangeForCallsign(cs);
                if (!simulatedAircraft.hasCallsign())
                {
                    // removed in provider
                    if (!cs.isEmpty()) { invalid.insert(cs); }
                    continue;
                }
                xplaneAircraft.setSimulatedAircraft(simulatedAircraft);
            }

            // remove the invalid ones
            int i = 0;
            if (this->isTestMode()) { invalid.clear(); } // skip this in test mode
            for (const CCallsign &cs : invalid) { this->triggerRemoveAircraft(cs, ++i * 100); }

            // KB: IMHO those data are pretty useless for XPlane
            // no need to request them all the times
            if ((m_slowTimerCalls % 3u) == 0u) { this->requestRemoteAircraftDataFromXPlane(); }

            // FPS
            // reading FPS resets average, so we only monitor over some time
            if ((m_slowTimerCalls % 5u) == 0u)
            {
                constexpr double warningMiles = 1;
                constexpr double disconnectMiles = 2;
                const double previousMiles = m_trackMilesShort;

                m_serviceProxy->getFrameStats(&m_averageFps, &m_simTimeRatio, &m_trackMilesShort, &m_minutesLate);

                if (previousMiles < disconnectMiles && m_trackMilesShort >= disconnectMiles)
                {
                    emit insufficientFrameRateDetected(true);
                }
                else if (previousMiles < warningMiles && m_trackMilesShort >= warningMiles)
                {
                    emit insufficientFrameRateDetected(false);
                }
            }
        }
    }

    bool CSimulatorXPlane::isConnected() const { return m_serviceProxy && m_trafficProxy; }

    bool CSimulatorXPlane::connectTo()
    {
        if (this->isConnected()) { return true; }
        const QString dBusServerAddress = m_xSwiftBusServerSettings.getThreadLocal().getDBusServerAddressQt();

        if (CDBusServer::isSessionOrSystemAddress(dBusServerAddress))
        {
            m_dBusConnection = QDBusConnection::sessionBus();
            m_dbusMode = Session;
        }
        else if (CDBusServer::isQtDBusAddress(dBusServerAddress))
        {
            m_dBusConnection = QDBusConnection::connectToPeer(dBusServerAddress, "xswiftbus");
            if (!m_dBusConnection.isConnected()) { return false; }
            m_dbusMode = P2P;
        }

        m_serviceProxy = new CXSwiftBusServiceProxy(m_dBusConnection, this);
        m_trafficProxy = new CXSwiftBusTrafficProxy(m_dBusConnection, this);

        // hook up disconnected slot of connection
        bool s = m_dBusConnection.connect(QString(), DBUS_PATH_LOCAL, DBUS_INTERFACE_LOCAL, "Disconnected", this,
                                          SLOT(onDBusServiceUnregistered()));
        Q_ASSERT(s);
        if (!m_serviceProxy->isValid() || !m_trafficProxy->isValid())
        {
            this->disconnectFrom();
            return false;
        }

        emitOwnAircraftModelChanged(m_serviceProxy->getAircraftModelPath(), m_serviceProxy->getAircraftModelFilename(),
                                    m_serviceProxy->getAircraftLivery(), m_serviceProxy->getAircraftIcaoCode(),
                                    m_serviceProxy->getAircraftModelString(), m_serviceProxy->getAircraftName(),
                                    m_serviceProxy->getAircraftDescription());
        QString xplaneVersion = QStringLiteral("%1.%2")
                                    .arg(m_serviceProxy->getXPlaneVersionMajor())
                                    .arg(m_serviceProxy->getXPlaneVersionMinor());
        setSimulatorDetails("X-Plane", {}, xplaneVersion);
        connect(m_serviceProxy, &CXSwiftBusServiceProxy::aircraftModelChanged, this,
                &CSimulatorXPlane::emitOwnAircraftModelChanged);
        connect(m_trafficProxy, &CXSwiftBusTrafficProxy::simFrame, this, &CSimulatorXPlane::updateRemoteAircraft);
        connect(m_trafficProxy, &CXSwiftBusTrafficProxy::remoteAircraftAdded, this,
                &CSimulatorXPlane::onRemoteAircraftAdded);
        connect(m_trafficProxy, &CXSwiftBusTrafficProxy::remoteAircraftAddingFailed, this,
                &CSimulatorXPlane::onRemoteAircraftAddingFailed);
        if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
        m_trafficProxy->removeAllPlanes();

        // send the settings
        this->sendXSwiftBusSettings();

        // load CSL
        this->loadCslPackages();

        // finish
        this->initSimulatorInternals();
        this->emitSimulatorCombinedStatus();

        return true;
    }

    bool CSimulatorXPlane::disconnectFrom()
    {
        if (!this->isConnected()) { return true; } // avoid emit if already disconnected
        this->disconnectFromDBus();
        if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
        delete m_serviceProxy;
        delete m_trafficProxy;
        m_serviceProxy = nullptr;
        m_trafficProxy = nullptr;
        m_fastTimerCalls = 0;
        m_slowTimerCalls = 0;

        this->emitSimulatorCombinedStatus();
        return true;
    }

    void CSimulatorXPlane::onDBusServiceUnregistered()
    {
        if (!m_serviceProxy) { return; }
        CLogMessage(this).info(u"XPlane xSwiftBus service unregistered");

        if (m_dbusMode == P2P) { QDBusConnection::disconnectFromPeer(m_dBusConnection.name()); }
        m_dBusConnection = QDBusConnection { "default" };
        if (m_watcher) { m_watcher->setConnection(m_dBusConnection); }
        delete m_serviceProxy;
        delete m_trafficProxy;
        m_serviceProxy = nullptr;
        m_trafficProxy = nullptr;
        this->emitSimulatorCombinedStatus();
    }

    void CSimulatorXPlane::emitOwnAircraftModelChanged(const QString &path, const QString &filename,
                                                       const QString &livery, const QString &icao,
                                                       const QString &modelString, const QString &name,
                                                       const QString &description)
    {
        CAircraftModel model(modelString, CAircraftModel::TypeOwnSimulatorModel, CSimulatorInfo::XPLANE, name,
                             description, icao);
        if (!livery.isEmpty()) { model.setModelString(model.getModelString() + " " + livery); }
        model.setFileName(path + "/" + filename);

        this->reverseLookupAndUpdateOwnAircraftModel(model);
    }

    void CSimulatorXPlane::displayStatusMessage(const CStatusMessage &message) const
    {
        // No assert here, as status message may come in because of network problems
        if (this->isShuttingDownOrDisconnected()) { return; }

        // avoid infinite recursion in case this function is called due to a message caused by this very function
        static bool isInFunction = false;
        if (isInFunction) { return; }
        isInFunction = true;

        std::vector<int> msgBoxValues = m_xSwiftBusServerSettings.getThreadLocal().getMessageBoxValuesVector();
        QColor color = msgBoxValues[9];
        /* switch (message.getSeverity())
        {
        case CStatusMessage::SeverityDebug:   color = "teal";   break;
        case CStatusMessage::SeverityInfo:    color = "cyan";   break;
        case CStatusMessage::SeverityWarning: color = "orange"; break;
        case CStatusMessage::SeverityError:   color = "red";    break;
        } */

        m_serviceProxy->addTextMessage("swift: " + message.getMessage(), color.redF(), color.greenF(), color.blueF());
        isInFunction = false;
    }

    void CSimulatorXPlane::displayTextMessage(const network::CTextMessage &message) const
    {
        // avoid issues during shutdown
        if (this->isShuttingDownOrDisconnected()) { return; }

        std::vector<int> msgBoxValues = m_xSwiftBusServerSettings.getThreadLocal().getMessageBoxValuesVector();
        QColor color;
        if (message.isServerMessage()) { color = msgBoxValues[8]; }
        else if (message.isSupervisorMessage()) { color = msgBoxValues[10]; }
        else if (message.isPrivateMessage()) { color = msgBoxValues[7]; }
        else { color = msgBoxValues[6]; }

        m_serviceProxy->addTextMessage(message.getSenderCallsign().toQString() + ": " + message.getMessage(),
                                       color.redF(), color.greenF(), color.blueF());
    }

    bool CSimulatorXPlane::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
    {
        return m_xplaneAircraftObjects.contains(callsign);
    }

    bool CSimulatorXPlane::updateOwnSimulatorCockpit(const simulation::CSimulatedAircraft &aircraft,
                                                     const CIdentifier &originator)
    {
        if (originator == this->identifier()) { return false; }
        if (this->isShuttingDownOrDisconnected()) { return false; } // could happen during shutdown

        auto com1 = CComSystem::getCom1System({ m_xplaneData.com1ActiveKhz, CFrequencyUnit::kHz() },
                                              { m_xplaneData.com1StandbyKhz, CFrequencyUnit::kHz() });
        auto com2 = CComSystem::getCom2System({ m_xplaneData.com2ActiveKhz, CFrequencyUnit::kHz() },
                                              { m_xplaneData.com2StandbyKhz, CFrequencyUnit::kHz() });
        auto xpdr = CTransponder::getStandardTransponder(m_xplaneData.xpdrCode,
                                                         xpdrMode(m_xplaneData.xpdrMode, m_xplaneData.xpdrIdent));
        if (aircraft.hasChangedCockpitData(com1, com2, xpdr))
        {
            m_xplaneData.com1ActiveKhz =
                aircraft.getCom1System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
            m_xplaneData.com1StandbyKhz =
                aircraft.getCom1System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
            m_xplaneData.com2ActiveKhz =
                aircraft.getCom2System().getFrequencyActive().valueInteger(CFrequencyUnit::kHz());
            m_xplaneData.com2StandbyKhz =
                aircraft.getCom2System().getFrequencyStandby().valueInteger(CFrequencyUnit::kHz());
            m_xplaneData.xpdrCode = aircraft.getTransponderCode();
            m_xplaneData.xpdrMode = xpdrMode(aircraft.getTransponderMode());
            m_serviceProxy->setCom1ActiveKhz(m_xplaneData.com1ActiveKhz);
            m_serviceProxy->setCom1StandbyKhz(m_xplaneData.com1StandbyKhz);
            m_serviceProxy->setCom2ActiveKhz(m_xplaneData.com2ActiveKhz);
            m_serviceProxy->setCom2StandbyKhz(m_xplaneData.com2StandbyKhz);
            m_serviceProxy->setTransponderCode(m_xplaneData.xpdrCode);
            m_serviceProxy->setTransponderMode(m_xplaneData.xpdrMode);

            m_serviceProxy
                ->cancelAllPendingAsyncCalls(); // in case there is already a reply with some old data incoming
            return true;
        }
        return false;
    }

    bool CSimulatorXPlane::updateOwnSimulatorSelcal(const CSelcal &selcal, const CIdentifier &originator)
    {
        if (originator == this->identifier()) { return false; }
        if (this->isShuttingDownOrDisconnected()) { return false; } // could happen during shutdown

        //! \fixme KB 8/2017 use SELCAL??
        Q_UNUSED(selcal)
        return false;
    }

    void CSimulatorXPlane::loadCslPackages()
    {
        // An ad-hoc type for keeping track of packages as they are discovered.
        // A model is a member of a package if the model path starts with the package path.
        // A trailing separator is appended only for checking if a model path starts with this path.
        struct Prefix
        {
            Prefix(const QString &p) : s(p + '/') {}
            QString parent() const
            {
                return s.section('/', 0, -2, QString::SectionSkipEmpty | QString::SectionIncludeLeadingSep);
            }
            bool isPrefixOf(const QString &o) const { return o.startsWith(s); }
            QString s;
        };

        // Heterogeneous comparison so a package can be found by binary search
        // (e.g. std::lower_bound) using a model path as the search key.
        struct PrefixComparator
        {
            bool operator()(const Prefix &a, const QString &b) const
            {
                return QStringView(a.s) < QStringView(b).left(a.s.size());
            }
            bool operator()(const QString &a, const Prefix &b) const
            {
                return QStringView(a).left(b.s.size()) < QStringView(b.s);
            }
        };

        // The list of packages discovered so far.
        QList<Prefix> packages;

        Q_ASSERT(this->isConnected());
        const CAircraftModelList models = this->getModelSet();

        // Find the CSL packages for all models in the list
        for (const auto &model : models)
        {
            const QString &modelFile = model.getFileName();
            if (modelFile.isEmpty() || !QFile::exists(modelFile)) { continue; }

            // Check if this model's package was already found
            auto it = std::lower_bound(packages.begin(), packages.end(), modelFile, PrefixComparator());
            if (it != packages.end() && it->isPrefixOf(modelFile)) { continue; }

            // This model's package was not already found, so find it and add it to the list
            QString package = findCslPackage(modelFile);
            if (package.isEmpty()) { continue; }
            packages.insert(it, package);
        }

        // comment KB 2019-06
        // a package is one xsb_aircraft.txt file BB has 9, X-CSL has 76
        CSetBuilder<QString> superpackages;
        for (const Prefix &package : std::as_const(packages)) { superpackages.insert(package.parent()); }
        QStringList superpackagesList = superpackages;

        const QDir simDir = getSimulatorSettings().getSimulatorDirectoryOrDefault();
        for (const QString &package : superpackagesList)
        {
            if (CDirectoryUtils::isSameOrSubDirectoryOf(package, simDir))
            {
                const QString message = m_trafficProxy->loadPlanesPackage(package);
                if (!message.isEmpty())
                {
                    CLogMessage(this).validationError(u"CSL package '%1' xpmp error: %2") << package << message;
                }
            }
            else
            {
                CLogMessage(this).validationError(
                    u"CSL package '%1' can not be loaded as it is outside the X-Plane installation directory")
                    << package;
            }
        }
    }

    QString CSimulatorXPlane::findCslPackage(const QString &modelFile)
    {
        //! \todo KB 2018-02 KB when I have removed the CSL dir (acciedently) there was no warning here
        const QFileInfo info(modelFile);
        QDir dir = info.isDir() ? QDir(modelFile) : info.dir();
        do {
            if (dir.exists(QStringLiteral("xsb_aircraft.txt"))) { return dir.path(); }
        }
        while (dir.cdUp());
        CLogMessage(this).warning(u"Failed to find CSL package for %1") << modelFile;
        return {};
    }

    bool CSimulatorXPlane::physicallyAddRemoteAircraft(const CSimulatedAircraft &newRemoteAircraft)
    {
        // avoid issue in rapid shutdown
        if (this->isShuttingDownOrDisconnected()) { return false; }

        // entry checks
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "thread");
        Q_ASSERT_X(!newRemoteAircraft.getCallsign().isEmpty(), Q_FUNC_INFO, "empty callsign");
        Q_ASSERT_X(newRemoteAircraft.hasModelString(), Q_FUNC_INFO, "missing model string");

        // crosscheck if still a valid aircraft
        // it can happen that aircraft has been removed, timed out ...
        if (!this->isAircraftInRangeOrTestMode(newRemoteAircraft.getCallsign()))
        {
            // next cycle will be called by callbacks or timer
            CLogMessage(this).warning(u"Aircraft '%1' no longer in range, will not add")
                << newRemoteAircraft.getCallsign();
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

            // some more validation
            if (aircraftModel.getCallsign() != newRemoteAircraft.getCallsign())
            {
                CLogMessage(this).warning(u"Model for '%1' has no callsign, maybe using a default model") << callsign;
                aircraftModel.setCallsign(callsign); // fix callsign to avoid follow up issues

                // we could disable the aircraft?
            }

            const QString livery = aircraftModel.getLivery().getCombinedCode(); //! \todo livery resolution for XP
            m_trafficProxy->addPlane(callsign, aircraftModel.getModelString(),
                                     newRemoteAircraft.getAircraftIcaoCode().getDesignator(),
                                     newRemoteAircraft.getAirlineIcaoCode().getDesignator(), livery);
            PlanesPositions pos;
            pos.push_back(newRemoteAircraft.getSituation());
            m_trafficProxy->setPlanesPositions(pos);

            PlanesSurfaces surfaces;
            surfaces.push_back(newRemoteAircraft.getCallsign(), newRemoteAircraft.getParts());
            m_trafficProxy->setPlanesSurfaces(surfaces);
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
        // avoid issue in rapid shutdown
        if (this->isShuttingDownOrDisconnected()) { return false; }

        // only remove from sim
        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "wrong thread");
        if (callsign.isEmpty()) { return false; } // can happen if an object is not an aircraft

        // really remove from simulator
        if (!this->isTestMode() && !m_xplaneAircraftObjects.contains(callsign) &&
            !m_pendingToBeAddedAircraft.containsCallsign(callsign) && !m_addingInProgressAircraft.contains(callsign))
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
        }

        // if adding in progress, postpone
        if (m_addingInProgressAircraft.contains(callsign))
        {
            // we are just about to add that aircraft
            QPointer<CSimulatorXPlane> myself(this);
            QTimer::singleShot(TimeoutAdding, this, [=] {
                if (!myself) { return; }
                m_addingInProgressAircraft.remove(callsign); // remove as "in progress"
                this->physicallyRemoveRemoteAircraft(callsign); // and remove from sim. if it was added in the mean time
            });
            return false; // do nothing right now
        }

        m_trafficProxy->removePlane(callsign.asString());
        m_xplaneAircraftObjects.remove(callsign);
        m_pendingToBeAddedAircraft.removeByCallsign(callsign);

        // bye
        return CSimulatorPluginCommon::physicallyRemoveRemoteAircraft(callsign);
    }

    int CSimulatorXPlane::physicallyRemoveAllRemoteAircraft()
    {
        if (this->isShuttingDownOrDisconnected()) { return 0; }
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
        if (!m_trafficProxy || !m_trafficProxy->isValid()) { return false; }
        m_trafficProxy->setFollowedAircraft(callsign.toQString());
        return true;
    }

    void CSimulatorXPlane::updateRemoteAircraft()
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

        uint32_t aircraftNumber = 0;
        const bool updateAllAircraft = this->isUpdateAllRemoteAircraft(currentTimestamp);
        const CCallsignSet callsignsInRange = this->getAircraftInRangeCallsigns();
        for (const CXPlaneMPAircraft &xplaneAircraft : std::as_const(m_xplaneAircraftObjects))
        {
            const CCallsign callsign(xplaneAircraft.getCallsign());
            const bool hasCallsign = !callsign.isEmpty();
            if (!hasCallsign)
            {
                // does not make sense to continue here
                SWIFT_VERIFY_X(false, Q_FUNC_INFO, "missing callsign");
                continue;
            }

            // skip no longer in range
            if (!callsignsInRange.contains(callsign)) { continue; }

            planesTransponders.callsigns.push_back(callsign.asString());
            planesTransponders.codes.push_back(xplaneAircraft.getAircraft().getTransponderCode());
            CTransponder::TransponderMode transponderMode = xplaneAircraft.getAircraft().getTransponderMode();
            planesTransponders.idents.push_back(transponderMode == CTransponder::StateIdent);
            planesTransponders.modeCs.push_back(transponderMode == CTransponder::ModeC);

            // setup
            const CInterpolationAndRenderingSetupPerCallsign setup =
                this->getInterpolationSetupConsolidated(callsign, updateAllAircraft);

            // interpolated situation/parts
            const CInterpolationResult result =
                xplaneAircraft.getInterpolation(currentTimestamp, setup, aircraftNumber++);
            if (result.getInterpolationStatus().hasValidSituation())
            {
                CAircraftSituation interpolatedSituation(result);

                // adjust altitude to compensate for XP12 temperature effect
                const CLength relativeAltitude =
                    interpolatedSituation.geodeticHeight() - getOwnAircraftPosition().geodeticHeight();
                const double altitudeDeltaWeight =
                    2 - qBound(3000.0, relativeAltitude.abs().value(CLengthUnit::ft()), 6000.0) / 3000;
                const CLength alt = interpolatedSituation.getAltitude() +
                                    m_altitudeDelta * altitudeDeltaWeight *
                                        (1 - interpolatedSituation.getOnGroundInfo().getGroundFactor());
                interpolatedSituation.setAltitude({ alt, interpolatedSituation.getAltitude().getReferenceDatum() });

                // update situation
                if (updateAllAircraft || !this->isEqualLastSent(interpolatedSituation))
                {
                    this->rememberLastSent(interpolatedSituation);
                    planesPositions.push_back(interpolatedSituation);
                }
            }
            else
            {
                CLogMessage(this).warning(
                    this->getInvalidSituationLogMessage(callsign, result.getInterpolationStatus()));
            }

            const CAircraftParts parts(result);
            if (result.getPartsStatus().isSupportingParts() || parts.getPartsDetails() == CAircraftParts::GuessedParts)
            {
                if (updateAllAircraft || !this->isEqualLastSent(parts, callsign))
                {
                    this->rememberLastSent(parts, callsign);
                    planesSurfaces.push_back(xplaneAircraft.getCallsign(), parts);
                }
            }

        } // all callsigns

        if (!planesTransponders.isEmpty()) { m_trafficProxy->setPlanesTransponders(planesTransponders); }

        if (!planesPositions.isEmpty())
        {
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                SWIFT_VERIFY_X(planesPositions.hasSameSizes(), Q_FUNC_INFO, "Mismatching sizes");
            }
            m_trafficProxy->setPlanesPositions(planesPositions);
        }

        if (!planesSurfaces.isEmpty()) { m_trafficProxy->setPlanesSurfaces(planesSurfaces); }

        // stats
        this->finishUpdateRemoteAircraftAndSetStatistics(currentTimestamp);
    }

    void CSimulatorXPlane::requestRemoteAircraftDataFromXPlane()
    {
        if (this->isShuttingDownOrDisconnected()) { return; }

        // It is not required to request all elevations and CGs, but only for aircraft "near ground relevant"
        // - we could use the elevation cache and CG cache to decide if we need to request
        // - if an aircraft is on ground but not moving, we do not need to request elevation if we already have it (it
        // will not change
        CCallsignSet callsigns = m_xplaneAircraftObjects.getAllCallsigns();
        const CCallsignSet remove = this->getLastSentCanLikelySkipNearGroundInterpolation().getCallsigns();
        callsigns.remove(remove);
        if (!callsigns.isEmpty()) { this->requestRemoteAircraftDataFromXPlane(callsigns); }
    }

    void CSimulatorXPlane::requestRemoteAircraftDataFromXPlane(const CCallsignSet &callsigns)
    {
        if (callsigns.isEmpty()) { return; }
        if (!m_trafficProxy || this->isShuttingDown()) { return; }
        const QStringList csStrings = callsigns.getCallsignStrings();
        QPointer<CSimulatorXPlane> myself(this);
        m_trafficProxy->getRemoteAircraftData(
            csStrings, [=](const QStringList &callsigns, const QDoubleList &latitudesDeg,
                           const QDoubleList &longitudesDeg, const QDoubleList &elevationsMeters,
                           const QBoolList &waterFlags, const QDoubleList &verticalOffsetsMeters) {
                if (!myself) { return; }
                this->updateRemoteAircraftFromSimulator(callsigns, latitudesDeg, longitudesDeg, elevationsMeters,
                                                        waterFlags, verticalOffsetsMeters);
            });
    }

    void CSimulatorXPlane::triggerRequestRemoteAircraftDataFromXPlane(const CCallsignSet &callsigns)
    {
        if (callsigns.isEmpty()) { return; }
        QPointer<CSimulatorXPlane> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            this->requestRemoteAircraftDataFromXPlane(callsigns);
        });
    }

    void CSimulatorXPlane::updateRemoteAircraftFromSimulator(
        const QStringList &callsigns, const QDoubleList &latitudesDeg, const QDoubleList &longitudesDeg,
        const QDoubleList &elevationsMeters, const QBoolList &waterFlags, const QDoubleList &verticalOffsetsMeters)
    {
        const int size = callsigns.size();

        // we skip if we are not near ground
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            Q_ASSERT_X(elevationsMeters.size() == size, Q_FUNC_INFO, "Wrong elevations");
            Q_ASSERT_X(waterFlags.size() == size, Q_FUNC_INFO, "Wrong waterFlags");
            Q_ASSERT_X(latitudesDeg.size() == size, Q_FUNC_INFO, "Wrong latitudesDeg");
            Q_ASSERT_X(longitudesDeg.size() == size, Q_FUNC_INFO, "Wrong longitudesDeg");
            Q_ASSERT_X(verticalOffsetsMeters.size() == size, Q_FUNC_INFO, "Wrong CG");
        }

        const CCallsignSet logCallsigns = this->getLogCallsigns();
        static const QString hint("remote acft.");
        for (int i = 0; i < size; i++)
        {
            const bool emptyCs = callsigns[i].isEmpty();
            SWIFT_VERIFY_X(!emptyCs, Q_FUNC_INFO, "Need callsign");
            if (emptyCs) { continue; }

            const CCallsign cs(callsigns[i]);
            if (!m_xplaneAircraftObjects.contains(cs)) { continue; }
            const CXPlaneMPAircraft xpAircraft = m_xplaneAircraftObjects[cs];
            SWIFT_VERIFY_X(xpAircraft.hasCallsign(), Q_FUNC_INFO, "Need callsign");
            if (!xpAircraft.hasCallsign()) { continue; }

            CElevationPlane elevation = CElevationPlane::null();
            if (!std::isnan(elevationsMeters[i]))
            {
                const CAltitude elevationAlt = CAltitude(elevationsMeters[i], CLengthUnit::m(), CLengthUnit::ft());
                elevation = CElevationPlane(CLatitude(latitudesDeg[i], CAngleUnit::deg()),
                                            CLongitude(longitudesDeg[i], CAngleUnit::deg()), elevationAlt,
                                            CElevationPlane::singlePointRadius());
            }

            const double cgValue = verticalOffsetsMeters[i]; // XP offset is swift CG
            CLength cg = std::isnan(cgValue) ? CLength::null() : CLength(cgValue, CLengthUnit::m(), CLengthUnit::ft());
            cg = fixSimulatorCg(cg, xpAircraft.getAircraftModel());

            // if we knew "on ground" here we could set it as parameter of rememberElevationAndSimulatorCG
            // with T778 we do NOT use this function for elevation here if "isSuspicious"
            const bool waterFlag = waterFlags[i];
            const bool useElevation = this->handleProbeValue(elevation, cs, waterFlag, hint, true);
            this->rememberElevationAndSimulatorCG(cs, xpAircraft.getAircraftModel(), false,
                                                  useElevation ? elevation : CElevationPlane::null(), cg);

            // loopback
            if (logCallsigns.contains(cs)) { this->addLoopbackSituation(cs, elevation, cg); }
        }
    }

    CLength CSimulatorXPlane::fixSimulatorCg(const CLength &cg, const CAircraftModel &model)
    {
        //! \todo KB 2020-04 hardcoded fix for XCSL
        // For XCSL null means no offset => 0
        // so not to override it with some DB value in X mode, we set it to "0"
        if (model.getDistributor().matchesKeyOrAlias(CDistributor::xplaneXcsl()) && cg.isNull())
        {
            return { 0.0, CLengthUnit::ft() };
        }
        return cg;
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

    bool CSimulatorXPlane::sendXSwiftBusSettings()
    {
        if (this->isShuttingDownOrDisconnected()) { return false; }
        if (!m_serviceProxy) { return false; }
        CXSwiftBusSettings s = m_xSwiftBusServerSettings.get();
        s.setCurrentUtcTime();
        m_serviceProxy->setSettingsJson(s.toXSwiftBusJsonStringQt());
        CLogMessage(this).info(u"Send settings: %1") << s.toQString(true);
        return true;
    }

    CXSwiftBusSettings CSimulatorXPlane::receiveXSwiftBusSettings(bool &ok)
    {
        ok = false;
        CXSwiftBusSettings s;

        if (this->isShuttingDownOrDisconnected()) { return s; }
        if (!m_serviceProxy) { return s; }

        const QString json = m_serviceProxy->getSettingsJson();
        s.parseXSwiftBusStringQt(json);
        ok = true;
        return s;
    }

    void CSimulatorXPlane::onXSwiftBusSettingsChanged()
    {
        bool ok {};
        const CXSwiftBusSettings xPlaneSide = this->receiveXSwiftBusSettings(ok);
        if (ok)
        {
            // we only send if DBus did not change
            // DBus changes would require a restart
            const CXSwiftBusSettings swiftSide = m_xSwiftBusServerSettings.get();
            if (xPlaneSide.getDBusServerAddressQt() == swiftSide.getDBusServerAddressQt())
            {
                this->sendXSwiftBusSettings();
            }
        }
    }

    void CSimulatorXPlane::setMinTerrainProbeDistance(const CLength &distance)
    {
        if (distance.isNull()) { return; }
        if (m_minSuspicousTerrainProbe.isNull() || distance < m_minSuspicousTerrainProbe)
        {
            m_minSuspicousTerrainProbe = distance;
        }
    }

    void CSimulatorXPlane::onRemoteAircraftAdded(const QString &callsign)
    {
        SWIFT_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
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
            m_statsAddMaxTimeMs = std::max(deltaTimeMs, m_statsAddMaxTimeMs);
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
        this->triggerRequestRemoteAircraftDataFromXPlane(cs);
        this->triggerAddNextPendingAircraft();

        Q_ASSERT_X(addedRemoteAircraft.hasCallsign(), Q_FUNC_INFO,
                   "No callsign"); // already checked above, MUST never happen
        Q_ASSERT_X(addedRemoteAircraft.getCallsign() == cs, Q_FUNC_INFO,
                   "No callsign"); // already checked above, MUST never happen
        m_xplaneAircraftObjects.insert(cs, CXPlaneMPAircraft(addedRemoteAircraft, this, &m_interpolationLogger));
        emit this->aircraftRenderingChanged(addedRemoteAircraft);
    }

    void CSimulatorXPlane::onRemoteAircraftAddingFailed(const QString &callsign)
    {
        SWIFT_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
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
            CLogMessage(this).warning(u"Adding '%1' failed, but aircraft no longer in range, will be removed")
                << callsign;
        }

        const bool wasPending = (static_cast<int>(m_addingInProgressAircraft.remove(cs)) > 0);
        Q_UNUSED(wasPending)

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
        CLogMessage(this).info(u"Adding next pending aircraft '%1', pending %2, in progress %3")
            << newRemoteAircraft.getCallsignAsString() << m_pendingToBeAddedAircraft.size()
            << m_addingInProgressAircraft.size();
        this->physicallyAddRemoteAircraft(newRemoteAircraft);
    }

    void CSimulatorXPlane::triggerAddNextPendingAircraft()
    {
        QPointer<CSimulatorXPlane> myself(this);
        QTimer::singleShot(100, this, [=] {
            if (!myself || !sApp || sApp->isShuttingDown()) { return; }
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

        for (const CCallsign &cs : std::as_const(timeoutCallsigns))
        {
            m_addingInProgressAircraft.remove(cs);
            CLogMessage(this).warning(u"Adding for '%1' timed out") << cs.asString();
        }

        return timeoutCallsigns.size();
    }

    void CSimulatorXPlane::triggerRemoveAircraft(const CCallsign &callsign, qint64 deferMs)
    {
        QPointer<CSimulatorXPlane> myself(this);
        QTimer::singleShot(deferMs, this, [=] {
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
        return { *mm.first, *mm.second };
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
                                                IClientProvider *clientProvider)
    {
        return new CSimulatorXPlane(info, ownAircraftProvider, remoteAircraftProvider, clientProvider, this);
    }

    CSimulatorXPlaneListener::CSimulatorXPlaneListener(const CSimulatorPluginInfo &info) : ISimulatorListener(info)
    {
        constexpr int QueryInterval = 5 * 1000; // 5 seconds
        m_timer.setInterval(QueryInterval);
        m_timer.setObjectName(this->objectName().append(":m_timer"));
        connect(&m_timer, &QTimer::timeout, this, &CSimulatorXPlaneListener::checkConnection);
    }

    void CSimulatorXPlaneListener::startImpl() { m_timer.start(); }

    void CSimulatorXPlaneListener::stopImpl() { m_timer.stop(); }

    void CSimulatorXPlaneListener::checkImpl()
    {
        if (!m_timer.isActive()) { return; }
        if (this->isShuttingDown()) { return; }

        m_timer.start(); // restart because we will check just now
        QPointer<CSimulatorXPlaneListener> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself) { return; }
            checkConnection();
        });
    }

    void CSimulatorXPlaneListener::checkConnection()
    {
        if (this->isShuttingDown()) { return; }
        Q_ASSERT_X(!CThreadUtils::thisIsMainThread(), Q_FUNC_INFO, "Expect to run in background");
        QElapsedTimer t;
        t.start();

        QString via;
        m_dBusServerAddress = m_xSwiftBusServerSettings.getThreadLocal().getDBusServerAddressQt();
        if (CDBusServer::isSessionOrSystemAddress(m_dBusServerAddress))
        {
            checkConnectionViaSessionBus();
            via = "SessionBus";
        }
        else if (CDBusServer::isQtDBusAddress(m_dBusServerAddress))
        {
            checkConnectionViaPeer(m_dBusServerAddress);
            via = "P2P";
        }

        CLogMessage(this).debug(u"Checked sim. 'XPLANE' via '%1' connection in %2ms") << via << t.elapsed();
    }

    void CSimulatorXPlaneListener::checkConnectionViaSessionBus()
    {
        m_DBusConnection = QDBusConnection::sessionBus();
        if (!m_DBusConnection.isConnected())
        {
            QDBusConnection::disconnectFromBus(m_DBusConnection.name());
            return;
        }
        checkConnectionCommon(); // bus
        QDBusConnection::disconnectFromBus(m_DBusConnection.name());
    }

    void CSimulatorXPlaneListener::checkConnectionViaPeer(const QString &address)
    {
        m_DBusConnection = QDBusConnection::connectToPeer(address, "xswiftbus");
        if (!m_DBusConnection.isConnected())
        {
            // This is required to cleanup the connection in QtDBus
            QDBusConnection::disconnectFromPeer(m_DBusConnection.name());
            return;
        }
        checkConnectionCommon(); // peer
        QDBusConnection::disconnectFromPeer(m_DBusConnection.name());
    }

    void CSimulatorXPlaneListener::checkConnectionCommon()
    {
        CXSwiftBusServiceProxy service(m_DBusConnection);
        CXSwiftBusTrafficProxy traffic(m_DBusConnection);

        const bool result = service.isValid() && traffic.isValid();
        if (!result) { return; }

        const QString swiftVersion = CBuildConfig::getVersionString();
        const QString xswiftbusVersion = service.getVersionNumber();
        const QString xswiftbusCommitHash = service.getCommitHash();
        if (xswiftbusVersion.isEmpty())
        {
            CLogMessage(this).warning(u"Could not determine which version of xswiftbus is running. Mismatched versions "
                                      u"might cause instability.");
        }
        else if (commitHash() != xswiftbusCommitHash)
        {
            CLogMessage(this).error(u"You are using an incorrect version of xswiftbus. The version of xswiftbus (%1) "
                                    u"should match the version of swift (%2). Consider upgrading!")
                << xswiftbusVersion << swiftVersion;
        }

        if (!traffic.initialize())
        {
            CLogMessage(this).error(
                u"Connection to xswiftbus successful, but could not initialize xswiftbus. Check X-Plane Log.txt.");
            return;
        }

        const MultiplayerAcquireInfo info = traffic.acquireMultiplayerPlanes();
        if (!info.hasAcquired)
        {
            const QString owner =
                info.owner.trimmed().isEmpty() ? QStringLiteral("unknown plugin") : info.owner.trimmed();
            CLogMessage(this).error(
                u"Connection to xswiftbus successful, but could not acquire multiplayer planes. '%1' has acquired them "
                u"already. Disable '%2' or remove it if not required and reload xswiftbus.")
                << owner << owner.toLower();
            return;
        }

        emit simulatorStarted(getPluginInfo());
    }

    void CSimulatorXPlaneListener::serviceRegistered(const QString &serviceName)
    {
        if (serviceName == xswiftbusServiceName()) { emit simulatorStarted(getPluginInfo()); }
        QDBusConnection::disconnectFromBus(m_DBusConnection.name());
    }

    void CSimulatorXPlaneListener::onXSwiftBusServerSettingChanged()
    {
        const CXSwiftBusSettings s = m_xSwiftBusServerSettings.get();
        if (m_dBusServerAddress != s.getDBusServerAddressQt())
        {
            this->stop();
            this->start();
            m_dBusServerAddress = s.getDBusServerAddressQt();
        }
    }
} // namespace swift::simplugin::xplane

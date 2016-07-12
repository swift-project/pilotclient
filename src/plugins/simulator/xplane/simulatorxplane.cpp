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
#include "xbusserviceproxy.h"
#include "xbustrafficproxy.h"
#include "xbusweatherproxy.h"

#include <QColor>
#include <QDBusServiceWatcher>
#include <QString>
#include <QTimer>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

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
                                           IWeatherGridProvider *weatherGridProvider,
                                           QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, parent)
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

            m_modelMatcher.setDefaultModel(CAircraftModel(
                                               "Jets A320_a A320_a_Austrian_Airlines A320_a_Austrian_Airlines",
                                               CAircraftModel::TypeModelMatchingDefaultModel,
                                               "A320 AUA",
                                               CAircraftIcaoCode("A320", "L2J")
                                           ));

            resetData();
            ps_reloadSettings();
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

                CAircraftSituation situation;
                situation.setPosition({ m_xplaneData.latitude, m_xplaneData.longitude, 0 });
                situation.setAltitude({ m_xplaneData.altitude, CAltitude::MeanSeaLevel, CLengthUnit::m() });
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

                CAircraftParts parts { {
                        m_xplaneData.strobeLightsOn, m_xplaneData.landingLightsOn, m_xplaneData.taxiLightsOn,
                        m_xplaneData.beaconLightsOn, m_xplaneData.navLightsOn, false
                    },
                    m_xplaneData.gearReployRatio > 0, static_cast<int>(m_xplaneData.flapsReployRatio * 100) ,
                    m_xplaneData.speedBrakeRatio > 0.5, engines, m_xplaneData.onGroundAll
                };
                updateOwnParts(parts);
            }
        }

        void CSimulatorXPlane::ps_installedModelsUpdated(const QStringList &modelStrings, const QStringList &icaos, const QStringList &airlines, const QStringList &liveries)
        {
            m_installedModels.clear();
            auto modelStringsIt = modelStrings.begin();
            auto icaosIt = icaos.begin();
            auto airlinesIt = airlines.begin();
            auto liveriesIt = liveries.begin();
            for (; modelStringsIt != modelStrings.end() && icaosIt != icaos.end() && airlinesIt != airlines.end() && liveriesIt != liveries.end(); ++modelStringsIt, ++icaosIt, ++airlinesIt, ++liveriesIt)
            {
                Q_UNUSED(liveriesIt);
                using namespace BlackMisc::Simulation;
                CAircraftIcaoCode aircraftIcao(*icaosIt);
                CLivery livery;   //! \todo resolve livery
                livery.setAirlineIcaoCode(CAirlineIcaoCode(*airlinesIt));
                CAircraftModel aircraftModel(*modelStringsIt, CAircraftModel::TypeDatabaseEntry, QString(), aircraftIcao, livery);
                m_installedModels.push_back(aircraftModel);
            }
        }

        void CSimulatorXPlane::ps_reloadSettings()
        {
            auto selectedWeatherScenario = m_weatherScenarioSettings.get();
            if (CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
            {
                m_lastWeatherPosition = {};
                injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
            }
        }

        bool CSimulatorXPlane::isConnected() const
        {
            return m_service && m_traffic && m_weather;
        }

        bool CSimulatorXPlane::connectTo()
        {
            if (isConnected()) { return true; }
            m_conn = QDBusConnection::sessionBus(); // TODO make this configurable
            m_service = new CXBusServiceProxy(m_conn, this);
            m_traffic = new CXBusTrafficProxy(m_conn, this);
            m_weather = new CXBusWeatherProxy(m_conn, this);

            if (m_service->isValid() && m_traffic->isValid() && m_weather->isValid() && m_traffic->initialize())
            {
                // FIXME duplication
                connect(m_service, &CXBusServiceProxy::aircraftModelChanged, this, &CSimulatorXPlane::ps_emitOwnAircraftModelChanged);
                connect(m_service, &CXBusServiceProxy::airportsInRangeUpdated, this, &CSimulatorXPlane::ps_setAirportsInRange);
                connect(m_traffic, &CXBusTrafficProxy::installedModelsUpdated, this, &CSimulatorXPlane::ps_installedModelsUpdated);
                m_service->updateAirportsInRange();
                m_traffic->updateInstalledModels();
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

        bool CSimulatorXPlane::disconnectFrom()
        {
            if (!this->isConnected()) { return true; } // avoid emit if already disconnected
            if (m_traffic)
            {
                m_traffic->cleanup();
            }

            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            delete m_traffic;
            delete m_weather;
            m_service = nullptr;
            m_traffic = nullptr;
            m_weather = nullptr;
            emitSimulatorCombinedStatus();
            return true;
        }

        void CSimulatorXPlane::ps_serviceUnregistered()
        {
            m_conn = QDBusConnection { "default" };
            m_watcher->setConnection(m_conn);
            delete m_service;
            delete m_traffic;
            delete m_weather;
            m_service = nullptr;
            m_traffic = nullptr;
            m_weather = nullptr;
            emitSimulatorCombinedStatus();
        }

        void CSimulatorXPlane::ps_emitOwnAircraftModelChanged(const QString &path, const QString &filename, const QString &livery, const QString &icao)
        {
            Q_UNUSED(icao);

            //! \todo XP, change as appropriate, add resolution of own livery
            // try to set correct model and ICAO values here
            // thy show up in GUI
            CAircraftModel model(getOwnAircraftModel());
            CAirlineIcaoCode airlineIcaoCode(model.getAirlineIcaoCode());
            model.setModelType(CAircraftModel::TypeOwnSimulatorModel);
            model.setFileName(path + "/" + filename);
            model.setLivery(CLivery("XPLANE." + livery, airlineIcaoCode, "XP livery", "", "", false));

            // updates
            updateOwnIcaoCodes(model.getAircraftIcaoCode(), airlineIcaoCode);
            updateOwnModel(model);
            emit ownAircraftModelChanged(getOwnAircraft());
        }

        void CSimulatorXPlane::displayStatusMessage(const BlackMisc::CStatusMessage &message) const
        {
            // No assert here as status message may come because of network problems
            if (!isConnected()) { return; }

            // avoid infinite recursion in case this function is called due to a message caused by this very function
            static bool isInFunction = false;
            if (isInFunction) { return; }
            isInFunction = true;

            QColor color;
            switch (message.getSeverity())
            {
            case CStatusMessage::SeverityDebug: color = "teal"; break;
            case CStatusMessage::SeverityInfo: color = "cyan"; break;
            case CStatusMessage::SeverityWarning: color = "orange"; break;
            case CStatusMessage::SeverityError: color = "red"; break;
            }

            m_service->addTextMessage("swift: " + message.getMessage(), color.redF(), color.greenF(), color.blueF());
            isInFunction = false;
        }

        void CSimulatorXPlane::displayTextMessage(const BlackMisc::Network::CTextMessage &message) const
        {
            if (!isConnected()) { return; }

            QColor color;
            if (message.isServerMessage()) { color = "orchid"; }
            else if (message.isSupervisorMessage()) { color = "yellow"; }
            else if (message.isPrivateMessage()) { color = "magenta"; }
            else { color = "lime"; }

            m_service->addTextMessage(message.getSenderCallsign().toQString() + ": " + message.getMessage(), color.redF(), color.greenF(), color.blueF());
        }

        CAircraftModelList CSimulatorXPlane::getInstalledModels() const
        {
            Q_ASSERT(isConnected());
            return m_installedModels;
        }

        void CSimulatorXPlane::ps_setAirportsInRange(const QStringList &icaos, const QStringList &names, const BlackMisc::CSequence<double> &lats, const BlackMisc::CSequence<double> &lons, const BlackMisc::CSequence<double> &alts)
        {
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
        }

        BlackMisc::Aviation::CAirportList CSimulatorXPlane::getAirportsInRange() const
        {
            //! \todo XP driver: Check if units match, xPlaneData has now hints what the values are
            return m_airportsInRange;
        }

        bool CSimulatorXPlane::setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset)
        {
            Q_UNUSED(offset);
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

        QDBusConnection CSimulatorXPlane::connectionFromString(const QString &str)
        {
            if (str == BlackMisc::CDBusServer::sessionBusAddress())
            {
                return QDBusConnection::sessionBus();
            }
            else if (str == BlackMisc::CDBusServer::systemBusAddress())
            {
                return QDBusConnection::systemBus();
            }
            else
            {
                Q_UNREACHABLE();
            }
        }

        bool CSimulatorXPlane::isPhysicallyRenderedAircraft(const CCallsign &callsign) const
        {
            //! \todo XP implement isRenderedAircraft correctly. This work around, but not really telling me if callsign is really(!) visible in SIM
            return getAircraftInRangeForCallsign(callsign).isRendered();
        }

        bool CSimulatorXPlane::updateOwnSimulatorCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            Q_ASSERT(isConnected());
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

            // matched models
            CAircraftModel aircraftModel = m_modelMatcher.getClosestMatch(newRemoteAircraft);
            Q_ASSERT_X(newRemoteAircraft.getCallsign() == aircraftModel.getCallsign(), Q_FUNC_INFO, "mismatching callsigns");

            CCallsign callsign(newRemoteAircraft.getCallsign());
            this->updateAircraftModel(callsign, aircraftModel);
            CSimulatedAircraft aircraftAfterModelApplied(getAircraftInRangeForCallsign(newRemoteAircraft.getCallsign()));

            QString livery = aircraftModel.getLivery().getCombinedCode(); //! \todo livery resolution for XP
            m_traffic->addPlane(newRemoteAircraft.getCallsign().asString(), aircraftModel.getModelString(),
                                newRemoteAircraft.getAircraftIcaoCode().getDesignator(),
                                newRemoteAircraft.getAirlineIcaoCode().getDesignator(),
                                livery);
            updateAircraftRendered(newRemoteAircraft.getCallsign(), true);
            CLogMessage(this).info("XP: Added aircraft %1") << newRemoteAircraft.getCallsign().toQString();

            bool rendered = true;
            aircraftAfterModelApplied.setRendered(rendered);
            this->updateAircraftRendered(callsign, rendered);
            emit modelMatchingCompleted(aircraftAfterModelApplied);

            return rendered;
        }

        void CSimulatorXPlane::ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
        {
            Q_ASSERT(isConnected());
            using namespace BlackMisc::PhysicalQuantities;
            m_traffic->addPlanePosition(situation.getCallsign().asString(),
                                        situation.latitude().value(CAngleUnit::deg()),
                                        situation.longitude().value(CAngleUnit::deg()),
                                        situation.getAltitude().value(CLengthUnit::ft()),
                                        situation.getPitch().value(CAngleUnit::deg()),
                                        situation.getBank().value(CAngleUnit::deg()),
                                        situation.getHeading().value(CAngleUnit::deg()),
                                        situation.getAdjustedMSecsSinceEpoch() - QDateTime::currentMSecsSinceEpoch());
        }

        void CSimulatorXPlane::ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts)
        {
            Q_ASSERT(isConnected());
            m_traffic->setPlaneSurfaces(callsign.asString(), true, 0, 0, 0, 0, 0, 0, 0, 0, 0, true, true, true, true, 0); // TODO landing gear, lights, control surfaces
            m_traffic->setPlaneTransponder(callsign.asString(), 2000, true, false); // TODO transponder
            Q_UNUSED(parts); // TODO
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
            updateAircraftRendered(callsign, false);
            CLogMessage(this).info("XP: Removed aircraft %1") << callsign.toQString();
            return true;
        }

        int CSimulatorXPlane::physicallyRemoveAllRemoteAircraft()
        {
            //! \todo XP driver obtain number of removed aircraft
            int r = getAircraftInRangeCount();
            m_traffic->removeAllPlanes();
            updateMarkAllAsNotRendered();
            CLogMessage(this).info("XP: Removed all aircraft");
            return r;
        }

        CCallsignSet CSimulatorXPlane::physicallyRenderedAircraft() const
        {
            //! \todo XP driver, return list of callsigns really present in the simulator
            return getAircraftInRange().findByRendered(true).getCallsigns(); // just a poor workaround
        }

        bool CSimulatorXPlane::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
        {
            // remove upfront, and then enable / disable again
            this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
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

        void CSimulatorXPlane::injectWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid)
        {
            m_weather->setUseRealWeather(false);

            // todo: find the closest
            CGridPoint gridPoint = weatherGrid.front();

            // todo: find the closest
            auto visibilityLayers = gridPoint.getVisibilityLayers();
            visibilityLayers.sortBy(&CVisibilityLayer::getBase);
            const CVisibilityLayer visibilityLayer = visibilityLayers.frontOrDefault();
            m_weather->setVisibility(visibilityLayer.getVisibility().value(CLengthUnit::m()));

            CTemperatureLayerList temperatureLayers = gridPoint.getTemperatureLayers();
            temperatureLayers.sortBy(&CTemperatureLayer::getLevel);
            const CTemperatureLayer temperatureLayer = temperatureLayers.frontOrDefault();
            m_weather->setTemperature(temperatureLayer.getTemperature().value(CTemperatureUnit::C()));
            m_weather->setDewPoint(temperatureLayer.getDewPoint().value(CTemperatureUnit::C()));
            m_weather->setQNH(gridPoint.getSurfacePressure().value(CPressureUnit::inHg()));

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

                m_weather->setCloudLayer(layerNumber, base, top, type, coverage);
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
                int altitudeMeter = windLayer.getLevel().value(CLengthUnit::m());
                double directionDeg = windLayer.getDirection().value(CAngleUnit::deg());
                int speedKts = windLayer.getSpeed().value(CSpeedUnit::kts());
                m_weather->setWindLayer(layerNumber, altitudeMeter, directionDeg, speedKts, 0, 0, 0);
                layerNumber++;
            }

            m_weather->setPrecipitationRatio(cloudLayers.frontOrDefault().getPrecipitationRate());
            m_weather->setThunderstormRatio(0.0);
        }

        BlackCore::ISimulator *CSimulatorXPlaneFactory::create(const CSimulatorPluginInfo &info,
                IOwnAircraftProvider *ownAircraftProvider,
                IRemoteAircraftProvider *remoteAircraftProvider,
                IWeatherGridProvider *weatherGridProvider)
        {
            return new CSimulatorXPlane(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, this);
        }

        CSimulatorXPlaneListener::CSimulatorXPlaneListener(const CSimulatorPluginInfo &info): ISimulatorListener(info)
        { }

        void CSimulatorXPlaneListener::start()
        {
            if (m_watcher) { return; } // already started
            if (isXBusRunning())
            {
                emit simulatorStarted(getPluginInfo());
            }
            else
            {
                CLogMessage(this).debug() << "Starting XBus on" << m_xbusServerSetting.getThreadLocal();
                m_conn = CSimulatorXPlane::connectionFromString(m_xbusServerSetting.getThreadLocal());
                m_watcher = new QDBusServiceWatcher(xbusServiceName(), m_conn, QDBusServiceWatcher::WatchForRegistration, this);
                connect(m_watcher, &QDBusServiceWatcher::serviceRegistered, this, &CSimulatorXPlaneListener::ps_serviceRegistered);
            }
        }

        void CSimulatorXPlaneListener::stop()
        {
            if (m_watcher)
            {
                m_watcher->deleteLater();
                m_watcher = nullptr;
            }
        }

        bool CSimulatorXPlaneListener::isXBusRunning() const
        {
            QDBusConnection conn = CSimulatorXPlane::connectionFromString(m_xbusServerSetting.getThreadLocal());
            CXBusServiceProxy *service = new CXBusServiceProxy(conn);
            CXBusTrafficProxy *traffic = new CXBusTrafficProxy(conn);

            bool result = service->isValid() && traffic->isValid();

            service->deleteLater();
            traffic->deleteLater();

            return result;
        }

        void CSimulatorXPlaneListener::ps_serviceRegistered(const QString &serviceName)
        {
            if (serviceName == xbusServiceName())
            {
                emit simulatorStarted(getPluginInfo());
            }
        }

        void CSimulatorXPlaneListener::ps_xbusServerSettingChanged()
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

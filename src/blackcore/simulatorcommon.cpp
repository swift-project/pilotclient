/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/simulatorcommon.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/db/databaseutils.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/interpolator.h"
#include "blackmisc/simulation/interpolationhints.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/threadutils.h"

#include <QDateTime>
#include <QString>
#include <QThread>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <functional>

using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore;
using namespace BlackCore::Db;

namespace BlackCore
{
    CSimulatorCommon::CSimulatorCommon(const CSimulatorPluginInfo &info,
                                       BlackMisc::Simulation::IOwnAircraftProvider    *ownAircraftProvider,
                                       BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                                       IWeatherGridProvider                           *weatherGridProvider,
                                       QObject *parent)
        : ISimulator(parent),
          COwnAircraftAware(ownAircraftProvider),
          CRemoteAircraftAware(remoteAircraftProvider),
          CWeatherGridAware(weatherGridProvider),
          m_simulatorPluginInfo(info)
    {
        this->setObjectName("Simulator: " + info.getIdentifier());
        CSimulatorCommon::registerHelp();

        // provider signals, hook up with remote aircraft provider
        m_remoteAircraftProviderConnections.append(
            m_remoteAircraftProvider->connectRemoteAircraftProviderSignals(
                this, // receiver must match object in bind
                std::bind(&CSimulatorCommon::rapOnRemoteProviderAddedAircraftSituation, this, std::placeholders::_1),
                std::bind(&CSimulatorCommon::rapOnRemoteProviderAddedAircraftParts, this, std::placeholders::_1, std::placeholders::_2),
                std::bind(&CSimulatorCommon::rapOnRemoteProviderRemovedAircraft, this, std::placeholders::_1),
                std::bind(&CSimulatorCommon::rapOnRecalculatedRenderedAircraft, this, std::placeholders::_1))
        );

        // timer
        connect(&m_oneSecondTimer, &QTimer::timeout, this, &CSimulatorCommon::oneSecondTimerTimeout);
        m_oneSecondTimer.setObjectName(this->objectName().append(":m_oneSecondTimer"));
        m_oneSecondTimer.start(1000);

        // swift data
        if (sApp && sApp->hasWebDataServices())
        {
            connect(sApp->getWebDataServices(), &CWebDataServices::allSwiftDbDataRead, this, &CSimulatorCommon::onSwiftDbAllDataRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAirportsRead, this, &CSimulatorCommon::onSwiftDbAirportsRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbModelMatchingEntitiesRead, this, &CSimulatorCommon::onSwiftDbModelMatchingEntitiesRead, Qt::QueuedConnection);
        }

        // info
        CLogMessage(this).info("Initialized simulator driver: '%1'") << m_simulatorPluginInfo.toQString();
    }

    CSimulatorCommon::~CSimulatorCommon() { }

    const CLogCategoryList &CSimulatorCommon::getLogCategories()
    {
        static const CLogCategoryList cats({ CLogCategory::driver(), CLogCategory::plugin() });
        return cats;
    }

    bool CSimulatorCommon::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        Q_ASSERT_X(remoteAircraft.hasModelString(), Q_FUNC_INFO, "Missing model string");
        Q_ASSERT_X(remoteAircraft.hasCallsign(), Q_FUNC_INFO, "Missing callsign");

        const bool renderingRestricted = m_interpolationRenderingSetup.isRenderingRestricted();
        if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QString("Restricted: %1 cs: '%2' enabled: %3").arg(boolToYesNo(renderingRestricted), remoteAircraft.getCallsignAsString(), boolToYesNo(remoteAircraft.isEnabled()))); }
        if (!remoteAircraft.isEnabled()) { return false; }

        // if not restriced, directly change
        if (!renderingRestricted) { this->callPhysicallyAddRemoteAircraft(remoteAircraft); return true; }

        // restricted -> will be added with next snapshot onRecalculatedRenderedAircraft
        return false;
    }

    bool CSimulatorCommon::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        // if not restriced, directly change
        if (!m_interpolationRenderingSetup.isRenderingRestricted())
        {
            m_statsPhysicallyAddedAircraft++;
            this->callPhysicallyRemoveRemoteAircraft(callsign); return true;
        }

        // will be added with next snapshot ps_recalculateRenderedAircraft
        return false;
    }

    void CSimulatorCommon::setNewPluginInfo(const CSimulatorPluginInfo &info, const CAircraftModel &defaultModel)
    {
        m_simulatorPluginInfo = info;
        m_defaultModel = defaultModel;
    }

    int CSimulatorCommon::maxAirportsInRange() const
    {
        // might change in future or become a setting or such
        return 20;
    }

    void CSimulatorCommon::blinkHighlightedAircraft()
    {
        if (m_highlightedAircraft.isEmpty() || m_highlightEndTimeMsEpoch < 1) { return; }
        m_blinkCycle = !m_blinkCycle;

        if (QDateTime::currentMSecsSinceEpoch() < m_highlightEndTimeMsEpoch)
        {
            // blink mode, toggle aircraft
            for (const CSimulatedAircraft &aircraft : as_const(m_highlightedAircraft))
            {
                if (m_blinkCycle)
                {
                    this->callPhysicallyRemoveRemoteAircraft(aircraft.getCallsign());
                }
                else
                {
                    this->callPhysicallyAddRemoteAircraft(aircraft); // blink
                }
            }
        }
        else
        {
            // restore
            for (const CSimulatedAircraft &aircraft : as_const(m_highlightedAircraft))
            {
                // get the current state for this aircraft
                // it might has been removed in the meantime
                const CCallsign cs(aircraft.getCallsign());
                this->resetAircraftFromProvider(cs);
            }
            m_highlightedAircraft.clear();
            m_highlightEndTimeMsEpoch = 0;
        }
    }

    void CSimulatorCommon::resetAircraftFromProvider(const CCallsign &callsign)
    {
        const CSimulatedAircraft aircraft(this->getAircraftInRangeForCallsign(callsign));
        const bool enabled = aircraft.isEnabled();
        if (enabled)
        {
            // are we already visible?
            if (!this->isPhysicallyRenderedAircraft(callsign))
            {
                this->callPhysicallyAddRemoteAircraft(aircraft); // enable/disable
            }
        }
        else
        {
            this->callPhysicallyRemoveRemoteAircraft(callsign);
        }
    }

    void CSimulatorCommon::reloadWeatherSettings()
    {
        if (!m_isWeatherActivated) { return; }
        const auto selectedWeatherScenario = m_weatherScenarioSettings.get();
        if (!CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
        {
            m_lastWeatherPosition = {};
            this->injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
        }
    }

    void CSimulatorCommon::reverseLookupAndUpdateOwnAircraftModel(const QString &modelString)
    {
        CAircraftModel model = getOwnAircraftModel();
        model.setModelString(modelString);
        this->reverseLookupAndUpdateOwnAircraftModel(model);
    }

    bool CSimulatorCommon::parseDetails(const CSimpleCommandParser &parser)
    {
        Q_UNUSED(parser);
        return false;
    }

    void CSimulatorCommon::debugLogMessage(const QString &msg) const
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        CLogMessage(this).info(msg);
    }

    void CSimulatorCommon::debugLogMessage(const QString &funcInfo, const QString &msg) const
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        CLogMessage(this).info("%1 %2") << msg << funcInfo;
    }

    bool CSimulatorCommon::showDebugLogMessage() const
    {
        return m_interpolationRenderingSetup.showSimulatorDebugMessages();
    }

    void CSimulatorCommon::reverseLookupAndUpdateOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model)
    {
        Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing sApp");
        Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");

        if (!model.hasModelString()) { return; }
        if (this->getOwnAircraftModel() != model)
        {
            if (CDatabaseUtils::hasDbAircraftData())
            {
                const CAircraftModel newModel = reverseLookupModel(model);
                const bool updated = this->updateOwnModel(newModel); // update in provider (normally the context)
                if (updated)
                {
                    emit this->ownAircraftModelChanged(this->getOwnAircraftModel());
                }
            }
            else
            {
                // we wait for the data
                connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbModelMatchingEntitiesRead, this, [ = ]
                {
                    this->reverseLookupAndUpdateOwnAircraftModel(model);
                });
            }
        }
    }

    CAirportList CSimulatorCommon::getAirportsInRange() const
    {
        // default implementation
        if (!sApp || !sApp->hasWebDataServices()) { return CAirportList(); }
        if (sApp->isShuttingDown()) { return CAirportList(); }

        CAirportList airports = sApp->getWebDataServices()->getAirports();
        if (airports.isEmpty()) { return airports; }
        const CCoordinateGeodetic ownPosition = this->getOwnAircraftPosition();
        airports = airports.findClosest(maxAirportsInRange(), ownPosition);
        if (m_autoCalcAirportDistance) { airports.calculcateAndUpdateRelativeDistanceAndBearing(ownPosition); }
        return airports;
    }

    void CSimulatorCommon::setWeatherActivated(bool activated)
    {
        m_isWeatherActivated = activated;
        if (m_isWeatherActivated)
        {
            const auto selectedWeatherScenario = m_weatherScenarioSettings.get();
            if (!CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
            {
                m_lastWeatherPosition = {};
                this->injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
            }
        }
    }

    CAircraftModel CSimulatorCommon::reverseLookupModel(const CAircraftModel &model)
    {
        bool modified = false;
        const CAircraftModel reverseModel = CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
        return reverseModel;
    }

    void CSimulatorCommon::onSwiftDbAllDataRead()
    {
        // void, can be overridden in specialized drivers
    }

    void CSimulatorCommon::onSwiftDbModelMatchingEntitiesRead()
    {
        // void, can be overridden in specialized drivers
    }

    void CSimulatorCommon::onSwiftDbAirportsRead()
    {
        // void, can be overridden in specialized drivers
    }

    CAircraftModel CSimulatorCommon::getDefaultModel() const
    {
        return m_defaultModel;
    }

    const CSimulatorPluginInfo &CSimulatorCommon::getSimulatorPluginInfo() const
    {
        return m_simulatorPluginInfo;
    }

    const CSimulatorInternals &CSimulatorCommon::getSimulatorInternals() const
    {
        return m_simulatorInternals;
    }

    void CSimulatorCommon::unload()
    {
        this->disconnectFrom(); // disconnect from simulator
        m_remoteAircraftProviderConnections.disconnectAll(); // disconnect signals from provider
    }

    bool CSimulatorCommon::isShuttingDown() const
    {
        return (!sApp || sApp->isShuttingDown());
    }

    void CSimulatorCommon::setInterpolationAndRenderingSetup(const CInterpolationAndRenderingSetup &setup)
    {
        {
            QWriteLocker lock(&m_interpolationRenderingSetupMutex);
            if (m_interpolationRenderingSetup == setup) { return; }
            m_interpolationRenderingSetup = setup;
        }

        const bool r = setup.isRenderingRestricted();
        const bool e = setup.isRenderingEnabled();

        emit renderRestrictionsChanged(r, e, setup.getMaxRenderedAircraft(), setup.getMaxRenderedDistance());
    }

    CInterpolationAndRenderingSetup CSimulatorCommon::getInterpolationAndRenderingSetup() const
    {
        QReadLocker lock(&m_interpolationRenderingSetupMutex);
        return m_interpolationRenderingSetup;
    }

    void CSimulatorCommon::highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime)
    {
        const CCallsign cs(aircraftToHighlight.getCallsign());
        m_highlightedAircraft.removeByCallsign(cs);
        if (enableHighlight)
        {
            const qint64 deltaT = displayTime.valueRounded(CTimeUnit::ms(), 0);
            m_highlightEndTimeMsEpoch = QDateTime::currentMSecsSinceEpoch() + deltaT;
            m_highlightedAircraft.push_back(aircraftToHighlight);
        }
    }

    int CSimulatorCommon::physicallyRemoveMultipleRemoteAircraft(const CCallsignSet &callsigns)
    {
        int removed = 0;
        for (const CCallsign &callsign : callsigns)
        {
            this->callPhysicallyRemoveRemoteAircraft(callsign);
            removed++;
        }
        return removed;
    }

    int CSimulatorCommon::physicallyRemoveAllRemoteAircraft()
    {
        // a default implementation, but normally overridden by the sims
        const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();
        return this->physicallyRemoveMultipleRemoteAircraft(callsigns);
    }

    bool CSimulatorCommon::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        if (this->isMyIdentifier(originator)) { return false; }
        if (commandLine.isEmpty()) { return false; }
        CSimpleCommandParser parser({ ".plugin", ".drv", ".driver" });
        parser.parse(commandLine);
        if (!parser.isKnownCommand()) { return false; }

        // .plugin unload
        if (parser.matchesPart(1, "unload")) { this->unload(); return true; }

        // .plugin log interpolator
        const QString part1(parser.part(1).toLower().trimmed());
        if (part1.startsWith("logint") && parser.hasPart(2))
        {
            const QString part2 = parser.part(2).toLower();
            if (part2 == "off" || part2 == "false")
            {
                CStatusMessage(this).info("Disabled interpolation logging");
                QWriteLocker l(&m_interpolationRenderingSetupMutex);
                m_interpolationRenderingSetup.clearInterpolatorLogCallsigns();
                return true;
            }
            if (part2 == "clear" || part2 == "clr")
            {
                m_interpolationLogger.clearLog();
                CStatusMessage(this).info("Cleared interpolation logging");
                return true;
            }
            if (part2 == "write" || part2 == "save")
            {
                // stop logging of other log
                {
                    QWriteLocker l(&m_interpolationRenderingSetupMutex);
                    m_interpolationRenderingSetup.clearInterpolatorLogCallsigns();
                }

                // write
                m_interpolationLogger.writeLogInBackground();
                CLogMessage(this).info("Started writing interpolation log");
                return true;
            }
            if (part2 == "show")
            {
                QDir dir(CInterpolationLogger::getLogDirectory());
                if (dir.exists())
                {
                    const QUrl dirUrl = QUrl::fromLocalFile(dir.absolutePath());
                    QDesktopServices::openUrl(dirUrl); // show dir in browser
                }
                else
                {
                    CLogMessage(this).warning("No interpolation log directory");
                }
                return true;
            }

            const QString cs = part2.toUpper();
            if (!CCallsign::isValidAircraftCallsign(cs)) { return false; }
            if (this->getAircraftInRangeCallsigns().contains(cs))
            {
                CLogMessage(this).info("Will log interpolation for '%1'") << cs;
                QWriteLocker l(&m_interpolationRenderingSetupMutex);
                m_interpolationRenderingSetup.addCallsignToLog(CCallsign(cs));
                return true;
            }
            else
            {
                CLogMessage(this).warning("Cannot log interpolation for '%1', no aircraft in range") << cs;
                return false;
            }
        }

        if (part1.startsWith("spline") || part1.startsWith("linear"))
        {
            const CCallsign cs(parser.hasPart(2) ? parser.part(2) : "");
            const bool changed = this->setInterpolatorMode(CInterpolatorMulti::modeFromString(part1), cs);
            if (changed)
            {
                CLogMessage(this).info("Changed interpolation mode");
            }
            else
            {
                CLogMessage(this).info("Unchanged interpolation mode");
            }
            return true;
        }

        // driver specific cmd line arguments
        return this->parseDetails(parser);
    }

    void CSimulatorCommon::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackCore::CSimulatorCommon")) { return; }
        CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
        CSimpleCommandParser::registerCommand({".drv logint callsign", "log interpolator for callsign"});
        CSimpleCommandParser::registerCommand({".drv logint off", "no log information for interpolator"});
        CSimpleCommandParser::registerCommand({".drv logint write", "write interpolator log to file"});
        CSimpleCommandParser::registerCommand({".drv logint clear", "clear current log"});
        CSimpleCommandParser::registerCommand({".drv spline|linear <callsign>", "set spline/linear interpolator for one/all callsign(s)"});
    }

    void CSimulatorCommon::resetStatistics()
    {
        m_statsPhysicallyAddedAircraft = 0;
        m_statsPhysicallyRemovedAircraft = 0;
        m_statsPartsAdded = 0;
        m_statsSituationAdded = 0;
    }

    void CSimulatorCommon::oneSecondTimerTimeout()
    {
        this->blinkHighlightedAircraft();
    }

    void CSimulatorCommon::onRecalculatedRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!snapshot.isValidSnapshot()) { return;}

        // for unrestricted values all add/remove actions are directly linked
        // when changing back from restricted->unrestricted an one time update is required
        if (!snapshot.isRestricted() && !snapshot.isRestrictionChanged()) { return; }

        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(this), Q_FUNC_INFO, "Needs to run in object thread");
        Q_ASSERT_X(snapshot.generatingThreadName() != QThread::currentThread()->objectName(), Q_FUNC_INFO, "Expect snapshot from background thread");

        // restricted snapshot values?
        bool changed = false;
        if (snapshot.isRenderingEnabled())
        {
            const CCallsignSet callsignsInSimulator(this->physicallyRenderedAircraft()); // state in simulator
            const CCallsignSet callsignsToBeRemoved(callsignsInSimulator.difference(snapshot.getEnabledAircraftCallsignsByDistance()));
            const CCallsignSet callsignsToBeAdded(snapshot.getEnabledAircraftCallsignsByDistance().difference(callsignsInSimulator));
            if (!callsignsToBeRemoved.isEmpty())
            {
                const int r = this->physicallyRemoveMultipleRemoteAircraft(callsignsToBeRemoved);
                changed = r > 0;
            }

            if (!callsignsToBeAdded.isEmpty())
            {
                CSimulatedAircraftList aircraftToBeAdded(this->getAircraftInRange().findByCallsigns(callsignsToBeAdded)); // thread safe copy
                for (const CSimulatedAircraft &aircraft : aircraftToBeAdded)
                {
                    Q_ASSERT_X(aircraft.isEnabled(), Q_FUNC_INFO, "Disabled aircraft detected as to be added");
                    Q_ASSERT_X(aircraft.hasModelString(), Q_FUNC_INFO, "Missing model string");
                    this->callPhysicallyAddRemoteAircraft(aircraft); // recalcuate snapshot
                    changed = true;
                }
            }
        }
        else
        {
            // no rendering at all, we remove everything
            const int r = this->physicallyRemoveAllRemoteAircraft();
            changed = r > 0;
        }

        // we have handled snapshot
        if (changed)
        {
            emit this->airspaceSnapshotHandled();
        }
    }

    void CSimulatorCommon::onRemoteProviderAddedAircraftSituation(const CAircraftSituation &situation)
    {
        Q_UNUSED(situation);
    }

    void CSimulatorCommon::onRemoteProviderAddedAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const CAircraftParts &parts)
    {
        Q_UNUSED(callsign);
        Q_UNUSED(parts);
    }

    void CSimulatorCommon::reset()
    {
        m_statsUpdateAircraftCountMs = 0;
        m_statsUpdateAircraftTimeAvgMs = 0;
        m_statsUpdateAircraftTimeTotalMs = 0;
        m_blinkCycle = false;
        m_highlightEndTimeMsEpoch = false;
        this->resetStatistics();
        this->clearAllAircraft();
    }

    void CSimulatorCommon::clearAllAircraft()
    {
        m_addAgainAircraftWhenRemoved.clear();
        m_highlightedAircraft.clear();
        m_callsignsToBeRendered.clear();
        m_hints.clear();
    }

    CAirportList CSimulatorCommon::getWebServiceAirports() const
    {
        if (!sApp->hasWebDataServices()) { return CAirportList(); }
        return sApp->getWebDataServices()->getAirports();
    }

    CAirport CSimulatorCommon::getWebServiceAirport(const CAirportIcaoCode &icao) const
    {
        if (!sApp->hasWebDataServices()) { return CAirport(); }
        return sApp->getWebDataServices()->getAirports().findFirstByIcao(icao);
    }

    void CSimulatorCommon::rapOnRecalculatedRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!this->isConnected()) return;
        this->onRecalculatedRenderedAircraft(snapshot);
    }

    void CSimulatorCommon::rapOnRemoteProviderAddedAircraftSituation(const CAircraftSituation &situation)
    {
        if (!this->isConnected()) return;
        m_statsSituationAdded++;
        this->onRemoteProviderAddedAircraftSituation(situation);
    }

    void CSimulatorCommon::rapOnRemoteProviderAddedAircraftParts(const CCallsign &callsign, const CAircraftParts &parts)
    {
        if (!this->isConnected()) return;
        m_statsPartsAdded++;
        this->onRemoteProviderAddedAircraftParts(callsign, parts);
    }

    void CSimulatorCommon::rapOnRemoteProviderRemovedAircraft(const CCallsign &callsign)
    {
        Q_UNUSED(callsign);
        // currently not used, the calls are handled by context call logicallyRemoveRemoteAircraft
    }

    void CSimulatorCommon::callPhysicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        m_statsPhysicallyAddedAircraft++;
        this->physicallyAddRemoteAircraft(remoteAircraft);
    }

    void CSimulatorCommon::callPhysicallyRemoveRemoteAircraft(const CCallsign &remoteCallsign)
    {
        m_statsPhysicallyRemovedAircraft++;
        this->physicallyRemoveRemoteAircraft(remoteCallsign);
    }
} // namespace

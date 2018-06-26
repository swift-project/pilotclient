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
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/simplecommandparser.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/threadutils.h"
#include "blackconfig/buildconfig.h"

#include <QDateTime>
#include <QString>
#include <QStringBuilder>
#include <QThread>
#include <QDir>
#include <QUrl>
#include <QDesktopServices>
#include <functional>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;
using namespace BlackCore::Db;

namespace BlackCore
{
    CSimulatorCommon::CSimulatorCommon(const CSimulatorPluginInfo &info,
                                       IOwnAircraftProvider *ownAircraftProvider,
                                       IRemoteAircraftProvider *remoteAircraftProvider,
                                       IWeatherGridProvider *weatherGridProvider,
                                       IClientProvider *clientProvider,
                                       QObject *parent)
        : ISimulator(info, ownAircraftProvider, remoteAircraftProvider, weatherGridProvider, clientProvider, parent)
    {
        this->setObjectName("Simulator: " + info.getIdentifier());
        CSimulatorCommon::registerHelp();

        // provider signals, hook up with remote aircraft provider
        m_remoteAircraftProviderConnections.append(
            CRemoteAircraftAware::provider()->connectRemoteAircraftProviderSignals(
                this, // receiver must match object in bind
                nullptr,
                nullptr,
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
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead, this, &CSimulatorCommon::onSwiftDbAllDataRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAirportsRead, this, &CSimulatorCommon::onSwiftDbAirportsRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbModelMatchingEntitiesRead, this, &CSimulatorCommon::onSwiftDbModelMatchingEntitiesRead, Qt::QueuedConnection);
        }

        connect(sApp, &CApplication::aboutToShutdown, this, &CSimulatorCommon::unload, Qt::QueuedConnection);

        // info
        CLogMessage(this).info("Initialized simulator driver: '%1'") << this->getSimulatorInfo().toQString();
    }

    CSimulatorCommon::~CSimulatorCommon()
    {
        this->safeKillTimer();
    }

    const CLogCategoryList &CSimulatorCommon::getLogCategories()
    {
        static const CLogCategoryList cats({ CLogCategory::driver(), CLogCategory::plugin() });
        return cats;
    }

    bool CSimulatorCommon::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        Q_ASSERT_X(remoteAircraft.hasModelString(), Q_FUNC_INFO, "Missing model string");
        Q_ASSERT_X(remoteAircraft.hasCallsign(), Q_FUNC_INFO, "Missing callsign");

        const bool renderingRestricted = this->getInterpolationSetupGlobal().isRenderingRestricted();
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
        if (!this->getInterpolationSetupGlobal().isRenderingRestricted())
        {
            m_statsPhysicallyAddedAircraft++;
            this->callPhysicallyRemoveRemoteAircraft(callsign); return true;
        }

        // will be added with next snapshot onRecalculatedRenderedAircraft
        return false;
    }

    int CSimulatorCommon::maxAirportsInRange() const
    {
        // might change in future or become a setting or such
        return 20;
    }

    void CSimulatorCommon::blinkHighlightedAircraft()
    {
        if (m_highlightedAircraft.isEmpty() || m_highlightEndTimeMsEpoch < 1) { return; }
        if (this->isShuttingDown()) { return; }
        m_blinkCycle = !m_blinkCycle;

        if (QDateTime::currentMSecsSinceEpoch() > m_highlightEndTimeMsEpoch)
        {
            this->stopHighlighting();
            return;
        }

        // blink mode, toggle aircraft
        for (const CSimulatedAircraft &aircraft : as_const(m_highlightedAircraft))
        {
            if (m_blinkCycle) { this->callPhysicallyRemoveRemoteAircraft(aircraft.getCallsign()); }
            else { this->callPhysicallyAddRemoteAircraft(aircraft);  }
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

    void CSimulatorCommon::debugLogMessage(const QString &msg)
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        const CStatusMessage m = CStatusMessage(this).info("%1") << msg;
        emit this->driverMessages(m);
    }

    void CSimulatorCommon::debugLogMessage(const QString &funcInfo, const QString &msg)
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        const CStatusMessage m = CStatusMessage(this).info("%1 %2") << msg << funcInfo;
        emit this->driverMessages(m);
    }

    bool CSimulatorCommon::showDebugLogMessage() const
    {
        const bool show = this->getInterpolationSetupGlobal().showSimulatorDebugMessages();
        return show;
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
                const CAircraftModel newModel = this->reverseLookupModel(model);
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
        if (this->isShuttingDown()) { return CAirportList(); }
        if (!sApp || !sApp->hasWebDataServices()) { return CAirportList(); }

        const CAirportList airports = sApp->getWebDataServices()->getAirports();
        if (airports.isEmpty()) { return airports; }
        const CCoordinateGeodetic ownPosition = this->getOwnAircraftPosition();
        CAirportList airportInRange = airports.findClosest(maxAirportsInRange(), ownPosition);
        if (m_autoCalcAirportDistance) { airportInRange.calculcateAndUpdateRelativeDistanceAndBearing(ownPosition); }
        return airportInRange;
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

    bool CSimulatorCommon::isUpdateAircraftLimited(qint64 timestamp)
    {
        if (!m_limitUpdateAircraft) { return false; }
        const bool hasToken = m_limitUpdateAircraftBucket.tryConsume(1, timestamp);
        return !hasToken;
    }

    bool CSimulatorCommon::isUpdateAircraftLimitedWithStats(qint64 startTime)
    {
        const bool limited = this->isUpdateAircraftLimited(startTime);
        this->setStatsRemoteAircraftUpdate(startTime, limited);
        return limited;
    }

    bool CSimulatorCommon::limitToUpdatesPerSecond(int numberPerSecond)
    {
        if (numberPerSecond < 1)
        {
            m_limitUpdateAircraft = false;
            return false;
        }

        int tokens = 0.1 * numberPerSecond; // 100ms
        do
        {
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(100); break; }
            tokens = 0.25 * numberPerSecond; // 250ms
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(250); break; }
            tokens = 0.5 * numberPerSecond; // 500ms
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(500); break; }
            tokens = numberPerSecond;
            m_limitUpdateAircraftBucket.setInterval(1000);
        }
        while (false);

        m_limitUpdateAircraftBucket.setCapacityAndTokensToRefill(tokens);
        m_limitUpdateAircraft = true;
        return true;
    }

    QString CSimulatorCommon::updateAircraftLimitationInfo() const
    {
        if (!m_limitUpdateAircraft) { return QStringLiteral("not limited"); }
        static const QString limInfo("Limited %1 times with %2/secs.");
        return limInfo.arg(m_statsUpdateAircraftLimited).arg(m_limitUpdateAircraftBucket.getTokensPerSecond());
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

    const CSimulatorInternals &CSimulatorCommon::getSimulatorInternals() const
    {
        return m_simulatorInternals;
    }

    void CSimulatorCommon::unload()
    {
        this->disconnectFrom(); // disconnect from simulator
        m_remoteAircraftProviderConnections.disconnectAll(); // disconnect signals from provider
    }

    bool CSimulatorCommon::disconnectFrom()
    {
        // supposed to be overridden
        return true;
    }

    bool CSimulatorCommon::logicallyReAddRemoteAircraft(const CCallsign &callsign)
    {
        if (this->isShuttingDown()) { return false; }
        if (callsign.isEmpty()) { return false; }
        this->stopHighlighting();
        this->logicallyRemoveRemoteAircraft(callsign);
        if (!this->isAircraftInRange(callsign)) { return false; }
        const QPointer<CSimulatorCommon> myself(this);
        QTimer::singleShot(2500, this, [ = ]
        {
            if (myself.isNull()) { return; }
            if (this->isShuttingDown()) { return; }
            if (!this->isAircraftInRange(callsign)) { return; }
            const CSimulatedAircraft aircraft = this->getAircraftInRangeForCallsign(callsign);
            if (aircraft.isEnabled() && aircraft.hasModelString())
            {
                this->logicallyAddRemoteAircraft(aircraft);
            }
        });
        return true;
    }

    CCallsignSet CSimulatorCommon::unrenderedEnabledAircraft() const
    {
        const CSimulatedAircraftList aircraft = this->getAircraftInRange().findByEnabled(true);
        if (aircraft.isEmpty()) { return CCallsignSet(); }
        CCallsignSet enabledOnes = aircraft.getCallsigns();
        const CCallsignSet renderedOnes = this->physicallyRenderedAircraft();
        enabledOnes.remove(renderedOnes);
        return enabledOnes;
    }

    CCallsignSet CSimulatorCommon::renderedDisabledAircraft() const
    {
        const CSimulatedAircraftList aircraft = this->getAircraftInRange().findByEnabled(false);
        if (aircraft.isEmpty()) { return CCallsignSet(); }
        const CCallsignSet disabledOnes = aircraft.getCallsigns();
        const CCallsignSet renderedOnes = this->physicallyRenderedAircraft();
        return renderedOnes.intersection(disabledOnes);
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
        if (callsigns.isEmpty()) { return 0; }
        this->stopHighlighting();
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
        const int r = this->physicallyRemoveMultipleRemoteAircraft(callsigns);
        this->debugVerifyStateAfterAllAircraftRemoved();
        this->clearAllRemoteAircraftData();
        return r;
    }

    bool CSimulatorCommon::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
    {
        if (this->isShuttingDown()) { return false; }
        return aircraft.isEnabled() ?
               this->physicallyAddRemoteAircraft(aircraft) :
               this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
    }

    bool CSimulatorCommon::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
    {
        // we expect the new model "in aircraft"
        // remove upfront, and then enable / disable again
        if (this->isShuttingDown()) { return false; }
        const CCallsign callsign = aircraft.getCallsign();
        if (!this->isPhysicallyRenderedAircraft(callsign)) { return false; }
        this->physicallyRemoveRemoteAircraft(callsign);
        return this->changeRemoteAircraftEnabled(aircraft);
    }

    bool CSimulatorCommon::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
    {
        if (this->isMyIdentifier(originator)) { return false; }
        if (this->isShuttingDown()) { return false; }

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
                this->clearInterpolationLogCallsigns();
                return true;
            }
            if (part2 == "clear" || part2 == "clr")
            {
                m_interpolationLogger.clearLog();
                CStatusMessage(this).info("Cleared interpolation logging");
                this->clearInterpolationLogCallsigns();
                return true;
            }
            if (part2.startsWith("max"))
            {
                if (!parser.hasPart(3)) { return false; }
                bool ok;
                const int max = parser.part(3).toInt(&ok);
                if (!ok) { return false; }
                m_interpolationLogger.setMaxSituations(max);
                CStatusMessage(this).info("Max.situations logged: %1") << max;
                return true;
            }
            if (part2 == "write" || part2 == "save")
            {
                // stop logging of other log
                this->clearInterpolationLogCallsigns();

                // write
                m_interpolationLogger.writeLogInBackground();
                CLogMessage(this).info("Started writing interpolation log");
                return true;
            }
            if (part2 == "show")
            {
                const QDir dir(CInterpolationLogger::getLogDirectory());
                if (CDirectoryUtils::isDirExisting(dir))
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

            const CCallsign cs(part2.toUpper());
            if (!cs.isValid()) { return false; }
            if (this->getAircraftInRangeCallsigns().contains(cs))
            {
                CLogMessage(this).info("Will log interpolation for '%1'") << cs.asString();
                this->setLogCallsign(true, cs);
                return true;
            }
            else
            {
                CLogMessage(this).warning("Cannot log interpolation for '%1', no aircraft in range") << cs.asString();
                return false;
            }
        } // logint

        if (part1.startsWith("spline") || part1.startsWith("linear"))
        {
            if (parser.hasPart(2))
            {
                const CCallsign cs(parser.part(2));
                const bool changed = this->setInterpolationMode(part1, cs);
                CLogMessage(this).info(changed ? "Changed interpolation mode for '%1'" : "Unchanged interpolation mode for '%1'") << cs.asString();
                return true;
            }
            else
            {
                CInterpolationAndRenderingSetupGlobal setup = this->getInterpolationSetupGlobal();
                const bool changed = setup.setInterpolatorMode(part1);
                if (changed) { this->setInterpolationSetupGlobal(setup); }
                CLogMessage(this).info(changed ? "Changed interpolation mode globally" : "Unchanged interpolation mode");
                return true;
            }
        } // spline/linear

        if (part1.startsWith("pos"))
        {
            CCallsign cs(parser.part(2).toUpper());
            if (!cs.isValid())
            {
                const CCallsignSet csSet = this->getLogCallsigns();
                if (csSet.size() != 1) { return false; }

                // if there is just one we take that one
                cs = *csSet.begin();
            }

            this->setLogCallsign(true, cs);
            CLogMessage(this).info("Display position for '%1'") << cs.asString();
            this->displayLoggedSituationInSimulator(cs, true);
            return true;
        }

        if (parser.hasPart(2) && (part1.startsWith("aircraft") || part1.startsWith("ac")))
        {
            const QString part2 = parser.part(2).toLower();
            if (parser.hasPart(3) && (part2.startsWith("readd") || part2.startsWith("re-add")))
            {
                const QString cs = parser.part(3).toUpper();
                if (cs == "all")
                {
                    this->physicallyRemoveAllRemoteAircraft();
                    const CStatusMessageList msgs = this->debugVerifyStateAfterAllAircraftRemoved();
                    this->clearAllRemoteAircraftData();
                    if (!msgs.isEmpty()) { emit this->driverMessages(msgs); }
                    const CSimulatedAircraftList aircraft = this->getAircraftInRange();
                    for (const CSimulatedAircraft a : aircraft)
                    {
                        if (a.isEnabled()) { this->logicallyAddRemoteAircraft(a); }
                    }
                }
                else if (CCallsign::isValidAircraftCallsign(cs))
                {
                    this->logicallyReAddRemoteAircraft(cs);
                    return true;
                }
                return false;
            }
            if (parser.hasPart(3) && (part2.startsWith("rm") || part2.startsWith("remove")))
            {
                const QString cs = parser.part(3).toUpper();
                if (CCallsign::isValidAircraftCallsign(cs))
                {
                    this->logicallyRemoveRemoteAircraft(cs);
                }
            }

            return false;
        }

        if (part1.startsWith("limit"))
        {
            const int perSecond = parser.toInt(2, -1);
            this->limitToUpdatesPerSecond(perSecond);
            CLogMessage(this).info("Remote aircraft updates limitations: %1") << this->updateAircraftLimitationInfo();
            return true;
        }

        // driver specific cmd line arguments
        return this->parseDetails(parser);
    }

    void CSimulatorCommon::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackCore::CSimulatorCommon")) { return; }
        CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
        CSimpleCommandParser::registerCommand({".drv limit number/secs.", "limit updates to number per second (0..off)"});
        CSimpleCommandParser::registerCommand({".drv logint callsign", "log interpolator for callsign"});
        CSimpleCommandParser::registerCommand({".drv logint off", "no log information for interpolator"});
        CSimpleCommandParser::registerCommand({".drv logint write", "write interpolator log to file"});
        CSimpleCommandParser::registerCommand({".drv logint clear", "clear current log"});
        CSimpleCommandParser::registerCommand({".drv logint max number", "max. number of entries logged"});
        CSimpleCommandParser::registerCommand({".drv pos callsign", "show position for callsign"});
        CSimpleCommandParser::registerCommand({".drv spline|linear callsign", "set spline/linear interpolator for one/all callsign(s)"});
        CSimpleCommandParser::registerCommand({".drv aircraft readd callsign", "add again (re-add) a given callsign"});
        CSimpleCommandParser::registerCommand({".drv aircraft readd all", "add again (re-add) all aircraft"});
        CSimpleCommandParser::registerCommand({".drv aircraft rm callsign", "remove a given callsign"});
    }

    void CSimulatorCommon::resetAircraftStatistics()
    {
        m_statsUpdateAircraftRuns = 0;
        m_statsUpdateAircraftTimeAvgMs = 0;
        m_statsUpdateAircraftTimeTotalMs = 0;
        m_statsMaxUpdateTimeMs = 0;
        m_statsCurrentUpdateTimeMs = 0;
        m_statsPhysicallyAddedAircraft = 0;
        m_statsPhysicallyRemovedAircraft = 0;
        m_statsLastUpdateAircraftRequestedMs = 0;
        m_statsUpdateAircraftRequestedDeltaMs = 0;
        m_statsUpdateAircraftLimited = 0;
    }

    CStatusMessageList CSimulatorCommon::debugVerifyStateAfterAllAircraftRemoved() const
    {
        CStatusMessageList msgs;
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return msgs; }
        if (!m_addAgainAircraftWhenRemoved.isEmpty()) { msgs.push_back(CStatusMessage(this).error("m_addAgainAircraftWhenRemoved not empty: '%1'") << m_addAgainAircraftWhenRemoved.getCallsignStrings(true).join(", ")); }
        return msgs;
    }

    void CSimulatorCommon::oneSecondTimerTimeout()
    {
        this->blinkHighlightedAircraft();
    }

    void CSimulatorCommon::safeKillTimer()
    {
        if (m_timerId < 0) { return; }
        this->killTimer(m_timerId);
        m_timerId = -1;
    }

    QString CSimulatorCommon::getInvalidSituationLogMessage(const CCallsign &callsign, const CInterpolationStatus &status, const QString &details) const
    {
        static const QString msg("CS: '%1' Interpolation: '%2'");
        const QString m = msg.arg(callsign.asString(), status.toQString());
        if (details.isEmpty()) { return m; }

        static const QString addDetails(" details: '%1'");
        return m + addDetails.arg(details);
    }

    bool CSimulatorCommon::clampedLog(const CCallsign &callsign, const CStatusMessage &message)
    {
        if (message.isEmpty()) { return false; }
        constexpr qint64 Timeout = 2000;
        const qint64 clampTs = m_clampedLogMsg.value(callsign, -1);
        const qint64 ts = QDateTime::currentMSecsSinceEpoch();
        if (clampTs > 0 && ((clampTs + Timeout) > ts)) { return false; }
        CLogMessage::preformatted(message);
        m_clampedLogMsg[callsign] = ts;
        return true;
    }

    void CSimulatorCommon::removedClampedLog(const CCallsign &callsign)
    {
        m_clampedLogMsg.remove(callsign);
    }

    void CSimulatorCommon::setStatsRemoteAircraftUpdate(qint64 startTime, bool limited)
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const qint64 dt = now - startTime;
        m_statsCurrentUpdateTimeMs = dt;
        m_statsUpdateAircraftTimeTotalMs += dt;
        m_statsUpdateAircraftRuns++;
        m_statsUpdateAircraftTimeAvgMs = static_cast<double>(m_statsUpdateAircraftTimeTotalMs) / static_cast<double>(m_statsUpdateAircraftRuns);
        m_updateRemoteAircraftInProgress = false;
        m_statsLastUpdateAircraftRequestedMs = startTime;

        if (m_statsMaxUpdateTimeMs < dt) { m_statsMaxUpdateTimeMs = dt; }
        if (m_statsLastUpdateAircraftRequestedMs > 0) { m_statsUpdateAircraftRequestedDeltaMs = startTime - m_statsLastUpdateAircraftRequestedMs; }
        if (limited) { m_statsUpdateAircraftLimited++; }
    }

    bool CSimulatorCommon::isEqualLastSent(const CAircraftSituation &compare) const
    {
        Q_ASSERT_X(compare.hasCallsign(), Q_FUNC_INFO, "Need callsign");
        if (!m_lastSentSituations.contains(compare.getCallsign())) { return false; }
        if (compare.isNull()) { return false; }
        return compare.equalPbhAndVector(m_lastSentSituations.value(compare.getCallsign()));
    }

    bool CSimulatorCommon::isEqualLastSent(const CAircraftParts &compare, const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        if (!m_lastSentParts.contains(callsign)) { return false; }
        return compare.equalValues(m_lastSentParts.value(callsign));
    }

    void CSimulatorCommon::rememberLastSent(const CAircraftSituation &sent)
    {
        Q_ASSERT_X(sent.hasCallsign(), Q_FUNC_INFO, "Need callsign");
        m_lastSentSituations.insert(sent.getCallsign(), sent);
    }

    void CSimulatorCommon::rememberLastSent(const CAircraftParts &sent, const CCallsign &callsign)
    {
        Q_ASSERT_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
        m_lastSentParts.insert(callsign, sent);
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

    void CSimulatorCommon::reset()
    {
        this->clearAllRemoteAircraftData();
    }

    void CSimulatorCommon::resetHighlighting()
    {
        m_highlightedAircraft.clear();
        m_blinkCycle = false;
        m_highlightEndTimeMsEpoch = false;
    }

    void CSimulatorCommon::stopHighlighting()
    {
        // restore
        const CSimulatedAircraftList highlightedAircraft(m_highlightedAircraft);
        for (const CSimulatedAircraft &aircraft : highlightedAircraft)
        {
            // get the current state for this aircraft
            // it might has been removed in the meantime
            const CCallsign cs(aircraft.getCallsign());
            this->resetAircraftFromProvider(cs);
        }
        this->resetHighlighting();
    }

    void CSimulatorCommon::clearAllRemoteAircraftData()
    {
        // rendering related stuff
        m_addAgainAircraftWhenRemoved.clear();
        m_callsignsToBeRendered.clear();
        m_clampedLogMsg.clear();
        m_lastSentParts.clear();
        m_lastSentSituations.clear();
        m_updateRemoteAircraftInProgress = false;

        this->resetHighlighting();
        this->resetAircraftStatistics();
    }

    CAirportList CSimulatorCommon::getWebServiceAirports() const
    {
        if (this->isShuttingDown()) { return CAirportList(); }
        if (!sApp->hasWebDataServices()) { return CAirportList(); }
        return sApp->getWebDataServices()->getAirports();
    }

    CAirport CSimulatorCommon::getWebServiceAirport(const CAirportIcaoCode &icao) const
    {
        if (this->isShuttingDown()) { return CAirport(); }
        if (!sApp->hasWebDataServices()) { return CAirport(); }
        return sApp->getWebDataServices()->getAirports().findFirstByIcao(icao);
    }

    void CSimulatorCommon::rapOnRecalculatedRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!this->isConnected()) { return; }
        if (this->isShuttingDown()) { return; }
        this->onRecalculatedRenderedAircraft(snapshot);
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
        m_clampedLogMsg.clear();
        m_lastSentParts.remove(remoteCallsign);
        m_lastSentSituations.remove(remoteCallsign);
        m_clampedLogMsg.remove(remoteCallsign);
        this->physicallyRemoveRemoteAircraft(remoteCallsign);
    }

    QString CSimulatorCommon::latestLoggedDataFormatted(const CCallsign &cs) const
    {
        const SituationLog s = m_interpolationLogger.getLastSituationLog(cs);
        const PartsLog p = m_interpolationLogger.getLastPartsLog(cs);

        static const QString sep("\n------\n");
        QString dm;
        if (s.tsCurrent > 0)
        {
            dm = QStringLiteral("Setup: ") % s.usedSetup.toQString(true) %
                 QStringLiteral("\n\n") %
                 QStringLiteral("Situation: ") % s.toQString(false, true, true, true, true, sep);
        }
        if (p.tsCurrent > 0) { dm += (dm.isEmpty() ? QStringLiteral("") : QStringLiteral("\n\n")) % QStringLiteral("Parts: ") % p.toQString(sep); }
        return dm;
    }

    void CSimulatorCommon::displayLoggedSituationInSimulator(const CCallsign &cs, bool stopLogging, int times)
    {
        if (cs.isEmpty()) { return; }
        if (this->isShuttingDown()) { return; }
        const CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(cs);
        const bool logsCs = setup.logInterpolation();
        if (!logsCs) { return; }

        stopLogging = stopLogging || !this->isSimulating(); // stop when sim was stopped
        stopLogging = stopLogging && logsCs;
        if (!stopLogging && times < 1) { return; }

        const bool inRange = this->getAircraftInRangeCallsigns().contains(cs);
        if (!stopLogging && !inRange) { return; }
        if (stopLogging && (times < 1 || !inRange))
        {
            this->setLogCallsign(false, cs);
            return;
        }

        const QString dm = this->latestLoggedDataFormatted(cs);
        if (!dm.isEmpty())
        {
            this->displayStatusMessage(CStatusMessage(this).info(dm));
            emit this->requestUiConsoleMessage(dm, true);
        }

        const int t = 4500 + (qrand() % 1000); // makes sure not always using the same time difference
        const QPointer<CSimulatorCommon> myself(this);
        QTimer::singleShot(t, this, [ = ]
        {
            if (myself.isNull() || myself->isShuttingDown()) { return; }
            this->displayLoggedSituationInSimulator(cs, stopLogging, times - 1);
        });
    }
} // namespace

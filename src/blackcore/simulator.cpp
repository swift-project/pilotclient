/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/db/databaseutils.h"
#include "blackcore/simulator.h"
#include "blackcore/webdataservices.h"
#include "blackcore/application.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/crashhandler.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"

#include <QFlag>
#include <Qt>
#include <QtGlobal>
#include <QPointer>
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
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Math;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Simulation::Data;
using namespace BlackMisc::Simulation::Settings;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Weather;
using namespace BlackCore::Db;

namespace BlackCore
{
    const QStringList &ISimulator::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::driver(), CLogCategories::plugin() });
        return cats;
    }

    ISimulator::~ISimulator()
    {
        this->safeKillTimer();
    }

    ISimulator::SimulatorStatus ISimulator::getSimulatorStatus() const
    {
        if (!this->isConnected()) { return ISimulator::Disconnected; }
        const SimulatorStatus status =
            Connected
            | (this->isSimulating() ? ISimulator::Simulating : static_cast<ISimulator::SimulatorStatusFlag>(0))
            | (this->isPaused() ? ISimulator::Paused : static_cast<ISimulator::SimulatorStatusFlag>(0));
        return status;
    }

    bool ISimulator::logicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        // if not restriced, directly change
        if (!this->getInterpolationSetupGlobal().isRenderingRestricted())
        {
            m_statsPhysicallyAddedAircraft++;
            this->callPhysicallyRemoveRemoteAircraft(callsign);
            return true;
        }

        // will be removed with next snapshot onRecalculatedRenderedAircraft
        return false;
    }

    bool ISimulator::logicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        if (!this->validateModelOfAircraft(remoteAircraft))
        {
            const CCallsign cs = remoteAircraft.getCallsign();
            CLogMessage(this).warning(u"Invalid aircraft detected, which will be disabled: '%1' '%2'") << cs << remoteAircraft.getModelString();
            this->updateAircraftEnabled(cs, false);
            this->updateAircraftRendered(cs, false);
            return false;
        }

        // no invalid model should ever reach this place here
        const bool renderingRestricted = this->getInterpolationSetupGlobal().isRenderingRestricted();
        if (this->showDebugLogMessage()) { this->debugLogMessage(Q_FUNC_INFO, QStringLiteral("Restricted: %1 cs: '%2' enabled: %3").arg(boolToYesNo(renderingRestricted), remoteAircraft.getCallsignAsString(), boolToYesNo(remoteAircraft.isEnabled()))); }
        if (!remoteAircraft.isEnabled()) { return false; }

        // if not restriced, directly change
        if (!renderingRestricted)
        {
            this->callPhysicallyAddRemoteAircraft(remoteAircraft);
            return true;
        }

        // restricted -> will be added with next snapshot onRecalculatedRenderedAircraft
        return false;
    }

    void ISimulator::highlightAircraft(const CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const CTime &displayTime)
    {
        const CCallsign cs(aircraftToHighlight.getCallsign());
        m_highlightedAircraft.removeByCallsign(cs);
        if (enableHighlight)
        {
            const qint64 deltaT = displayTime.valueInteger(CTimeUnit::ms());
            m_highlightEndTimeMsEpoch = QDateTime::currentMSecsSinceEpoch() + deltaT;
            m_highlightedAircraft.push_back(aircraftToHighlight);
        }
    }

    bool ISimulator::followAircraft(const CCallsign &callsign)
    {
        Q_UNUSED(callsign)
        return false;
    }

    void ISimulator::recalculateAllAircraft()
    {
        this->setUpdateAllRemoteAircraft();
    }

    bool ISimulator::isWeatherActivated() const
    {
        return m_isWeatherActivated;
    }

    void ISimulator::setWeatherActivated(bool activated)
    {
        m_isWeatherActivated = activated;
        if (m_isWeatherActivated)
        {
            const auto selectedWeatherScenario = m_weatherScenarioSettings.get();
            if (CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
            {
                if (m_lastWeatherPosition.isNull())
                {
                    const CCoordinateGeodetic p = this->getOwnAircraftPosition();
                    if (!p.isNull())
                    {
                        m_lastWeatherPosition = p;
                        this->requestWeatherGrid(p, this->identifier());
                    }
                }
            }
            else
            {
                m_lastWeatherPosition.setNull();
                this->injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
            }
        }
        else
        {
            m_lastWeatherPosition.setNull(); // clean up so next time we fetch weather again
        }
    }

    void ISimulator::setFlightNetworkConnected(bool connected)
    {
        m_networkConnected = connected;
    }

    void ISimulator::reloadWeatherSettings()
    {
        // log crash info about weather
        if (!this->isShuttingDown()) { CCrashHandler::instance()->crashAndLogAppendInfo(u"Simulator weather: " % boolToYesNo(m_isWeatherActivated)); }
        if (!m_isWeatherActivated) { return; }

        m_lastWeatherPosition.setNull();
        const CWeatherScenario selectedWeatherScenario = m_weatherScenarioSettings.get();
        if (CWeatherScenario::isRealWeatherScenario(selectedWeatherScenario))
        {
            if (m_lastWeatherPosition.isNull())
            {
                const CCoordinateGeodetic p = this->getOwnAircraftPosition();
                if (!p.isNull())
                {
                    m_lastWeatherPosition = p;
                    this->requestWeatherGrid(p, this->identifier());
                }
            }
        }
        else
        {
            this->injectWeatherGrid(CWeatherGrid::getByScenario(selectedWeatherScenario));
        }

        // log crash info about weather
        if (!this->isShuttingDown()) { CCrashHandler::instance()->crashAndLogAppendInfo(selectedWeatherScenario.toQString(true)); }
    }

    void ISimulator::clearAllRemoteAircraftData()
    {
        // rendering related stuff
        m_addAgainAircraftWhenRemoved.clear();
        m_callsignsToBeRendered.clear();
        this->resetLastSentValues(); // clear all last sent values
        m_updateRemoteAircraftInProgress = false;

        this->clearInterpolationSetupsPerCallsign();
        this->resetHighlighting();
        this->resetAircraftStatistics();
    }

    void ISimulator::debugLogMessage(const QString &msg)
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        const CStatusMessage m = CStatusMessage(this).info(u"%1") << msg;
        emit this->driverMessages(m);
    }

    void ISimulator::debugLogMessage(const QString &funcInfo, const QString &msg)
    {
        if (!this->showDebugLogMessage()) { return; }
        if (msg.isEmpty()) { return; }
        const CStatusMessage m = CStatusMessage(this).info(u"%1 %2") << msg << funcInfo;
        emit this->driverMessages(m);
    }

    bool ISimulator::showDebugLogMessage() const
    {
        const bool show = this->getInterpolationSetupGlobal().showSimulatorDebugMessages();
        return show;
    }

    void ISimulator::resetAircraftFromProvider(const CCallsign &callsign)
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

    void ISimulator::clearData(const CCallsign &callsign)
    {
        m_highlightedAircraft.removeByCallsign(callsign);
        m_statsPhysicallyRemovedAircraft++;
        m_lastSentParts.remove(callsign);
        m_lastSentSituations.remove(callsign);
        m_loopbackSituations.clear();
        this->removeInterpolationSetupPerCallsign(callsign);
    }

    bool ISimulator::addLoopbackSituation(const CAircraftSituation &situation)
    {
        const CCallsign cs = situation.getCallsign();
        if (!this->isLogCallsign(cs)) { return false; }
        CAircraftSituationList &situations = m_loopbackSituations[cs];
        situations.push_frontKeepLatestAdjustedFirst(situation, true, 10);
        return true;
    }

    bool ISimulator::addLoopbackSituation(const CCallsign &callsign, const CElevationPlane &elevationPlane, const CLength &cg)
    {
        if (!this->isLogCallsign(callsign)) { return false; }
        CAircraftSituation situation(callsign, elevationPlane);
        situation.setGroundElevation(elevationPlane, CAircraftSituation::FromProvider);
        situation.setCG(cg);
        situation.setCurrentUtcTime();
        situation.setTimeOffsetMs(0);
        CAircraftSituationList &situations = m_loopbackSituations[callsign];
        situations.push_frontKeepLatestAdjustedFirst(situation, true, 10);
        return true;
    }

    void ISimulator::reset()
    {
        this->clearAllRemoteAircraftData(); // reset
        m_averageFps = -1.0;
        m_simTimeRatio = 1.0;
        m_trackMilesShort = 0.0;
        m_minutesLate = 0.0;
    }

    bool ISimulator::isUpdateAllRemoteAircraft(qint64 currentTimestamp) const
    {
        if (m_updateAllRemoteAircraftUntil < 1) { return false; }
        if (currentTimestamp < 0) { currentTimestamp = QDateTime::currentMSecsSinceEpoch(); }
        return (m_updateAllRemoteAircraftUntil > currentTimestamp);
    }

    void ISimulator::setUpdateAllRemoteAircraft(qint64 currentTimestamp, qint64 forMs)
    {
        if (currentTimestamp < 0) { currentTimestamp = QDateTime::currentMSecsSinceEpoch(); }
        if (forMs < 0) { forMs = 10 * 1000; }
        m_updateAllRemoteAircraftUntil = currentTimestamp + forMs;
        this->resetLastSentValues();
    }

    void ISimulator::resetUpdateAllRemoteAircraft()
    {
        m_updateAllRemoteAircraftUntil = -1;
    }

    void ISimulator::resetHighlighting()
    {
        m_highlightedAircraft.clear();
        m_blinkCycle = false;
        m_highlightEndTimeMsEpoch = false;
    }

    void ISimulator::stopHighlighting()
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

    void ISimulator::oneSecondTimerTimeout()
    {
        this->blinkHighlightedAircraft();
    }

    void ISimulator::safeKillTimer()
    {
        if (m_timerId < 0) { return; }
        BLACK_AUDIT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Try to kill timer from another thread");
        this->killTimer(m_timerId);
        m_timerId = -1;
    }

    void ISimulator::injectWeatherGrid(const CWeatherGrid &weatherGrid)
    {
        Q_UNUSED(weatherGrid)
    }

    void ISimulator::blinkHighlightedAircraft()
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
        for (const CSimulatedAircraft &aircraft : std::as_const(m_highlightedAircraft))
        {
            if (m_blinkCycle) { this->callPhysicallyRemoveRemoteAircraft(aircraft.getCallsign(), true); }
            else { this->callPhysicallyAddRemoteAircraft(aircraft);  }
        }
    }

    CInterpolationAndRenderingSetupPerCallsign ISimulator::getInterpolationSetupConsolidated(const CCallsign &callsign, bool forceFullUpdate) const
    {
        CInterpolationAndRenderingSetupPerCallsign setup = this->getInterpolationSetupPerCallsignOrDefault(callsign);
        const CClient client = this->getClientOrDefaultForCallsign(callsign);
        setup.consolidateWithClient(client);
        if (forceFullUpdate) { setup.setForceFullInterpolation(forceFullUpdate); }
        return setup;
    }

    bool ISimulator::requestElevation(const ICoordinateGeodetic &reference, const CCallsign &callsign)
    {
        Q_UNUSED(reference)
        Q_UNUSED(callsign)
        return false;
    }

    void ISimulator::callbackReceivedRequestedElevation(const CElevationPlane &plane, const CCallsign &callsign, bool isWater)
    {
        if (this->isShuttingDown()) { return; }
        if (plane.isNull()) { return; } // this happens if requested for a coordinate where scenery is not available

        // Update in remote aircraft for given callsign
        // this will trigger also a position update, new interpolant etc.
        bool updatedForOnGroundPosition = false;
        const int updated = CRemoteAircraftAware::updateAircraftGroundElevation(callsign, plane, CAircraftSituation::FromProvider, &updatedForOnGroundPosition);

        // update in simulator and cache
        const bool likelyOnGroundElevation = updated > 0 && updatedForOnGroundPosition;
        ISimulationEnvironmentProvider::rememberGroundElevation(callsign, likelyOnGroundElevation, plane); // in simulator

        // signal we have received the elevation
        // used by log display
        emit this->receivedRequestedElevation(plane, callsign);
        Q_UNUSED(isWater)
    }

    void ISimulator::resetAircraftStatistics()
    {
        m_statsUpdateAircraftRuns        = 0;
        m_statsUpdateAircraftTimeAvgMs   = 0;
        m_statsUpdateAircraftTimeTotalMs = 0;
        m_statsMaxUpdateTimeMs           = 0;
        m_statsCurrentUpdateTimeMs       = 0;
        m_statsPhysicallyAddedAircraft   = 0;
        m_statsPhysicallyRemovedAircraft = 0;
        m_statsUpdateAircraftLimited     = 0;
        m_statsLastUpdateAircraftRequestedMs  = 0;
        m_statsUpdateAircraftRequestedDeltaMs = 0;
        ISimulationEnvironmentProvider::resetSimulationEnvironmentStatistics();
    }

    bool ISimulator::isEmulatedDriver() const
    {
        const QString className = this->metaObject()->className();
        return className.contains("emulated", Qt::CaseInsensitive);
    }

    bool ISimulator::parseCommandLine(const QString &commandLine, const CIdentifier &originator)
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
                CLogMessage(this).info(u"Disabled interpolation logging");
                this->clearInterpolationLogCallsigns();
                return true;
            }
            if (part2 == "clear" || part2 == "clr")
            {
                m_interpolationLogger.clearLog();
                CLogMessage(this).info(u"Cleared interpolation logging");
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
                CLogMessage(this).info(u"Max.situations logged: %1") << max;
                return true;
            }
            if (part2 == "write" || part2 == "save")
            {
                // stop logging of other log
                this->clearInterpolationLogCallsigns();

                // write
                const bool clearLog = true;
                m_interpolationLogger.writeLogInBackground(clearLog);
                CLogMessage(this).info(u"Started writing interpolation log");
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
                    CLogMessage(this).warning(u"No interpolation log directory");
                }
                return true;
            }

            const CCallsign cs(part2.toUpper());
            if (!cs.isValid()) { return false; }
            if (this->getAircraftInRangeCallsigns().contains(cs))
            {
                CLogMessage(this).info(u"Will log interpolation for '%1'") << cs.asString();
                this->setLogCallsign(true, cs);
                return true;
            }
            else
            {
                CLogMessage(this).warning(u"Cannot log interpolation for '%1', no aircraft in range") << cs.asString();
                return false;
            }
        } // logint

        if (part1.startsWith("spline") || part1.startsWith("linear"))
        {
            if (parser.hasPart(2))
            {
                const CCallsign cs(parser.part(2));
                const bool changed = this->setInterpolationMode(part1, cs);
                CLogMessage(this).info(changed
                                       ? QStringLiteral("Changed interpolation mode for '%1'")
                                       : QStringLiteral("Unchanged interpolation mode for '%1'")) << cs.asString();
                return true;
            }
            else
            {
                CInterpolationAndRenderingSetupGlobal setup = this->getInterpolationSetupGlobal();
                const bool changed = setup.setInterpolatorMode(part1);
                if (changed) { this->setInterpolationSetupGlobal(setup); }
                CLogMessage(this).info(changed
                                       ? QStringLiteral("Changed interpolation mode globally")
                                       : QStringLiteral("Unchanged interpolation mode"));
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
            CLogMessage(this).info(u"Display position for '%1'") << cs.asString();
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
                    this->clearAllRemoteAircraftData(); // "dot command"
                    if (!msgs.isEmpty()) { emit this->driverMessages(msgs); }
                    const CSimulatedAircraftList aircraft = this->getAircraftInRange();
                    for (const CSimulatedAircraft &a : aircraft)
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
            CLogMessage(this).info(u"Remote aircraft updates limitations: %1") << this->updateAircraftLimitationInfo();
            return true;
        }

        // CG override
        if (part1 == QStringView(u"cg"))
        {
            if (parser.part(2).startsWith("clear", Qt::CaseInsensitive))
            {
                CLogMessage(this).info(u"Clear all overridden CGs");
                const CLengthPerCallsign cgsPerCallsign = this->clearCGOverrides();

                // restore all CGs
                for (const CCallsign &cs : this->getAircraftInRangeCallsigns())
                {
                    // reset CGs per callsign
                    const CLength cg = cgsPerCallsign.contains(cs) ? cgsPerCallsign[cs] : CLength::null();
                    this->updateCG(cs, cg);
                }
                return true;
            }

            if (parser.hasPart(3))
            {
                // ms can be a string like "B773 B773_RR SDM"
                const QString ms = parser.partAndRemainingStringAfter(3).toUpper();
                CLength cg;
                cg.parseFromString(parser.part(2), CPqString::SeparatorBestGuess);
                if (!ms.isEmpty())
                {
                    CLogMessage(this).info(u"Setting CG for '%1': %2") << ms << cg.valueRoundedWithUnit();
                    const bool set = this->insertCGForModelStringOverridden(cg, ms);
                    if (set)
                    {
                        const CCallsignSet callsigns = this->updateCGForModel(ms, cg);
                        if (!callsigns.isEmpty())
                        {
                            this->insertCGOverridden(cg, callsigns);
                            CLogMessage(this).info(u"Setting CG for '%1': %2") << callsigns.getCallsignsAsString(true) << cg.valueRoundedWithUnit();
                        }
                        return true;

                    } // set
                } // model string

            } // 3 parts
        }

        // driver specific cmd line arguments
        return this->parseDetails(parser);
    }

    void ISimulator::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackCore::ISimulator")) { return; }
        CSimpleCommandParser::registerCommand({".drv", "alias: .driver .plugin"});
        CSimpleCommandParser::registerCommand({".drv unload", "unload driver"});
        CSimpleCommandParser::registerCommand({".drv cg length clear|modelstr.", "override CG"});
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
        CSimpleCommandParser::registerCommand({".drv aircraft rm callsign", "remove a given callsign from simulator"});

        if (CBuildConfig::isCompiledWithFsuipcSupport())
        {
            CSimpleCommandParser::registerCommand({".drv fsuipc on|off", "enable/disable FSUIPC (if applicable)"});
        }
    }

    QString ISimulator::statusToString(SimulatorStatus status)
    {
        QStringList s;
        if (status.testFlag(Unspecified))  { s << QStringLiteral("Unspecified"); }
        if (status.testFlag(Disconnected)) { s << QStringLiteral("Disconnected"); }
        if (status.testFlag(Connected))    { s << QStringLiteral("Connected"); }
        if (status.testFlag(Simulating))   { s << QStringLiteral("Simulating"); }
        if (status.testFlag(Paused))       { s << QStringLiteral("Paused"); }
        return s.join(", ");
    }

    bool ISimulator::isEqualLastSent(const CAircraftSituation &compare) const
    {
        Q_ASSERT_X(compare.hasCallsign(), Q_FUNC_INFO, "Need callsign");
        if (!m_lastSentSituations.contains(compare.getCallsign())) { return false; }
        if (compare.isNull()) { return false; }
        return compare.equalPbhVectorAltitudeElevation(m_lastSentSituations.value(compare.getCallsign()));
        // return compare.equalPbhVectorAltitude(m_lastSentSituations.value(compare.getCallsign()));
    }

    bool ISimulator::isEqualLastSent(const CAircraftParts &compare, const CCallsign &callsign) const
    {
        if (callsign.isEmpty()) { return false; }
        if (!m_lastSentParts.contains(callsign)) { return false; }
        return compare.equalValues(m_lastSentParts.value(callsign));
    }

    void ISimulator::rememberLastSent(const CAircraftSituation &sent)
    {
        // normally we should never end up without callsign, but it has happened in real world scenarios
        // https://discordapp.com/channels/539048679160676382/568904623151382546/575712119513677826
        const bool hasCs = sent.hasCallsign();
        BLACK_VERIFY_X(hasCs, Q_FUNC_INFO, "Need callsign");
        if (!hasCs) { return; }
        m_lastSentSituations.insert(sent.getCallsign(), sent);
    }

    void ISimulator::rememberLastSent(const CAircraftParts &sent, const CCallsign &callsign)
    {
        // normally we should never end up without callsign, but it has happened in real world scenarios
        // https://discordapp.com/channels/539048679160676382/568904623151382546/575712119513677826
        BLACK_VERIFY_X(!callsign.isEmpty(), Q_FUNC_INFO, "Need callsign");
        if (callsign.isEmpty()) { return; }
        m_lastSentParts.insert(callsign, sent);
    }

    CAircraftSituationList ISimulator::getLastSentCanLikelySkipNearGroundInterpolation() const
    {
        const QList<CAircraftSituation> situations = m_lastSentSituations.values();
        CAircraftSituationList skipped;
        for (const CAircraftSituation &s : situations)
        {
            if (s.canLikelySkipNearGroundInterpolation()) { skipped.push_back(s); }
        }
        return skipped;
    }

    bool ISimulator::isAnyConnectedStatus(SimulatorStatus status)
    {
        return (status.testFlag(Connected) || status.testFlag(Simulating) || status.testFlag(Paused));
    }

    const CCallsign &ISimulator::getTestCallsign()
    {
        static const CCallsign cs("SWIFT");
        return cs;
    }

    ISimulator::ISimulator(const CSimulatorPluginInfo &pluginInfo,
                           IOwnAircraftProvider       *ownAircraftProvider,
                           IRemoteAircraftProvider    *remoteAircraftProvider,
                           IWeatherGridProvider       *weatherGridProvider,
                           IClientProvider            *clientProvider,
                           QObject                    *parent) :
        QObject(parent),
        COwnAircraftAware(ownAircraftProvider),
        CRemoteAircraftAware(remoteAircraftProvider),
        CWeatherGridAware(weatherGridProvider),
        CClientAware(clientProvider),
        ISimulationEnvironmentProvider(pluginInfo),
        IInterpolationSetupProvider(),
        CIdentifiable(this)
    {
        this->setObjectName("Simulator: " + pluginInfo.getIdentifier());
        m_interpolationLogger.setObjectName("Logger: " + pluginInfo.getIdentifier());

        ISimulator::registerHelp();

        // provider signals, hook up with remote aircraft provider
        m_remoteAircraftProviderConnections.append(
            CRemoteAircraftAware::provider()->connectRemoteAircraftProviderSignals(
                this, // receiver must match object in bind
                nullptr,
                nullptr,
                std::bind(&ISimulator::rapOnRemoteProviderRemovedAircraft, this, std::placeholders::_1),
                std::bind(&ISimulator::rapOnRecalculatedRenderedAircraft, this, std::placeholders::_1))
        );

        // timer
        connect(&m_oneSecondTimer, &QTimer::timeout, this, &ISimulator::oneSecondTimerTimeout);
        m_oneSecondTimer.setObjectName(this->objectName().append(":m_oneSecondTimer"));
        m_oneSecondTimer.start(1000);

        // swift data
        if (sApp && sApp->hasWebDataServices())
        {
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAllDataRead,  this, &ISimulator::onSwiftDbAllDataRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbAirportsRead, this, &ISimulator::onSwiftDbAirportsRead, Qt::QueuedConnection);
            connect(sApp->getWebDataServices(), &CWebDataServices::swiftDbModelMatchingEntitiesRead, this, &ISimulator::onSwiftDbModelMatchingEntitiesRead, Qt::QueuedConnection);
        }
        connect(sApp, &CApplication::aboutToShutdown, this, &ISimulator::unload, Qt::QueuedConnection);

        // provider
        if (pluginInfo.isEmulatedPlugin() && !pluginInfo.getSimulatorInfo().isSingleSimulator())
        {
            // emulated driver with NO info yet
            CLogMessage(this).info(u"Plugin '%1' with no simulator info yet, hope it will be set later") << pluginInfo.getIdentifier();
        }
        else
        {
            // NORMAL CASE or plugin with info already set
            this->setNewPluginInfo(pluginInfo, m_multiSettings.getSettings(pluginInfo.getSimulatorInfo()));
        }

        // info data
        m_simulatorInternals.setSimulatorName(this->getSimulatorName());
        m_simulatorInternals.setSwiftPluginName(this->getSimulatorPluginInfo().toQString());

        // model changed
        connect(this, &ISimulator::ownAircraftModelChanged, this, &ISimulator::onOwnModelChanged, Qt::QueuedConnection);

        // info
        CLogMessage(this).info(u"Initialized simulator driver: '%1'") <<
                (this->getSimulatorInfo().isUnspecified() ?
                 this->getSimulatorPluginInfo().toQString() :
                 this->getSimulatorInfo().toQString());
    }

    void ISimulator::onRecalculatedRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!snapshot.isValidSnapshot()) { return; }

        // for unrestricted values all add/remove actions are directly linked
        // when changing back from restricted->unrestricted an one time update is required
        if (!snapshot.isRestricted() && !snapshot.isRestrictionChanged()) { return; }

        Q_ASSERT_X(CThreadUtils::isInThisThread(this), Q_FUNC_INFO, "Needs to run in object thread");
        Q_ASSERT_X(snapshot.generatingThreadName() != QThread::currentThread()->objectName(), Q_FUNC_INFO, "Expect snapshot from background thread");

        // restricted snapshot values?
        bool changed = false;
        if (snapshot.isRenderingEnabled())
        {
            // make sure not to add aircraft again which are no longer in range
            const CCallsignSet callsignsInRange = this->getAircraftInRangeCallsigns();
            const CCallsignSet callsignsEnabledAndStillInRange = snapshot.getEnabledAircraftCallsignsByDistance().intersection(callsignsInRange);
            const CCallsignSet callsignsInSimulator(this->physicallyRenderedAircraft()); // state in simulator
            const CCallsignSet callsignsToBeRemoved(callsignsInSimulator.difference(callsignsEnabledAndStillInRange));
            const CCallsignSet callsignsToBeAdded(callsignsEnabledAndStillInRange.difference(callsignsInSimulator));
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

    bool ISimulator::physicallyRemoveRemoteAircraft(const CCallsign &callsign)
    {
        this->resetLastSentValues(callsign);
        return true;
    }

    int ISimulator::physicallyRemoveMultipleRemoteAircraft(const CCallsignSet &callsigns)
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

    int ISimulator::physicallyRemoveAllRemoteAircraft()
    {
        // a default implementation, but normally overridden by the sims
        const CCallsignSet callsigns = this->getAircraftInRangeCallsigns();

        // normally that would be already done in the specializied implementation
        const int r = this->physicallyRemoveMultipleRemoteAircraft(callsigns);

        // leave no trash
        this->clearAllRemoteAircraftData(); // remove all aircraft
        return r;
    }

    CAirportList ISimulator::getWebServiceAirports() const
    {
        if (this->isShuttingDown()) { return CAirportList(); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirportList(); }
        return sApp->getWebDataServices()->getAirports();
    }

    CAirport ISimulator::getWebServiceAirport(const CAirportIcaoCode &icao) const
    {
        if (this->isShuttingDown()) { return CAirport(); }
        if (!sApp || sApp->isShuttingDown() || !sApp->hasWebDataServices()) { return CAirport(); }
        return sApp->getWebDataServices()->getAirports().findFirstByIcao(icao);
    }

    int ISimulator::maxAirportsInRange() const
    {
        // might change in future or become a setting or such
        return 20;
    }

    void ISimulator::onSwiftDbAllDataRead()
    {
        // void, can be overridden in specialized drivers
    }

    void ISimulator::onSwiftDbModelMatchingEntitiesRead()
    {
        // void, can be overridden in specialized drivers
    }

    void ISimulator::onSwiftDbAirportsRead()
    {
        // void, can be overridden in specialized drivers
    }

    void ISimulator::initSimulatorInternals()
    {
        const CSpecializedSimulatorSettings s = this->getSimulatorSettings();
        m_simulatorInternals.setSimulatorName(this->getSimulatorName());
        m_simulatorInternals.setSwiftPluginName(this->getSimulatorPluginInfo().toQString());
        m_simulatorInternals.setSimulatorInstallationDirectory(s.getSimulatorDirectoryOrDefault());
    }

    void ISimulator::rememberElevationAndSimulatorCG(const CCallsign &callsign, const CAircraftModel &model, bool likelyOnGroundElevation, const CElevationPlane &elevation, const CLength &simulatorCG)
    {
        if (callsign.isEmpty()) { return; }
        if (elevation.hasMSLGeodeticHeight())
        {
            const int aircraftCount = this->getAircraftInRangeCount();
            this->setMaxElevationsRemembered(aircraftCount * 3); // at least 3 elevations per aircraft, even better as not all are requesting elevations
            this->rememberGroundElevation(callsign, likelyOnGroundElevation, elevation);
        }

        const QString modelString = model.getModelString();
        if (modelString.isEmpty()) { return; }

        // this value now is the simulator or overridden value
        const CLength cgOvr = this->overriddenCGorDefault(simulatorCG, modelString);
        if (!cgOvr.isNull() && !this->hasSameSimulatorCG(cgOvr, callsign))
        {
            // the value did change
            const CSimulatorSettings::CGSource source = this->getSimulatorSettings().getSimulatorSettings().getCGSource();
            if (source != CSimulatorSettings::CGFromDBOnly)
            {
                this->insertCG(cgOvr, modelString, callsign); // per model string and CG
            }

            // here we know we have a valid model and CG did change
            const CSimulatorInfo sim = this->getSimulatorInfo();
            m_autoPublishing.insert(modelString, simulatorCG); // still using simulator CG here, not the overridden value

            // if simulator did change, add as well
            if (!model.getSimulator().matchesAll(sim))
            {
                m_autoPublishing.insert(modelString, this->getSimulatorInfo());
            }
        }
    }

    void ISimulator::emitSimulatorCombinedStatus(SimulatorStatus oldStatus)
    {
        const SimulatorStatus newStatus = this->getSimulatorStatus();
        if (oldStatus != newStatus)
        {
            // decouple, follow up of signal can include unloading
            // simulator so this should happen strictly asyncronously (which is like forcing Qt::QueuedConnection)
            QPointer<ISimulator> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself || !sApp || sApp->isShuttingDown()) { return; }

                // now simulating
                if (newStatus.testFlag(Simulating))
                {
                    this->setUpdateAllRemoteAircraft(); // force an update of every remote aircraft
                }

                emit this->simulatorStatusChanged(newStatus); // only place where we should emit the signal, use emitSimulatorCombinedStatus to emit
            });
        }
    }

    void ISimulator::emitInterpolationSetupChanged()
    {
        QPointer<ISimulator> myself(this);
        QTimer::singleShot(5, this, [ = ]
        {
            if (!myself) { return; }
            emit this->interpolationAndRenderingSetupChanged();
        });
    }

    bool ISimulator::setInterpolationSetupGlobal(const CInterpolationAndRenderingSetupGlobal &setup)
    {
        if (!IInterpolationSetupProvider::setInterpolationSetupGlobal(setup)) { return false; }
        const bool r = setup.isRenderingRestricted();
        const bool e = setup.isRenderingEnabled();

        if (!this->isShuttingDown()) { CCrashHandler::instance()->crashAndLogAppendInfo(u"Rendering setup: " % setup.toQString(true)); }
        emit this->renderRestrictionsChanged(r, e, setup.getMaxRenderedAircraft(), setup.getMaxRenderedDistance());
        return true;
    }

    CAircraftSituationList ISimulator::getLoopbackSituations(const CCallsign &callsign) const
    {
        return m_loopbackSituations.value(callsign);
    }

    CAirportList ISimulator::getAirportsInRange(bool recalculateDistance) const
    {
        // default implementation
        if (this->isShuttingDown()) { return CAirportList(); }
        if (!sApp || !sApp->hasWebDataServices()) { return CAirportList(); }

        const CAirportList airports = sApp->getWebDataServices()->getAirports();
        if (airports.isEmpty()) { return airports; }
        const CCoordinateGeodetic ownPosition = this->getOwnAircraftPosition();
        CAirportList airportsInRange = airports.findClosest(maxAirportsInRange(), ownPosition);
        if (recalculateDistance) { airportsInRange.calculcateAndUpdateRelativeDistanceAndBearing(this->getOwnAircraftPosition()); }
        return airportsInRange;
    }

    CAircraftModel ISimulator::reverseLookupModel(const CAircraftModel &model)
    {
        bool modified = false;
        const CAircraftModel reverseModel = CDatabaseUtils::consolidateOwnAircraftModelWithDbData(model, false, &modified);
        return reverseModel;
    }

    bool ISimulator::isUpdateAircraftLimited(qint64 timestamp)
    {
        if (!m_limitUpdateAircraft) { return false; }
        const bool hasToken = m_limitUpdateAircraftBucket.tryConsume(1, timestamp);
        return !hasToken;
    }

    bool ISimulator::isUpdateAircraftLimitedWithStats(qint64 startTime)
    {
        const bool limited = this->isUpdateAircraftLimited(startTime);
        return limited;
    }

    bool ISimulator::limitToUpdatesPerSecond(int numberPerSecond)
    {
        if (numberPerSecond < 1)
        {
            m_limitUpdateAircraft = false;
            return false;
        }

        int tokens = qRound(0.1 * numberPerSecond); // 100ms
        do
        {
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(100); break; }
            tokens = qRound(0.25 * numberPerSecond); // 250ms
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(250); break; }
            tokens = qRound(0.5 * numberPerSecond); // 500ms
            if (tokens >= 3) { m_limitUpdateAircraftBucket.setInterval(500); break; }
            tokens = numberPerSecond;
            m_limitUpdateAircraftBucket.setInterval(1000);
        }
        while (false);

        m_limitUpdateAircraftBucket.setCapacityAndTokensToRefill(tokens);
        m_limitUpdateAircraft = true;
        return true;
    }

    QString ISimulator::updateAircraftLimitationInfo() const
    {
        if (!m_limitUpdateAircraft) { return QStringLiteral("not limited"); }
        static const QString limInfo("Limited %1 time(s) with %2/secs.");
        return limInfo.arg(m_statsUpdateAircraftLimited).arg(m_limitUpdateAircraftBucket.getTokensPerSecond());
    }

    void ISimulator::resetLastSentValues()
    {
        m_lastSentParts.clear();
        m_lastSentSituations.clear();
    }

    void ISimulator::resetLastSentValues(const CCallsign &callsign)
    {
        m_lastSentParts.remove(callsign);
        m_lastSentSituations.remove(callsign);
    }

    void ISimulator::unload()
    {
        this->disconnectFrom(); // disconnect from simulator
        const bool saved = m_autoPublishing.writeJsonToFile(); // empty data are ignored
        if (saved) { emit this->autoPublishDataWritten(this->getSimulatorInfo()); }
        m_autoPublishing.clear();
        m_remoteAircraftProviderConnections.disconnectAll(); // disconnect signals from provider
    }

    bool ISimulator::isAircraftInRangeOrTestMode(const CCallsign &callsign) const
    {
        return this->isTestMode() || this->isAircraftInRange(callsign);
    }

    bool ISimulator::disconnectFrom()
    {
        m_averageFps      = -1.0;
        m_simTimeRatio    =  1.0;
        m_trackMilesShort =  0.0;
        m_minutesLate     =  0.0;
        return true;
    }

    bool ISimulator::logicallyReAddRemoteAircraft(const CCallsign &callsign)
    {
        if (this->isShuttingDown()) { return false; }
        if (callsign.isEmpty())     { return false; }

        this->stopHighlighting();
        this->logicallyRemoveRemoteAircraft(callsign);
        if (!this->isAircraftInRange(callsign)) { return false; }
        const QPointer<ISimulator> myself(this);
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

    CCallsignSet ISimulator::unrenderedEnabledAircraft() const
    {
        const CSimulatedAircraftList aircraft = this->getAircraftInRange().findByEnabled(true);
        if (aircraft.isEmpty()) { return CCallsignSet(); }
        CCallsignSet enabledOnes = aircraft.getCallsigns();
        const CCallsignSet renderedOnes = this->physicallyRenderedAircraft();
        enabledOnes.remove(renderedOnes);
        return enabledOnes;
    }

    CCallsignSet ISimulator::renderedDisabledAircraft() const
    {
        const CSimulatedAircraftList aircraft = this->getAircraftInRange().findByEnabled(false);
        if (aircraft.isEmpty()) { return CCallsignSet(); }
        const CCallsignSet disabledOnes = aircraft.getCallsigns();
        const CCallsignSet renderedOnes = this->physicallyRenderedAircraft();
        return renderedOnes.intersection(disabledOnes);
    }

    bool ISimulator::changeRemoteAircraftEnabled(const CSimulatedAircraft &aircraft)
    {
        if (this->isShuttingDown()) { return false; }
        return aircraft.isEnabled() ?
               this->physicallyAddRemoteAircraft(aircraft) :
               this->physicallyRemoveRemoteAircraft(aircraft.getCallsign());
    }

    bool ISimulator::changeRemoteAircraftModel(const CSimulatedAircraft &aircraft)
    {
        // we expect the new model "in aircraft"
        // remove upfront, and then enable / disable again
        if (this->isShuttingDown()) { return false; }
        const CCallsign callsign = aircraft.getCallsign();
        if (!this->isPhysicallyRenderedAircraft(callsign)) { return false; }
        this->physicallyRemoveRemoteAircraft(callsign);
        // return this->changeRemoteAircraftEnabled(aircraft);

        const QPointer<ISimulator> myself(this);
        QTimer::singleShot(1000, this, [ = ]
        {
            if (!myself) { return; }
            if (this->isAircraftInRange(callsign))
            {
                this->changeRemoteAircraftEnabled(aircraft);
            }
        });
        return true;
    }

    CStatusMessageList ISimulator::debugVerifyStateAfterAllAircraftRemoved() const
    {
        CStatusMessageList msgs;
        if (!CBuildConfig::isLocalDeveloperDebugBuild()) { return msgs; }
        if (!m_addAgainAircraftWhenRemoved.isEmpty()) { msgs.push_back(CStatusMessage(this).error(u"m_addAgainAircraftWhenRemoved not empty: '%1'") << m_addAgainAircraftWhenRemoved.getCallsignStrings(true).join(", ")); }
        return msgs;
    }

    QString ISimulator::getInvalidSituationLogMessage(const CCallsign &callsign, const CInterpolationStatus &status, const QString &details) const
    {
        static const QString msg("Interpolation ('%1'): '%2'");
        const QString m = msg.arg(callsign.asString(), status.toQString());
        if (details.isEmpty()) { return m; }

        static const QString addDetails(" details: '%1'");
        return m % addDetails.arg(details);
    }

    void ISimulator::finishUpdateRemoteAircraftAndSetStatistics(qint64 startTime, bool limited)
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const qint64 dt  = now - startTime;
        m_statsCurrentUpdateTimeMs = dt;
        m_statsUpdateAircraftTimeTotalMs += dt;
        m_statsUpdateAircraftRuns++;
        m_statsUpdateAircraftTimeAvgMs = static_cast<double>(m_statsUpdateAircraftTimeTotalMs) / static_cast<double>(m_statsUpdateAircraftRuns);
        m_updateRemoteAircraftInProgress = false;
        m_statsLastUpdateAircraftRequestedMs = startTime;

        if (!this->isUpdateAllRemoteAircraft(startTime)) { this->resetUpdateAllRemoteAircraft(); }

        if (m_statsMaxUpdateTimeMs < dt) { m_statsMaxUpdateTimeMs = dt; }
        if (m_statsLastUpdateAircraftRequestedMs > 0) { m_statsUpdateAircraftRequestedDeltaMs = startTime - m_statsLastUpdateAircraftRequestedMs; }
        if (limited) { m_statsUpdateAircraftLimited++; }
    }

    void ISimulator::onOwnModelChanged(const CAircraftModel &newModel)
    {
        Q_UNUSED(newModel)
        // can be overridden
    }

    bool ISimulator::updateOwnSituationAndGroundElevation(const CAircraftSituation &situation)
    {
        const bool updated = this->updateOwnSituation(situation);

        // do not use every situation, but every deltaMs and only on ground
        constexpr qint64 deltaMs = 5000;
        if (situation.isOnGround() && situation.getTimeDifferenceAbsMs(m_lastRecordedGndElevationMs) > deltaMs)
        {
            m_lastRecordedGndElevationMs = situation.getMSecsSinceEpoch();
            const CSimulatorSettings settings = m_multiSettings.getSettings(this->getSimulatorInfo());
            if (settings.isRecordOwnAircraftGnd())
            {
                const CSimulatedAircraft ownAircraft = this->getOwnAircraft();
                CAltitude elevation = situation.getGroundElevation();
                if (elevation.isNull())
                {
                    // calculate elevation
                    const CLength cg = ownAircraft.getModel().getCG();
                    elevation = (cg.isNull() || situation.getAltitude().isNull()) ? CAltitude::null() : (situation.getAltitude().withOffset(cg * -1.0));
                }

                // own ground elevations
                if (elevation.hasMeanSeaLevelValue())
                {
                    const CCallsign cs    = situation.hasCallsign() ? situation.getCallsign() : ownAircraft.getCallsign();
                    const CLength radius  = settings.getRecordedGndRadius().isNull() ? CElevationPlane::singlePointRadius() : settings.getRecordedGndRadius();
                    const CElevationPlane ep(situation, radius);
                    const bool remembered = this->rememberGroundElevation(cs, situation.isOnGround(), ep, radius);

                    if (CBuildConfig::isLocalDeveloperDebugBuild())
                    {
                        const bool invalid = situation.isOnGround() && elevation.isZeroEpsilonConsidered();
                        BLACK_AUDIT_X(!invalid, Q_FUNC_INFO, "On ground in water");
                    }
                    Q_UNUSED(remembered) // false means it was already in that cache, or something else is wrong
                }
            }
        }
        return updated;
    }

    CAircraftModelList ISimulator::getModelSet() const
    {
        const CSimulatorInfo simulator = this->getSimulatorInfo();
        if (!simulator.isSingleSimulator()) { return CAircraftModelList(); }

        CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
        return CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
    }

    bool ISimulator::validateModelOfAircraft(const CSimulatedAircraft &aircraft) const
    {
        const CAircraftModel model = aircraft.getModel();
        if (!aircraft.hasCallsign())
        {
            CLogMessage(this).warning(u"Missing callsign for '%1'") << aircraft.getModelString();
            return false;
        }
        if (!model.hasModelString())
        {
            CLogMessage(this).warning(u"No model string for callsign '%1'") << aircraft.getCallsign();
            return false;
        }
        if (model.isCallsignEmpty())
        {
            CLogMessage(this).warning(u"No callsign for model of aircraft '%1'") << aircraft.getCallsign();
            return false;
        }
        return true;
    }

    void ISimulator::logAddingAircraftModel(const CSimulatedAircraft &aircraft) const
    {
        CLogMessage(this).info(u"Adding '%1' '%2' to '%3'") << aircraft.getCallsign() << aircraft.getModel().getModelStringAndDbKey() << this->getSimulatorInfo().toQString(true);
    }

    QString ISimulator::latestLoggedDataFormatted(const CCallsign &cs) const
    {
        const SituationLog s = m_interpolationLogger.getLastSituationLog(cs);
        const PartsLog p = m_interpolationLogger.getLastPartsLog(cs);

        static const QString sep("\n------\n");
        QString dm;
        if (s.tsCurrent > 0)
        {
            dm = u"Setup: " % s.usedSetup.toQString(true) %
                 u"\n\n" %
                 u"Situation: " % s.toQString(false, true, true, true, true, sep);
        }
        if (p.tsCurrent > 0) { dm += (dm.isEmpty() ? u"Parts: " : u"\n\nParts: ") % p.toQString(sep); }
        return dm;
    }

    void ISimulator::rapOnRecalculatedRenderedAircraft(const CAirspaceAircraftSnapshot &snapshot)
    {
        if (!this->isConnected())   { return; }
        if (this->isShuttingDown()) { return; }
        this->onRecalculatedRenderedAircraft(snapshot);
    }

    void ISimulator::rapOnRemoteProviderRemovedAircraft(const CCallsign &callsign)
    {
        Q_UNUSED(callsign)
        // currently not used, the calls are handled by context call logicallyRemoveRemoteAircraft
    }

    void ISimulator::callPhysicallyAddRemoteAircraft(const CSimulatedAircraft &remoteAircraft)
    {
        m_statsPhysicallyAddedAircraft++;
        this->physicallyAddRemoteAircraft(remoteAircraft);
    }

    void ISimulator::callPhysicallyRemoveRemoteAircraft(const CCallsign &remoteCallsign, bool blinking)
    {
        if (!blinking) { this->clearData(remoteCallsign); }
        this->physicallyRemoveRemoteAircraft(remoteCallsign);
    }

    void ISimulator::displayLoggedSituationInSimulator(const CCallsign &cs, bool stopLogging, int times)
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

        const int t = CMathUtils::randomInteger(4500, 5500); // makes sure not always using the same time difference
        const QPointer<ISimulator> myself(this);
        QTimer::singleShot(t, this, [ = ]
        {
            if (!myself || myself->isShuttingDown()) { return; }
            this->displayLoggedSituationInSimulator(cs, stopLogging, times - 1);
        });
    }

    void ISimulator::reverseLookupAndUpdateOwnAircraftModel(const QString &modelString)
    {
        CAircraftModel model = this->getOwnAircraftModel();
        model.setModelString(modelString);
        this->reverseLookupAndUpdateOwnAircraftModel(model);
    }

    void ISimulator::reverseLookupAndUpdateOwnAircraftModel(const CAircraftModel &model)
    {
        if (!model.hasModelString()) { return; }
        if (this->isShuttingDown())  { return; }
        Q_ASSERT_X(sApp->hasWebDataServices(), Q_FUNC_INFO, "Missing web services");

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

    ISimulatorListener::ISimulatorListener(const CSimulatorPluginInfo &info) :
        QObject(), m_info(info)
    {
        this->setObjectName("ISimulatorListener:" + info.toQString());

        // stop listener after it reports simulator ready
        bool s = connect(this, &ISimulatorListener::simulatorStarted, this, &ISimulatorListener::stop, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "connect failed");

        if (sApp)
        {
            s = connect(sApp, &CApplication::aboutToShutdown, this, &ISimulatorListener::onAboutToShutdown, Qt::QueuedConnection);
            Q_ASSERT_X(s, Q_FUNC_INFO, "connect failed");
        }

        Q_UNUSED(s)
    }

    QString ISimulatorListener::backendInfo() const
    {
        return m_info.toQString();
    }

    bool ISimulatorListener::isShuttingDown() const
    {
        return (!sApp || sApp->isShuttingDown() || m_aboutToShutdown);
    }

    void ISimulatorListener::onAboutToShutdown()
    {
        if (!m_aboutToShutdown) { return; }
        m_aboutToShutdown = true;
        this->stop();
    }

    void ISimulatorListener::start()
    {
        if (m_isRunning) { return; }
        if (!CThreadUtils::isInThisThread(this))
        {
            // call in correct thread
            QPointer<ISimulatorListener> myself(this);
            QTimer::singleShot(0, this, [ = ] { if (myself) { this->start(); }});
            return;
        }

        m_isRunning = true;
        this->startImpl();
    }

    void ISimulatorListener::stop()
    {
        if (!m_isRunning) { return; }
        if (!CThreadUtils::isInThisThread(this))
        {
            // call in correct thread
            QPointer<ISimulatorListener> myself(this);
            QTimer::singleShot(0, this, [ = ] { if (myself) { this->stop(); }});
            return;
        }

        this->stopImpl();
        m_isRunning = false;
    }

    void ISimulatorListener::check()
    {
        if (!m_isRunning) { return; }
        if (!CThreadUtils::isInThisThread(this))
        {
            // call in correct thread
            QPointer<ISimulatorListener> myself(this);
            QTimer::singleShot(0, this, [ = ] { if (myself) { this->check(); }});
            return;
        }

        this->checkImpl();
    }
} // namespace

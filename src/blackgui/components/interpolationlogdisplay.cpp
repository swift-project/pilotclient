/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationlogdisplay.h"
#include "ui_interpolationlogdisplay.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextnetworkimpl.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/airspacemonitor.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/stringutils.h"

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CInterpolationLogDisplay::CInterpolationLogDisplay(QWidget *parent) :
            COverlayMessagesFrame(parent),
            CIdentifiable(this),
            ui(new Ui::CInterpolationLogDisplay)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

            ui->setupUi(this);
            ui->tw_LogTabs->setCurrentIndex(TabFlow);
            constexpr int timeSecs = 5;
            ui->hs_UpdateTime->setValue(timeSecs);
            this->onSliderChanged(timeSecs);

            CLedWidget::LedShape shape = CLedWidget::Rounded;
            ui->led_Parts->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Parts received", "", 14);
            ui->led_Situation->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Situation received", "", 14);
            ui->led_Elevation->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Elevation received", "", 14);
            ui->led_Running->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Running", "Stopped", 14);
            ui->led_Updating->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Just updating", "Idle", 14);

            m_callsign = ui->comp_CallsignCompleter->getCallsign();

            connect(&m_updateTimer, &QTimer::timeout, this, &CInterpolationLogDisplay::updateLog);
            connect(ui->comp_CallsignCompleter, &CCallsignCompleter::validCallsignEntered, this, &CInterpolationLogDisplay::onCallsignEntered);
            connect(ui->hs_UpdateTime, &QSlider::valueChanged, this, &CInterpolationLogDisplay::onSliderChanged);
            connect(ui->pb_StartStop, &QPushButton::released, this, &CInterpolationLogDisplay::toggleStartStop);
            connect(ui->pb_ResetStats, &QPushButton::released, this, &CInterpolationLogDisplay::resetStatistics);
            connect(ui->pb_ShowInSimulator, &QPushButton::released, this, &CInterpolationLogDisplay::showLogInSimulator);
            connect(ui->pb_GetLastInterpolation, &QPushButton::released, this, &CInterpolationLogDisplay::displayLastInterpolation);
            connect(sGui, &CGuiApplication::aboutToShutdown, this, &CInterpolationLogDisplay::onAboutToShutdown);

            // ui->le_Foo->setVisible(false);
            // ui->lbl_Foo->setVisible(false);
        }

        CInterpolationLogDisplay::~CInterpolationLogDisplay()
        {
            // void
        }

        void CInterpolationLogDisplay::setSimulator(CSimulatorCommon *simulatorCommon)
        {
            if (simulatorCommon && simulatorCommon == m_simulatorCommon) { return; } // same
            if (m_simulatorCommon)
            {
                this->disconnect(m_simulatorCommon);
                m_simulatorCommon->disconnect(this);
            }
            m_simulatorCommon = simulatorCommon;
            if (!simulatorCommon) { return; }
            connect(m_simulatorCommon, &CSimulatorCommon::receivedRequestedElevation, this, &CInterpolationLogDisplay::onElevationReceived);
            connect(m_simulatorCommon, &CSimulatorCommon::requestedElevation, this, &CInterpolationLogDisplay::onElevationRequested);
            connect(m_simulatorCommon, &CSimulatorCommon::destroyed, this, &CInterpolationLogDisplay::onSimulatorUnloaded);
            connect(m_simulatorCommon, &CSimulatorCommon::simulatorStatusChanged, this, &CInterpolationLogDisplay::onSimulatorStatusChanged);
        }

        void CInterpolationLogDisplay::setAirspaceMonitor(CAirspaceMonitor *airspaceMonitor)
        {
            if (airspaceMonitor && airspaceMonitor == m_airspaceMonitor) { return; } // same
            if (m_airspaceMonitor)
            {
                this->disconnect(m_airspaceMonitor);
                m_airspaceMonitor->disconnect(this);
            }
            m_airspaceMonitor = airspaceMonitor;

            connect(m_airspaceMonitor, &CAirspaceMonitor::addedAircraftSituation, this, &CInterpolationLogDisplay::onSituationAdded, Qt::QueuedConnection);
            connect(m_airspaceMonitor, &CAirspaceMonitor::addedAircraftParts, this, &CInterpolationLogDisplay::onPartsAdded, Qt::QueuedConnection);
        }

        void CInterpolationLogDisplay::updateLog()
        {
            ui->led_Updating->blink(250);
            if (!this->checkLogPrerequisites())
            {
                ui->le_SimulatorSpecific->setText(m_simulatorCommon->getStatisticsSimulatorSpecific());
                return;
            }

            // only display visible tab
            if (ui->tw_LogTabs->currentWidget() == ui->tb_TextLog)
            {
                const QString log = m_simulatorCommon->latestLoggedDataFormatted(m_callsign);
                ui->te_TextLog->setText(log);
            }
            else if (ui->tw_LogTabs->currentWidget() == ui->tb_DataFlow)
            {
                ui->le_CG->setText(m_airspaceMonitor->getCG(m_callsign).valueRoundedWithUnit(CLengthUnit::ft(), 1));
                ui->le_CG->home(false);
                ui->le_Parts->setText(boolToYesNo(m_airspaceMonitor->isRemoteAircraftSupportingParts(m_callsign)));

                static const QString msTimeStr("%1ms");
                static const QString updateTimes("%1ms avg: %2ms max: %3ms");
                const QString avgUpdateTimeRounded = QString::number(m_simulatorCommon->getStatisticsAverageUpdateTimeMs(), 'f', 2);

                ui->le_UpdateTimes->setText(updateTimes.
                                            arg(m_simulatorCommon->getStatisticsCurrentUpdateTimeMs()).
                                            arg(avgUpdateTimeRounded).
                                            arg(m_simulatorCommon->getStatisticsMaxUpdateTimeMs()));
                ui->le_UpdateTimes->home(false);
                ui->le_UpdateCount->setText(QString::number(m_simulatorCommon->getStatisticsUpdateRuns()));
                ui->le_UpdateReqTime->setText(msTimeStr.arg(m_simulatorCommon->getStatisticsAircraftUpdatedRequestedDeltaMs()));
                ui->le_Limited->setText(m_simulatorCommon->updateAircraftLimitationInfo());

                ui->le_SimulatorSpecific->setText(m_simulatorCommon->getStatisticsSimulatorSpecific());
                ui->le_SimulatorSpecific->home(false);

                const CClient client = m_airspaceMonitor->getClientOrDefaultForCallsign(m_callsign);
                ui->le_GndFlag->setText(boolToYesNo(client.hasGndFlagCapability()));

                this->displayElevationRequestReceive();
                this->displayLastInterpolation();
            }
        }

        void CInterpolationLogDisplay::displayLastInterpolation()
        {
            if (!this->checkLogPrerequisites()) { return; }

            const SituationLog sLog = m_simulatorCommon->interpolationLogger().getLastSituationLog();
            ui->te_LastInterpolatedSituation->setText(sLog.situationCurrent.toQString(true));
            ui->te_SituationChange->setText(sLog.change.toQString(true));

            ui->le_SceneryOffset->setText(sLog.change.getGuessedSceneryDeviation().valueRoundedWithUnit(CLengthUnit::ft(), 1));
            ui->le_SceneryOffsetCG->setText(sLog.change.getGuessedSceneryDeviationCG().valueRoundedWithUnit(CLengthUnit::ft(), 1));

            const PartsLog pLog = m_simulatorCommon->interpolationLogger().getLastPartsLog();
            ui->te_LastInterpolatedParts->setText(pLog.parts.toQString(true));
        }

        void CInterpolationLogDisplay::onSliderChanged(int timeSecs)
        {
            static const QString time("%1secs");
            m_updateTimer.setInterval(timeSecs * 1000);
            ui->le_UpdateTime->setText(time.arg(timeSecs));
        }

        void CInterpolationLogDisplay::onCallsignEntered()
        {
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign();
            if (!m_simulatorCommon)
            {
                this->stop();
                return;
            }
            if (m_callsign == cs) { return; }

            // clear last callsign
            if (!m_callsign.isEmpty())
            {
                m_simulatorCommon->setLogInterpolation(false, m_callsign); // stop logging "old" callsign
                m_callsign = CCallsign(); // clear callsign
                this->clear();
            }

            // set new callsign or stop
            if (cs.isEmpty())
            {
                this->stop();
                return;
            }

            m_callsign = cs;
            m_simulatorCommon->setLogInterpolation(true, cs);
            if (!this->start())
            {
                this->initPartsView();
            }
        }

        void CInterpolationLogDisplay::toggleStartStop()
        {
            const bool running = m_updateTimer.isActive();
            if (running) { this->stop(); }
            else { this->start(); }
        }

        void CInterpolationLogDisplay::showLogInSimulator()
        {
            if (m_callsign.isEmpty()) { return; }
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

            const QString cmd = QStringLiteral(".drv pos ") + m_callsign.asString();
            sGui->getIContextSimulator()->parseCommandLine(cmd, this->identifier());
        }

        bool CInterpolationLogDisplay::start()
        {
            if (m_updateTimer.isActive()) { return false; }

            const int interval = 1000 * ui->hs_UpdateTime->value();
            m_updateTimer.start(interval);
            ui->pb_StartStop->setText(stopText());
            ui->led_Running->setOn(true);

            this->initPartsView();
            return true;
        }

        void CInterpolationLogDisplay::stop()
        {
            m_updateTimer.stop();
            ui->pb_StartStop->setText(startText());
            ui->led_Running->setOn(false);
        }

        bool CInterpolationLogDisplay::logCallsign(const CCallsign &cs) const
        {
            if (!sGui || sGui->isShuttingDown()) { return false; }
            if (!m_airspaceMonitor || !m_simulatorCommon || m_callsign.isEmpty()) { return false; }
            if (cs != m_callsign) { return false; }
            return true;
        }

        void CInterpolationLogDisplay::onAboutToShutdown()
        {
            m_updateTimer.stop();
            m_simulatorCommon = nullptr;
        }

        void CInterpolationLogDisplay::onSimulatorUnloaded()
        {
            m_updateTimer.stop();
            m_simulatorCommon = nullptr;
            this->resetStatistics();
        }

        void CInterpolationLogDisplay::onSimulatorStatusChanged(ISimulator::SimulatorStatus status)
        {
            Q_UNUSED(status);
            m_updateTimer.stop();
            this->resetStatistics();
        }

        void CInterpolationLogDisplay::onSituationAdded(const CAircraftSituation &situation)
        {
            const CCallsign cs = situation.getCallsign();
            if (!this->logCallsign(cs)) { return; }
            const CAircraftSituationList situations = m_airspaceMonitor->remoteAircraftSituations(cs);
            const CAircraftSituationChangeList changes = m_airspaceMonitor->remoteAircraftSituationChanges(cs);
            ui->tvp_AircraftSituations->updateContainerAsync(situations);
            ui->tvp_Changes->updateContainerMaybeAsync(changes);
            ui->led_Situation->blink();
        }

        void CInterpolationLogDisplay::onPartsAdded(const CCallsign &callsign, const CAircraftParts &parts)
        {
            if (!this->logCallsign(callsign)) { return; }
            Q_UNUSED(parts);
            const CAircraftPartsList partsList = m_airspaceMonitor->remoteAircraftParts(callsign);
            ui->tvp_AircraftParts->updateContainerAsync(partsList);
            ui->led_Parts->blink();
        }

        void CInterpolationLogDisplay::onElevationReceived(const CElevationPlane &plane, const CCallsign &callsign)
        {
            if (!this->logCallsign(callsign)) { return; }
            m_elvReceived++;
            ui->le_Elevation->setText(plane.toQString());
            this->displayElevationRequestReceive();
            ui->led_Elevation->blink();
        }

        void CInterpolationLogDisplay::onElevationRequested(const CCallsign &callsign)
        {
            if (!this->logCallsign(callsign)) { return; }
            m_elvRequested++;
            this->displayElevationRequestReceive();
            ui->led_Elevation->blink();
        }

        void CInterpolationLogDisplay::resetStatistics()
        {
            if (m_simulatorCommon) { m_simulatorCommon->resetAircraftStatistics(); }
        }

        void CInterpolationLogDisplay::clear()
        {
            ui->tvp_AircraftParts->clear();
            ui->tvp_AircraftSituations->clear();
            ui->te_TextLog->clear();
            ui->le_CG->clear();
            ui->le_Elevation->clear();
            ui->le_ElevationReqRec->clear();
            ui->le_Parts->clear();
            ui->le_UpdateTimes->clear();
            ui->le_UpdateTimes->clear();
            ui->le_Limited->clear();
            m_elvReceived = m_elvRequested = 0;
        }

        bool CInterpolationLogDisplay::checkLogPrerequisites()
        {
            CStatusMessage m;
            do
            {
                if (!this->isVisible()) { return false; } // silently return
                if (!sApp || sApp->isShuttingDown()) { break; } // stop and return
                if (m_callsign.isEmpty())
                {
                    static const CStatusMessage ms = CStatusMessage(this).validationError("No callsign");
                    m = ms;
                    break;
                }

                const bool canUpdateLog = m_airspaceMonitor && m_simulatorCommon && m_simulatorCommon->isConnected() && !m_simulatorCommon->isShuttingDown();
                if (!canUpdateLog)
                {
                    static const CStatusMessage ms = CStatusMessage(this).validationError("No airspace monitor or simulator or shutting down");
                    m = ms;
                    break;
                }

                if (!m_simulatorCommon->getLogCallsigns().contains(m_callsign))
                {
                    static const CStatusMessage ms = CStatusMessage(this).validationError("No longer logging callsign");
                    m = ms;
                    break;
                }
                return true;
            }
            while (false);

            this->stop();
            if (!m.isEmpty()) { this->showOverlayMessage(m); }
            return false;
        }

        void CInterpolationLogDisplay::initPartsView()
        {
            // it can take a while until we receive parts, so we init
            QPointer<CInterpolationLogDisplay> myself(this);
            QTimer::singleShot(250, this, [ = ]
            {
                if (!myself) { return; }
                if (m_callsign.isEmpty()) { return; }
                myself->onPartsAdded(m_callsign, CAircraftParts());
            });
        }

        void CInterpolationLogDisplay::displayElevationRequestReceive()
        {
            if (!m_airspaceMonitor) { return; }
            static const QString rr("%1 / %2 hits %3 / %4 %5%");
            const QPair<int, int> foundMissed = m_airspaceMonitor->getElevationsFoundMissed();
            const int f = foundMissed.first;
            const int m = foundMissed.second;
            const double hitRatioPercent = 100.0 * static_cast<double>(f) / static_cast<double>(f + m);
            ui->le_ElevationReqRec->setText(rr.arg(m_elvRequested).arg(m_elvReceived).arg(f).arg(m).arg(QString::number(hitRatioPercent, 'f', 1)));
        }

        void CInterpolationLogDisplay::linkWithAirspaceMonitor()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->supportsContexts()) { return; }
            if (!sGui->getCoreFacade() || !sGui->getCoreFacade()->getCContextNetwork()) { return; }
            const CContextNetwork *cn = sGui->getCoreFacade()->getCContextNetwork();
            this->setAirspaceMonitor(cn->airspace());
        }

        const QString &CInterpolationLogDisplay::startText()
        {
            static const QString start("start");
            return start;
        }

        const QString &CInterpolationLogDisplay::stopText()
        {
            static const QString stop("stop");
            return stop;
        }
    } // ns
} // ns

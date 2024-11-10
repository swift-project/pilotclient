// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "interpolationlogdisplay.h"
#include "ui_interpolationlogdisplay.h"
#include "blackgui/editors/coordinateform.h"
#include "blackgui/guiapplication.h"
#include "core/context/contextnetworkimpl.h"
#include "core/context/contextsimulator.h"
#include "core/airspacemonitor.h"
#include "misc/timestampobjectlist.h"
#include "misc/stringutils.h"

#include <QStringLiteral>

using namespace swift::core;
using namespace swift::core::context;
using namespace BlackGui::Views;
using namespace BlackGui::Editors;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::geo;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;

namespace BlackGui::Components
{
    const QStringList &CInterpolationLogDisplay::getLogCategories()
    {
        static const QStringList cats { CLogCategories::interpolator(), CLogCategories::driver() };
        return cats;
    }

    CInterpolationLogDisplay::CInterpolationLogDisplay(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                          CIdentifiable(this),
                                                                          ui(new Ui::CInterpolationLogDisplay)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");

        ui->setupUi(this);
        ui->tw_LogTabs->setCurrentIndex(TabFlow);
        constexpr int timeSecs = 5;
        ui->hs_UpdateTime->setValue(timeSecs);
        this->onSliderChanged(timeSecs);

        const CLedWidget::LedShape shape = CLedWidget::Rounded;
        ui->led_Parts->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Parts received", "", 14);
        ui->led_Situation->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Situation received", "", 14);
        ui->led_Elevation->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Elevation received", "", 14);
        ui->led_Running->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Running", "Stopped", 14);
        ui->led_Updating->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Just updating", "Idle", 14);

        m_callsign = ui->comp_CallsignCompleter->getCallsign();
        ui->tvp_InboundAircraftSituations->setWithMenuRequestElevation(true);

        m_elvHistoryModel = new QStringListModel(this);
        ui->lv_ElevevationHistory->setModel(m_elvHistoryModel);
        ui->lv_ElevevationHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);

        const int elvHistoryCount = 100;
        m_elvHistoryCount = elvHistoryCount;
        ui->le_ElvHistoryCount->setText(QString::number(elvHistoryCount));

        connect(&m_updateTimer, &QTimer::timeout, this, &CInterpolationLogDisplay::updateLog);
        connect(ui->comp_CallsignCompleter, &CCallsignCompleter::editingFinishedDigest, this, &CInterpolationLogDisplay::onCallsignEntered);
        connect(ui->hs_UpdateTime, &QSlider::valueChanged, this, &CInterpolationLogDisplay::onSliderChanged);
        connect(ui->pb_StartStop, &QPushButton::released, this, &CInterpolationLogDisplay::toggleStartStop);
        connect(ui->pb_ResetLastSent, &QPushButton::released, this, &CInterpolationLogDisplay::resetLastSentValues);
        connect(ui->pb_ResetStats, &QPushButton::released, this, &CInterpolationLogDisplay::resetStatistics);
        connect(ui->pb_ShowLogInSimulator, &QPushButton::released, this, &CInterpolationLogDisplay::logPosCommand);
        connect(ui->pb_FollowInSimulator, &QPushButton::released, this, &CInterpolationLogDisplay::followInSimulator);
        connect(ui->pb_RequestElevation1, &QPushButton::released, this, &CInterpolationLogDisplay::requestElevationClicked);
        connect(ui->pb_RequestElevation2, &QPushButton::released, this, &CInterpolationLogDisplay::requestElevationClicked);
        connect(ui->pb_GetLastInterpolation, &QPushButton::released, this, &CInterpolationLogDisplay::getLogAmdDisplayLastInterpolation);
        connect(ui->pb_InjectElevation, &QPushButton::released, this, &CInterpolationLogDisplay::onInjectElevation);
        connect(ui->pb_ElvClear, &QPushButton::released, this, &CInterpolationLogDisplay::clearElevationResults);
        connect(ui->pb_RecalcAllAircraft, &QPushButton::released, this, &CInterpolationLogDisplay::requestRecalculateAll);
        connect(ui->pb_ClearLog, &QPushButton::released, this, &CInterpolationLogDisplay::clearLogCommand);
        connect(ui->pb_ClearLog2, &QPushButton::released, this, &CInterpolationLogDisplay::clearLogCommand);
        connect(ui->pb_WriteLogToFile, &QPushButton::released, this, &CInterpolationLogDisplay::writeLogCommand);
        connect(ui->pb_WriteLogToFile2, &QPushButton::released, this, &CInterpolationLogDisplay::writeLogCommand);
        connect(ui->le_InjectElevation, &QLineEdit::returnPressed, this, &CInterpolationLogDisplay::onInjectElevation);
        connect(ui->le_ElvHistoryCount, &QLineEdit::editingFinished, this, &CInterpolationLogDisplay::onElevationHistoryCountFinished);
        connect(ui->cb_ElvAllowPseudo, &QCheckBox::toggled, this, &CInterpolationLogDisplay::onPseudoElevationToggled);
        connect(ui->tvp_InboundAircraftSituations, &CAircraftSituationView::requestElevation, this, &CInterpolationLogDisplay::requestElevation);
        connect(ui->editor_ElevationCoordinate, &CCoordinateForm::changedCoordinate, this, &CInterpolationLogDisplay::requestElevationAtPosition);
        connect(sGui, &CGuiApplication::aboutToShutdown, this, &CInterpolationLogDisplay::onAboutToShutdown, Qt::QueuedConnection);
    }

    CInterpolationLogDisplay::~CInterpolationLogDisplay()
    {
        // void
    }

    void CInterpolationLogDisplay::setSimulator(ISimulator *simulator)
    {
        if (simulator && simulator == m_simulator) { return; } // same
        if (m_simulator)
        {
            this->disconnect(m_simulator);
            m_simulator->disconnect(this);
        }
        m_simulator = simulator;
        if (!simulator) { return; }
        connect(m_simulator, &ISimulator::receivedRequestedElevation, this, &CInterpolationLogDisplay::onElevationReceived, Qt::QueuedConnection);
        connect(m_simulator, &ISimulator::requestedElevation, this, &CInterpolationLogDisplay::onElevationRequested, Qt::QueuedConnection);
        connect(m_simulator, &ISimulator::destroyed, this, &CInterpolationLogDisplay::onSimulatorUnloaded);
        connect(m_simulator, &ISimulator::simulatorStatusChanged, this, &CInterpolationLogDisplay::onSimulatorStatusChanged);
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
            ui->le_SimulatorSpecific->setText(m_simulator->getStatisticsSimulatorSpecific());
            return;
        }

        const SituationLog sLog = m_simulator->interpolationLogger().getLastSituationLog();
        m_lastInterpolations.push_frontKeepLatestAdjustedFirst(sLog.situationCurrent, true, 10);

        // only display visible tab
        if (ui->tw_LogTabs->currentWidget() == ui->tb_TextLog)
        {
            const QString log = m_simulator->latestLoggedDataFormatted(m_callsign);
            ui->te_TextLog->setText(log);
        }
        else if (ui->tw_LogTabs->currentWidget() == ui->tb_DataFlow && m_airspaceMonitor)
        {
            ui->le_CG->setText(m_airspaceMonitor->getSimulatorCG(m_callsign).valueRoundedWithUnit(CLengthUnit::ft(), 1));
            ui->le_CG->home(false);
            ui->le_Parts->setText(boolToYesNo(m_airspaceMonitor->isRemoteAircraftSupportingParts(m_callsign)));

            static const QString msTimeStr("%1ms");
            static const QString updateTimes("%1ms avg: %2ms max: %3ms");
            const QString avgUpdateTimeRounded = QString::number(m_simulator->getStatisticsAverageUpdateTimeMs(), 'f', 2);

            ui->le_UpdateTimes->setText(updateTimes.arg(m_simulator->getStatisticsCurrentUpdateTimeMs()).arg(avgUpdateTimeRounded).arg(m_simulator->getStatisticsMaxUpdateTimeMs()));
            ui->le_UpdateTimes->home(false);
            ui->le_UpdateCount->setText(QString::number(m_simulator->getStatisticsUpdateRuns()));
            ui->le_UpdateReqTime->setText(msTimeStr.arg(m_simulator->getStatisticsAircraftUpdatedRequestedDeltaMs()));
            ui->le_Limited->setText(m_simulator->updateAircraftLimitationInfo());

            ui->le_SimulatorSpecific->setText(m_simulator->getStatisticsSimulatorSpecific());
            ui->le_SimulatorSpecific->home(false);

            const CClient client = m_airspaceMonitor->getClientOrDefaultForCallsign(m_callsign);
            ui->le_GndFlag->setText(boolToYesNo(client.hasGndFlagCapability()));

            this->displayElevationRequestReceive();
            this->displayLastInterpolation(sLog);
        }
        else if (ui->tw_LogTabs->currentWidget() == ui->tb_Loopback)
        {
            this->displayLoopback();
        }
    }

    void CInterpolationLogDisplay::getLogAmdDisplayLastInterpolation()
    {
        if (!this->canLog()) { return; }
        const SituationLog sLog = m_simulator->interpolationLogger().getLastSituationLog();
        this->displayLastInterpolation(sLog);
    }

    void CInterpolationLogDisplay::displayLastInterpolation(const SituationLog &sLog)
    {
        if (!this->checkLogPrerequisites()) { return; }

        ui->te_LastInterpolatedSituation->setText(sLog.situationCurrent.toQString(true));
        ui->te_SituationChange->setText(sLog.change.toQString(true));

        ui->le_SceneryOffset->setText(sLog.change.getGuessedSceneryDeviation().valueRoundedWithUnit(CLengthUnit::ft(), 1));
        ui->le_SceneryOffsetCG->setText(sLog.change.getGuessedSceneryDeviationCG().valueRoundedWithUnit(CLengthUnit::ft(), 1));

        const PartsLog pLog = m_simulator->interpolationLogger().getLastPartsLog();
        ui->te_LastInterpolatedParts->setText(pLog.parts.toQString(true));
    }

    void CInterpolationLogDisplay::displayLoopback()
    {
        if (!m_simulator || m_callsign.isEmpty()) { return; }
        ui->tvp_LoopbackAircraftSituations->updateContainerAsync(m_simulator->getLoopbackSituations(m_callsign));
        ui->tvp_InterpolatedAircraftSituations->updateContainerAsync(m_lastInterpolations);
    }

    void CInterpolationLogDisplay::onSliderChanged(int timeSecs)
    {
        m_updateTimer.setInterval(timeSecs * 1000);
        ui->le_UpdateTime->setText(QStringLiteral("%1secs").arg(timeSecs));
    }

    void CInterpolationLogDisplay::onCallsignEntered()
    {
        if (!this->canLog())
        {
            this->stop();
            CLogMessage(this).warning(u"Stopping logging (log.display), no simulator");
            return;
        }

        const CCallsign cs = ui->comp_CallsignCompleter->getCallsign();
        if (m_callsign == cs) { return; }

        // empty callsign, just stop
        if (cs.isEmpty())
        {
            this->stop();
            m_callsign.clear();
            return;
        }

        // clear last callsign
        if (!m_callsign.isEmpty())
        {
            m_simulator->setLogInterpolation(false, m_callsign); // stop logging "old" callsign
            m_callsign.clear(); // clear callsign
            this->clear();
        }

        // set new callsign or stop
        m_callsign = cs;
        m_simulator->setLogInterpolation(true, cs);
        ui->comp_Parts->setCallsign(cs);
        if (!this->start())
        {
            this->initPartsView();
        }
        CLogMessage(this).info(u"Starting logging (log.display) of '%1'") << m_callsign;
    }

    void CInterpolationLogDisplay::onPseudoElevationToggled(bool checked)
    {
        if (!this->canLog()) { return; }
        m_simulator->setTestEnablePseudoElevation(checked);

        CAltitude elvTest = CAltitude::null();
        if (!ui->le_ElevationTestValue->text().isEmpty())
        {
            CLength l;
            const QString v = ui->le_ElevationTestValue->text();
            l.parseFromString(v);
            if (!l.isNull())
            {
                elvTest = CAltitude(l, CAltitude::MeanSeaLevel);
            }
        }
        m_simulator->setTestElevation(elvTest);
    }

    void CInterpolationLogDisplay::toggleStartStop()
    {
        const bool running = m_updateTimer.isActive();
        m_callsign.clear(); // force update of data and log. start/stop
        if (running)
        {
            this->stop();
        }
        else
        {
            // treat like a callsign was entered
            this->onCallsignEntered();
        }
    }

    void CInterpolationLogDisplay::followInSimulator()
    {
        if (m_callsign.isEmpty()) { return; }
        if (!this->canLog()) { return; }
        m_simulator->followAircraft(m_callsign);
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
        if (!m_airspaceMonitor || !m_simulator || m_callsign.isEmpty()) { return false; }
        if (cs != m_callsign) { return false; }
        return true;
    }

    void CInterpolationLogDisplay::onAboutToShutdown()
    {
        m_updateTimer.stop();
        m_simulator = nullptr;
    }

    void CInterpolationLogDisplay::onSimulatorUnloaded()
    {
        m_updateTimer.stop();
        m_simulator = nullptr;
        this->resetStatistics();
    }

    void CInterpolationLogDisplay::onSimulatorStatusChanged(ISimulator::SimulatorStatus status)
    {
        Q_UNUSED(status)
        m_updateTimer.stop();
        this->resetStatistics();
    }

    void CInterpolationLogDisplay::onSituationAdded(const CAircraftSituation &situation)
    {
        static const QString info("times: %1 offset %2");

        const CCallsign cs = situation.getCallsign();
        if (!this->logCallsign(cs)) { return; }
        const CAircraftSituationList situations = m_airspaceMonitor->remoteAircraftSituations(cs);
        const MillisecondsMinMaxMean tsDiffMsMinMaxMean = situations.getTimestampDifferenceMinMaxMean();
        const MillisecondsMinMaxMean offsetMsMinMaxMean = situations.getOffsetMinMaxMean();
        const CAircraftSituationChangeList changes = m_airspaceMonitor->remoteAircraftSituationChanges(cs);
        ui->tvp_InboundAircraftSituations->updateContainerAsync(situations);
        ui->tvp_Changes->updateContainerMaybeAsync(changes);
        ui->le_InboundSituationsInfo->setText(info.arg(tsDiffMsMinMaxMean.asString(), offsetMsMinMaxMean.asString()));
        ui->led_Situation->blink();
    }

    void CInterpolationLogDisplay::onPartsAdded(const CCallsign &callsign, const CAircraftParts &parts)
    {
        if (!this->logCallsign(callsign)) { return; }
        Q_UNUSED(parts)
        const CAircraftPartsList partsList = m_airspaceMonitor->remoteAircraftParts(callsign);
        ui->tvp_InboundAircraftParts->updateContainerAsync(partsList);
        ui->led_Parts->blink();
    }

    void CInterpolationLogDisplay::onElevationReceived(const CElevationPlane &elevationPlane, const CCallsign &callsign)
    {
        m_elvReceived++;
        if (m_elvHistoryCount > 0)
        {
            const QString history = callsign.asString() %
                                    QStringLiteral(": ") % QTime::currentTime().toString("hh:mm:ss.zzz") %
                                    QStringLiteral(" ") % elevationPlane.toQString(true);
            m_elvHistoryModel->insertRow(0);
            const QModelIndex index = m_elvHistoryModel->index(0, 0);
            m_elvHistoryModel->setData(index, history);

            const int c = m_elvHistoryModel->rowCount();
            if (m_elvHistoryCount < c)
            {
                m_elvHistoryModel->removeRows(m_elvHistoryCount, c - m_elvHistoryCount);
            }
        }

        // not for a real plane, but to get elevation at any position for testing
        if (callsign == CInterpolationLogDisplay::pseudoCallsignElevation())
        {
            this->displayArbitraryElevation(elevationPlane);
            return;
        }

        if (!this->logCallsign(callsign)) { return; }

        // for logged callsign
        m_elvReceivedLoggedCs++;
        ui->le_Elevation->setText(elevationPlane.toQString());
        this->displayElevationRequestReceive();
        ui->led_Elevation->blink();
    }

    void CInterpolationLogDisplay::onElevationRequested(const CCallsign &callsign)
    {
        m_elvRequested++;
        if (!this->logCallsign(callsign)) { return; }

        // for logged callsign
        m_elvRequestedLoggedCs++;
        this->displayElevationRequestReceive();
        ui->led_Elevation->blink();
    }

    void CInterpolationLogDisplay::onInjectElevation()
    {
        if (!this->canLog()) { return; }
        const QString elv = ui->le_InjectElevation->text().trimmed();
        if (elv.isEmpty()) { return; }

        const CAircraftSituationList situations = m_airspaceMonitor->remoteAircraftSituations(m_callsign);
        if (situations.isEmpty()) { return; }

        CAltitude alt;
        alt.parseFromString(elv, CPqString::SeparatorBestGuess);
        const CElevationPlane ep(situations.latestAdjustedObject(), alt, CElevationPlane::singlePointRadius());

        // inject as received from simulator
        m_simulator->callbackReceivedRequestedElevation(ep, m_callsign, false);
    }

    void CInterpolationLogDisplay::onElevationHistoryCountFinished()
    {
        const QString cs = ui->le_ElvHistoryCount->text().trimmed();
        int c = -1;
        if (!cs.isEmpty())
        {
            bool ok = false;
            const int cc = cs.toInt(&ok);
            if (ok) { c = cc; }
        }
        m_elvHistoryCount = c;
    }

    void CInterpolationLogDisplay::resetStatistics()
    {
        if (m_simulator) { m_simulator->resetAircraftStatistics(); }
    }

    void CInterpolationLogDisplay::resetLastSentValues()
    {
        if (m_simulator) { m_simulator->resetLastSentValues(); }
    }

    void CInterpolationLogDisplay::clear()
    {
        ui->tvp_InboundAircraftParts->clear();
        ui->tvp_InboundAircraftSituations->clear();
        ui->te_TextLog->clear();
        ui->le_CG->clear();
        ui->le_Elevation->clear();
        ui->le_ElevationReqRec->clear();
        ui->le_ElevationReqRec->setToolTip("elevation requested");
        ui->le_Parts->clear();
        ui->le_UpdateTimes->clear();
        ui->le_UpdateTimes->clear();
        ui->le_Limited->clear();
        m_elvReceivedLoggedCs = m_elvRequestedLoggedCs = 0;
        m_elvReceived = m_elvRequested = 0;
        m_lastInterpolations.clear();

        this->clearElevationResults();
    }

    void CInterpolationLogDisplay::clearElevationResults()
    {
        ui->pte_ElevationAtPosition->clear();
        if (m_elvHistoryModel)
        {
            m_elvHistoryModel->removeRows(0, m_elvHistoryModel->rowCount());
        }
    }

    void CInterpolationLogDisplay::logPosCommand()
    {
        if (m_callsign.isEmpty()) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        const QString cmd = QStringLiteral(".drv logint clear") % m_callsign.asString();
        sGui->getIContextSimulator()->parseCommandLine(cmd, this->identifier());
    }

    void CInterpolationLogDisplay::clearLogCommand()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        const QString cmd = QStringLiteral(".drv logint clear");
        sGui->getIContextSimulator()->parseCommandLine(cmd, this->identifier());
    }

    void CInterpolationLogDisplay::writeLogCommand()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }

        const QString cmd = QStringLiteral(".drv logint write");
        sGui->getIContextSimulator()->parseCommandLine(cmd, this->identifier());
    }

    bool CInterpolationLogDisplay::checkLogPrerequisites()
    {
        CStatusMessage m;
        do
        {
            if (!this->isVisible()) { return false; } // silently return
            if (!sGui || sGui->isShuttingDown()) { break; } // stop and return
            if (m_callsign.isEmpty())
            {
                // static const CStatusMessage ms = CStatusMessage(this).validationError(u"No callsign for logging");
                // m = ms;
                break;
            }

            const bool canUpdateLog = m_airspaceMonitor && m_simulator && m_simulator->isConnected() && !m_simulator->isShuttingDown();
            if (!canUpdateLog)
            {
                static const CStatusMessage ms = CStatusMessage(this).validationError(u"No airspace monitor or simulator or shutting down");
                m = ms;
                break;
            }

            if (!m_simulator->isLogCallsign(m_callsign))
            {
                m = CStatusMessage(this).validationError(u"No longer logging callsign '%1'") << m_callsign;
                break;
            }
            return true;
        }
        while (false);

        this->stop();
        if (!m.isEmpty()) { this->showOverlayMessage(m, 5000); }
        return false;
    }

    void CInterpolationLogDisplay::initPartsView()
    {
        // it can take a while until we receive parts, so we init
        QPointer<CInterpolationLogDisplay> myself(this);
        QTimer::singleShot(250, this, [=] {
            if (!myself) { return; }
            if (m_callsign.isEmpty()) { return; }
            myself->onPartsAdded(m_callsign, CAircraftParts());
        });
    }

    void CInterpolationLogDisplay::displayElevationRequestReceive()
    {
        if (!m_airspaceMonitor) { return; }
        static const QString info("req. %1, %2/rec. %3, %4 | found/missed: '%5' | times: %6");
        const QString foundMissed = m_airspaceMonitor->getElevationsFoundMissedInfo();
        const QString reqTimes = m_airspaceMonitor->getElevationRequestTimesInfo();
        const QString reqRec = info.arg(m_elvRequestedLoggedCs).arg(m_elvRequested).arg(m_elvReceivedLoggedCs).arg(m_elvReceived).arg(foundMissed, reqTimes);

        ui->le_ElevationReqRec->setText(reqRec);
        ui->le_ElevationReqRec->setToolTip(reqRec);
        ui->le_ElevationReqRec->home(false);
    }

    void CInterpolationLogDisplay::displayArbitraryElevation(const CElevationPlane &elevation)
    {
        ui->pte_ElevationAtPosition->appendPlainText(elevation.toQString());
    }

    void CInterpolationLogDisplay::requestElevationClicked()
    {
        if (m_callsign.isEmpty()) { return; }
        const CAircraftSituationList situations = m_airspaceMonitor->remoteAircraftSituations(m_callsign);
        if (situations.isEmpty()) { return; }
        this->requestElevation(situations.latestAdjustedObject());
    }

    void CInterpolationLogDisplay::requestElevation(const CAircraftSituation &situation)
    {
        if (!this->canLog()) { return; }
        m_simulator->requestElevationBySituation(situation);
    }

    void CInterpolationLogDisplay::requestElevationAtPosition()
    {
        if (!this->canLog()) { return; }
        const CCoordinateGeodetic coordinate = ui->editor_ElevationCoordinate->getCoordinate();
        const bool ok = m_simulator->requestElevation(coordinate, CInterpolationLogDisplay::pseudoCallsignElevation());
        if (ok)
        {
            static const QString info("Requesting elevation: %1");
            ui->pte_ElevationAtPosition->setPlainText(info.arg(coordinate.toQString()));
        }
        else
        {
            static const QString info("Cannot request elevation");
            ui->pte_ElevationAtPosition->setPlainText(info);
        }
    }

    void CInterpolationLogDisplay::requestRecalculateAll()
    {
        if (!this->canLog()) { return; }
        sApp->getIContextSimulator()->recalculateAllAircraft();
    }

    bool CInterpolationLogDisplay::canLog() const
    {
        return (sApp && !sApp->isShuttingDown() && sApp->getIContextSimulator() && m_simulator);
    }

    const CCallsign &CInterpolationLogDisplay::pseudoCallsignElevation()
    {
        static const CCallsign cs("SW1LOX");
        return cs;
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

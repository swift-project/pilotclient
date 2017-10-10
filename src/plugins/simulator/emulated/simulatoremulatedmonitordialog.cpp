/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatoremulatedmonitordialog.h"
#include "simulatoremulated.h"
#include "ui_simulatoremulatedmonitordialog.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackGui;
using namespace BlackGui::Editors;

namespace BlackSimPlugin
{
    namespace Emulated
    {
        const CLogCategoryList &CSimulatorEmulatedMonitorDialog::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::driver(), CLogCategory::plugin() };
            return cats;
        }

        CSimulatorEmulatedMonitorDialog::CSimulatorEmulatedMonitorDialog(CSimulatorEmulated *simulator, QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CSimulatorEmulatedMonitorDialog)
        {
            Q_ASSERT_X(simulator, Q_FUNC_INFO, "Need simulator");

            ui->setupUi(this);
            ui->tw_SwiftMonitorDialog->setCurrentIndex(0);
            ui->comp_LogComponent->setMaxLogMessages(500);
            this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

            m_simulator = simulator;
            m_uiUpdateTimer.setObjectName(this->objectName() + ":uiUpdateTimer");
            m_uiUpdateTimer.start(2.5 * 1000);
            connect(m_simulator, &CSimulatorEmulated::internalAircraftChanged, this, &CSimulatorEmulatedMonitorDialog::setInternalAircraftUiValues, Qt::QueuedConnection);
            connect(&m_uiUpdateTimer, &QTimer::timeout, this, &CSimulatorEmulatedMonitorDialog::timerBasedUiUpdates);
            connect(ui->pb_ResetStatistics, &QPushButton::clicked, this, &CSimulatorEmulatedMonitorDialog::resetStatistics);

            connect(ui->cb_Connected, &QCheckBox::released, this, &CSimulatorEmulatedMonitorDialog::onSimulatorValuesChanged);
            connect(ui->cb_Paused, &QCheckBox::released, this, &CSimulatorEmulatedMonitorDialog::onSimulatorValuesChanged);
            connect(ui->cb_Simulating, &QCheckBox::released, this, &CSimulatorEmulatedMonitorDialog::onSimulatorValuesChanged);

            connect(ui->editor_Situation, &CSituationForm::changeAircraftSituation, this, &CSimulatorEmulatedMonitorDialog::changeSituationFromUi);
            connect(ui->editor_AircraftParts, &CAircraftPartsForm::changeAircraftParts, this, &CSimulatorEmulatedMonitorDialog::changePartsFromUi);
            connect(ui->editor_Com, &CCockpitComForm::changedCockpitValues, this, &CSimulatorEmulatedMonitorDialog::changeComFromUi);
            connect(ui->editor_Com, &CCockpitComForm::changedSelcal, this, &CSimulatorEmulatedMonitorDialog::changeSelcalFromUi);

            ui->wi_LedReceiving->setToolTips("receiving", "idle");
            ui->wi_LedReceiving->setShape(CLedWidget::Rounded);
            ui->wi_LedSending->setToolTips("sending", "idle");
            ui->wi_LedSending->setShape(CLedWidget::Rounded);

            this->setSimulatorUiValues();
            this->setInternalAircraftUiValues();
        }

        CSimulatorEmulatedMonitorDialog::~CSimulatorEmulatedMonitorDialog()
        { }

        void CSimulatorEmulatedMonitorDialog::appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage)
        {
            ui->comp_LogComponent->appendStatusMessagesToList(statusMessage);
        }

        void CSimulatorEmulatedMonitorDialog::appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages)
        {
            ui->comp_LogComponent->appendStatusMessagesToList(statusMessages);
        }

        void CSimulatorEmulatedMonitorDialog::appendReceivingCall(const QString &function, const QString &p1, const QString &p2, const QString &p3)
        {
            ui->wi_LedReceiving->blink();
            this->appendFunctionCall(function, p1, p2, p3);
        }

        void CSimulatorEmulatedMonitorDialog::appendSendingCall(const QString &function, const QString &p1, const QString &p2, const QString &p3)
        {
            ui->wi_LedSending->blink();
            this->appendFunctionCall(function, p1, p2, p3);
        }

        void CSimulatorEmulatedMonitorDialog::appendFunctionCall(const QString &function, const QString &p1, const QString &p2, const QString &p3)
        {
            static const QString c1("%1 %2");
            static const QString c2("%1 %2 %3");
            static const QString c3("%1 %2 %3 %4");

            CStatusMessage msg;
            if (!p3.isEmpty())
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityInfo, c3.arg(function, p1, p2, p3));
            }
            else if (!p2.isEmpty())
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityInfo, c2.arg(function, p1, p2));
            }
            else if (!p1.isEmpty())
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityInfo, c1.arg(function, p1));
            }
            else
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityInfo, function);
            }
            this->appendStatusMessageToList(msg);
        }

        void CSimulatorEmulatedMonitorDialog::displayStatusMessage(const CStatusMessage &message)
        {
            ui->pte_StatusMessages->setPlainText(message.toQString(true));
        }

        void CSimulatorEmulatedMonitorDialog::displayTextMessage(const Network::CTextMessage &message)
        {
            ui->pte_TextMessages->setPlainText(message.toQString(true));
        }

        void CSimulatorEmulatedMonitorDialog::onSimulatorValuesChanged()
        {
            m_simulator->setCombinedStatus(
                ui->cb_Connected->isChecked(),
                ui->cb_Simulating->isChecked(),
                ui->cb_Paused->isChecked()
            );
        }

        void CSimulatorEmulatedMonitorDialog::changeComFromUi(const CSimulatedAircraft &aircraft)
        {
            if (!m_simulator) { return; }
            m_simulator->changeInternalCom(aircraft);
        }

        void CSimulatorEmulatedMonitorDialog::changeSelcalFromUi(const CSelcal &selcal)
        {
            if (!m_simulator) { return; }
            m_simulator->changeInternalSelcal(selcal);
        }

        void CSimulatorEmulatedMonitorDialog::changeSituationFromUi()
        {
            if (!m_simulator) { return; }
            const CAircraftSituation s(ui->editor_Situation->getSituation());
            m_simulator->changeInternalSituation(s);
        }

        void CSimulatorEmulatedMonitorDialog::changePartsFromUi()
        {
            if (!m_simulator) { return; }
            const CAircraftParts p(ui->editor_AircraftParts->getAircraftPartsFromGui());
            m_simulator->changeInternalParts(p);
        }

        void CSimulatorEmulatedMonitorDialog::setSimulatorUiValues()
        {
            ui->cb_Connected->setChecked(m_simulator->isConnected());
            ui->cb_Paused->setChecked(m_simulator->isPaused());
            ui->cb_Simulating->setChecked(m_simulator->isSimulating());

            ui->le_Simulator->setText(m_simulator->getSimulatorInfo().toQString(true));
            ui->le_SimulatorPlugin->setText(m_simulator->getSimulatorPluginInfo().toQString(true));
        }

        void CSimulatorEmulatedMonitorDialog::setInternalAircraftUiValues()
        {
            const CSimulatedAircraft internal(m_simulator->getInternalOwnAircraft());
            ui->editor_Situation->setSituation(internal.getSituation());
            ui->editor_AircraftParts->setAircraftParts(internal.getParts());
            ui->editor_Com->setValue(internal);
        }

        void CSimulatorEmulatedMonitorDialog::timerBasedUiUpdates()
        {
            if (!m_simulator) { return; }
            ui->le_PhysicallyAddedAircraft->setText(QString::number(m_simulator->m_physicallyAdded));
            ui->le_PhysicallyRemovedAircraft->setText(QString::number(m_simulator->m_physicallyRemoved));
            ui->le_SituationAdded->setText(QString::number(m_simulator->m_situationAdded));
            ui->le_PartsAdded->setText(QString::number(m_simulator->m_partsAdded));
            ui->le_AircraftRendered->setText(QString::number(m_simulator->m_renderedAircraft.size()));
            ui->le_PartsEnabledAircraft->setText(QString::number(m_simulator->getRemoteAircraftSupportingPartsCount()));
        }

        void CSimulatorEmulatedMonitorDialog::resetStatistics()
        {
            if (!m_simulator) { m_simulator->resetStatistics(); }
            ui->le_PhysicallyAddedAircraft->clear();
            ui->le_PhysicallyRemovedAircraft->clear();
            ui->le_SituationAdded->clear();
            ui->le_PartsAdded->clear();
        }
    } // ns
} // ns

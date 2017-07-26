/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorswiftmonitordialog.h"
#include "simulatorswift.h"
#include "ui_simulatorswiftmonitordialog.h"

using namespace BlackMisc;

namespace BlackSimPlugin
{
    namespace Swift
    {
        const CLogCategoryList &CSimulatorSwiftMonitorDialog::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::driver(), CLogCategory::plugin() };
            return cats;
        }

        CSimulatorSwiftMonitorDialog::CSimulatorSwiftMonitorDialog(CSimulatorSwift *simulator, QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CSimulatorSwiftMonitorDialog)
        {
            Q_ASSERT_X(simulator, Q_FUNC_INFO, "Need simulator");

            m_simulator = simulator;
            ui->setupUi(this);
            ui->tw_SwiftMonitorDialog->setCurrentIndex(0);
            this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

            connect(ui->cb_Connected, &QCheckBox::released, this, &CSimulatorSwiftMonitorDialog::onSimulatorValuesChanged);
            connect(ui->cb_Paused, &QCheckBox::released, this, &CSimulatorSwiftMonitorDialog::onSimulatorValuesChanged);
            connect(ui->cb_Simulating, &QCheckBox::released, this, &CSimulatorSwiftMonitorDialog::onSimulatorValuesChanged);

            this->setSimulatorUiValues();
            ui->comp_Position->setCoordinate(m_simulator->getOwnAircraftSituation());
        }

        CSimulatorSwiftMonitorDialog::~CSimulatorSwiftMonitorDialog()
        { }

        void CSimulatorSwiftMonitorDialog::appendStatusMessageToList(const BlackMisc::CStatusMessage &statusMessage)
        {
            ui->comp_LogComponent->appendStatusMessagesToList(statusMessage);
        }

        void CSimulatorSwiftMonitorDialog::appendStatusMessagesToList(const BlackMisc::CStatusMessageList &statusMessages)
        {
            ui->comp_LogComponent->appendStatusMessagesToList(statusMessages);
        }

        void CSimulatorSwiftMonitorDialog::appendFunctionCall(const QString &function, const QString &p1, const QString &p2, const QString &p3)
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

        void CSimulatorSwiftMonitorDialog::onSimulatorValuesChanged()
        {
            m_simulator->setCombinedStatus(
                ui->cb_Connected->isChecked(),
                ui->cb_Simulating->isChecked(),
                ui->cb_Paused->isChecked()
            );
        }

        void CSimulatorSwiftMonitorDialog::setSimulatorUiValues()
        {
            ui->cb_Connected->setChecked(m_simulator->isConnected());
            ui->cb_Paused->setChecked(m_simulator->isPaused());
            ui->cb_Simulating->setChecked(m_simulator->isSimulating());

            ui->le_Simulator->setText(m_simulator->getSimulatorInfo().toQString(true));
            ui->le_SimulatorPlugin->setText(m_simulator->getSimulatorPluginInfo().toQString(true));
        }
    } // ns
} // ns

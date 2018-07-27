/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorxplaneconfigwindow.h"
#include "blackcore/application.h"
#include "ui_simulatorxplaneconfigwindow.h"

#include <QComboBox>
#include <QDialogButtonBox>

using namespace BlackGui;
using namespace BlackMisc;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CSimulatorXPlaneConfigWindow::CSimulatorXPlaneConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorXPlaneConfigWindow)
        {
            ui->setupUi(this);

            ui->comp_SettingsXSwiftBus->set(m_xswiftbusServerSetting.getThreadLocal());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorXPlaneConfigWindow::settingsAccepted);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorXPlaneConfigWindow::close);
        }

        CSimulatorXPlaneConfigWindow::~CSimulatorXPlaneConfigWindow()
        { }

        void CSimulatorXPlaneConfigWindow::settingsAccepted()
        {
            QString currentAddress = m_xswiftbusServerSetting.getThreadLocal();
            if (currentAddress != ui->comp_SettingsXSwiftBus->getDBusAddress())
            {
                m_xswiftbusServerSetting.set(ui->comp_SettingsXSwiftBus->getDBusAddress());
            }
            close();
        }

    } // ns
} // ns

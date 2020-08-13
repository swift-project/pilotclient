/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorflightgearconfigwindow.h"
#include "blackcore/application.h"
#include "ui_simulatorflightgearconfigwindow.h"

#include <QComboBox>
#include <QDialogButtonBox>

using namespace BlackGui;
using namespace BlackMisc;
//using namespace BlackMisc::Simulation::Flightgear;

namespace BlackSimPlugin
{
    namespace Flightgear
    {
        CSimulatorFlightgearConfigWindow::CSimulatorFlightgearConfigWindow(QWidget *parent) :
            CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorFlightgearConfigWindow)
        {
            ui->setupUi(this);
            CGuiUtility::disableMinMaxCloseButtons(this);
            ui->comp_SettingsFGSwiftBus->setDefaultP2PAddress(m_fgswiftbusServerSetting.getDefault());
            ui->comp_SettingsFGSwiftBus->set(m_fgswiftbusServerSetting.getThreadLocal());

            connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorFlightgearConfigWindow::onSettingsAccepted);
            connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorFlightgearConfigWindow::close);
        }

        CSimulatorFlightgearConfigWindow::~CSimulatorFlightgearConfigWindow()
        { }

        void CSimulatorFlightgearConfigWindow::onSettingsAccepted()
        {
            const QString currentAddress = m_fgswiftbusServerSetting.getThreadLocal();
            const QString updatedAddress = ui->comp_SettingsFGSwiftBus->getDBusAddress();
            if (currentAddress != ui->comp_SettingsFGSwiftBus->getDBusAddress())
            {
                m_fgswiftbusServerSetting.set(updatedAddress);
            }
            close();
        }
    } // ns
} // ns

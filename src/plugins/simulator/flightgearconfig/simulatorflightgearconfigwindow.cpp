/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_simulatorflightgearconfigwindow.h"
#include "simulatorflightgearconfigwindow.h"
#include "blackcore/application.h"
#include "blackgui/guiapplication.h"

#include <QComboBox>
#include <QDialogButtonBox>

using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Simulation::Settings;
//using namespace BlackMisc::Simulation::Flightgear;

namespace BlackSimPlugin::Flightgear
{
    CSimulatorFlightgearConfigWindow::CSimulatorFlightgearConfigWindow(QWidget *parent) :
        CPluginConfigWindow(parent),
        ui(new Ui::CSimulatorFlightgearConfigWindow)
    {
        ui->setupUi(this);
        CGuiUtility::disableMinMaxCloseButtons(this);
        ui->comp_SettingsFGSwiftBus->setDefaultP2PAddress(m_fgswiftbusServerSetting.getDefault().getDBusServerAddress());
        ui->comp_SettingsFGSwiftBus->set(m_fgswiftbusServerSetting.getThreadLocal().getDBusServerAddress());

        connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this, &CSimulatorFlightgearConfigWindow::onSettingsAccepted);
        connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorFlightgearConfigWindow::close);
    }

    CSimulatorFlightgearConfigWindow::~CSimulatorFlightgearConfigWindow()
    { }

    CFGSwiftBusSettings CSimulatorFlightgearConfigWindow::getSettingsFromUI() const
    {
        CFGSwiftBusSettings s = m_fgswiftbusServerSetting.getThreadLocal();
        s.setDBusServerAddress(ui->comp_SettingsFGSwiftBus->getDBusAddress());
        return s;
    }

    void CSimulatorFlightgearConfigWindow::onSettingsChanged()
    {
        ui->comp_SettingsFGSwiftBus->set(m_fgswiftbusServerSetting.getThreadLocal().getDBusServerAddress());
    }

    void CSimulatorFlightgearConfigWindow::onSettingsAccepted()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        const CFGSwiftBusSettings s = m_fgswiftbusServerSetting.getThreadLocal();
        CFGSwiftBusSettings changed = getSettingsFromUI();

        if (s != changed)
        {
            m_fgswiftbusServerSetting.set(changed);
        }
        close();
    }
} // ns

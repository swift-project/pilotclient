// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorflightgearconfigwindow.h"

#include <QComboBox>
#include <QDialogButtonBox>

#include "ui_simulatorflightgearconfigwindow.h"

#include "core/application.h"
#include "gui/guiapplication.h"

using namespace swift::gui;
using namespace swift::misc;
using namespace swift::misc::simulation::settings;
// using namespace swift::misc::simulation::Flightgear;

namespace swift::simplugin::flightgear
{
    CSimulatorFlightgearConfigWindow::CSimulatorFlightgearConfigWindow(QWidget *parent)
        : CPluginConfigWindow(parent), ui(new Ui::CSimulatorFlightgearConfigWindow)
    {
        ui->setupUi(this);
        CGuiUtility::disableMinMaxCloseButtons(this);
        ui->comp_SettingsFGSwiftBus->setDefaultP2PAddress(
            m_fgswiftbusServerSetting.getDefault().getDBusServerAddress());
        ui->comp_SettingsFGSwiftBus->set(m_fgswiftbusServerSetting.getThreadLocal().getDBusServerAddress());

        connect(ui->bb_OkCancel, &QDialogButtonBox::accepted, this,
                &CSimulatorFlightgearConfigWindow::onSettingsAccepted);
        connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &CSimulatorFlightgearConfigWindow::close);
    }

    CSimulatorFlightgearConfigWindow::~CSimulatorFlightgearConfigWindow() = default;

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

        if (s != changed) { m_fgswiftbusServerSetting.set(changed); }
        close();
    }
} // namespace swift::simplugin::flightgear

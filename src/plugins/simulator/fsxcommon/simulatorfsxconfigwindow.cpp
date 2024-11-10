// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfsxconfigwindow.h"
#include "ui_simulatorfsxconfigwindow.h"

using namespace swift::gui;

namespace BlackSimPlugin::FsxCommon
{
    CSimulatorFsxConfigWindow::CSimulatorFsxConfigWindow(const QString &simulator, QWidget *parent) : CPluginConfigWindow(parent),
                                                                                                      m_simulator(simulator),
                                                                                                      ui(new Ui::CSimulatorFsxConfigWindow)
    {
        ui->setupUi(this);
        ui->tw_Settings->setCurrentIndex(0);
        ui->comp_FsxSettings->setSimulator(m_simulator);
        connect(ui->bb_OkCancel, &QDialogButtonBox::rejected, this, &QWidget::close);
        this->setWindowTitle(m_simulator.toQString(true) + " plugin configuration");
        CGuiUtility::disableMinMaxCloseButtons(this);
    }

    CSimulatorFsxConfigWindow::~CSimulatorFsxConfigWindow()
    {
        // void
    }
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatoremulatedconfigwidget.h"

#include "ui_simulatoremulatedconfigwidget.h"

using namespace swift::gui;

namespace swift::simplugin::emulated
{
    CSimulatorEmulatedConfigWidget::CSimulatorEmulatedConfigWidget(QWidget *parent) : swift::gui::CPluginConfigWindow(parent),
                                                                                      ui(new Ui::CSimulatorEmulatedConfigWidget)
    {
        ui->setupUi(this);
        CGuiUtility::disableMinMaxCloseButtons(this);
        connect(ui->bb_Close, &QDialogButtonBox::rejected, this, &CSimulatorEmulatedConfigWidget::close);
    }

    CSimulatorEmulatedConfigWidget::~CSimulatorEmulatedConfigWidget()
    {}
} // namespace swift::simplugin::emulated

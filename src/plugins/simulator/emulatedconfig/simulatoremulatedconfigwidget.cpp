/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatoremulatedconfigwidget.h"
#include "ui_simulatoremulatedconfigwidget.h"

using namespace BlackGui;

namespace BlackSimPlugin
{
    namespace Emulated
    {
        CSimulatorEmulatedConfigWidget::CSimulatorEmulatedConfigWidget(QWidget *parent) :
            BlackGui::CPluginConfigWindow(parent),
            ui(new Ui::CSimulatorEmulatedConfigWidget)
        {
            ui->setupUi(this);
            CGuiUtility::disableMinMaxCloseButtons(this);
            connect(ui->bb_Close, &QDialogButtonBox::rejected, this, &CSimulatorEmulatedConfigWidget::close);
        }

        CSimulatorEmulatedConfigWidget::~CSimulatorEmulatedConfigWidget()
        { }
    } // ns
} // ns

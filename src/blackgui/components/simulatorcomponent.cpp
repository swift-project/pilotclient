/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorcomponent.h"
#include "ui_simulatorcomponent.h"

namespace BlackGui
{
    namespace Components
    {
        CSimulatorComponent::CSimulatorComponent(QWidget *parent) :
            QTabWidget(parent), ui(new Ui::CSimulatorComponent)
        {
            ui->setupUi(this);
        }

        CSimulatorComponent::~CSimulatorComponent()
        {
            delete ui;
        }
    }
}

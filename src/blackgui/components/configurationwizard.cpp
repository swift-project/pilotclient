/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "configurationwizard.h"
#include "ui_configurationwizard.h"

namespace BlackGui
{
    namespace Components
    {
        CConfigurationWizard::CConfigurationWizard(QWidget *parent) :
            QWizard(parent),
            ui(new Ui::CConfigurationWizard)
        {
            ui->setupUi(this);
        }

        CConfigurationWizard::~CConfigurationWizard()
        { }
    } // ns
} // ns

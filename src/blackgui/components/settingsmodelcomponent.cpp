/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsmodelcomponent.h"
#include "ui_settingsmodelcomponent.h"

namespace BlackGui
{
    namespace Components
    {
        CSettingsModelComponent::CSettingsModelComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsModelComponent)
        {
            ui->setupUi(this);
        }

        CSettingsModelComponent::~CSettingsModelComponent()
        { }
    } // ns
} // ns

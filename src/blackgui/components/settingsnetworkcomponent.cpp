/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
#include "blackgui/components/settingsnetworkcomponent.h"
#include "blackgui/guiapplication.h"
#include "ui_settingsnetworkcomponent.h"

#include <QCheckBox>

using namespace BlackCore;

namespace BlackGui::Components
{
    CSettingsNetworkComponent::CSettingsNetworkComponent(QWidget *parent) : QFrame(parent),
                                                                            ui(new Ui::CSettingsNetworkComponent)
    {
        ui->setupUi(this);
    }

    CSettingsNetworkComponent::~CSettingsNetworkComponent() {}
} // ns

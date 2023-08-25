// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

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

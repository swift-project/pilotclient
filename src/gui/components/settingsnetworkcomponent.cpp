// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextnetwork.h"
#include "gui/components/settingsnetworkcomponent.h"
#include "gui/guiapplication.h"
#include "ui_settingsnetworkcomponent.h"

#include <QCheckBox>

using namespace swift::core;

namespace swift::gui::components
{
    CSettingsNetworkComponent::CSettingsNetworkComponent(QWidget *parent) : QFrame(parent),
                                                                            ui(new Ui::CSettingsNetworkComponent)
    {
        ui->setupUi(this);
    }

    CSettingsNetworkComponent::~CSettingsNetworkComponent() {}
} // ns

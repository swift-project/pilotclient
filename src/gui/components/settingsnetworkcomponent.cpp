// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/settingsnetworkcomponent.h"

#include <QCheckBox>

#include "ui_settingsnetworkcomponent.h"

#include "core/context/contextnetwork.h"
#include "gui/guiapplication.h"

using namespace swift::core;

namespace swift::gui::components
{
    CSettingsNetworkComponent::CSettingsNetworkComponent(QWidget *parent)
        : QFrame(parent), ui(new Ui::CSettingsNetworkComponent)
    {
        ui->setupUi(this);
    }

    CSettingsNetworkComponent::~CSettingsNetworkComponent() {}
} // namespace swift::gui::components

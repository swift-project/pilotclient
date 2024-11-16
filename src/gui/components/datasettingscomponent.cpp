// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/datasettingscomponent.h"

#include "ui_datasettingscomponent.h"

using namespace swift::gui;
using namespace swift::core::db;

namespace swift::gui::components
{
    CDataSettingsComponent::CDataSettingsComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDataSettingsComponent)
    {
        ui->setupUi(this);
        ui->comp_GuiSettings->hideOpacity(true);
    }

    CDataSettingsComponent::~CDataSettingsComponent() {}

    void CDataSettingsComponent::setBackgroundUpdater(const CBackgroundDataUpdater *updater)
    {
        ui->comp_ModelSettings->setBackgroundUpdater(updater);
    }
} // namespace swift::gui::components

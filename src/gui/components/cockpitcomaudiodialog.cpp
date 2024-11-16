// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "cockpitcomaudiodialog.h"

#include "ui_cockpitcomaudiodialog.h"

namespace swift::gui::components
{
    CCockpitComAudioDialog::CCockpitComAudioDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CCockpitComAudioDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CCockpitComAudioDialog::~CCockpitComAudioDialog() {}

} // namespace swift::gui::components

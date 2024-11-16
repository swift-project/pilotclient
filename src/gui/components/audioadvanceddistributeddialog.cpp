// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "audioadvanceddistributeddialog.h"

#include "ui_audioadvanceddistributeddialog.h"

namespace swift::gui::components
{
    CAudioAdvancedDistributedDialog::CAudioAdvancedDistributedDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::CAudioAdvancedDistributedDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->reloadRegisteredDevices();
    }

    CAudioAdvancedDistributedDialog::~CAudioAdvancedDistributedDialog() {}

    void CAudioAdvancedDistributedDialog::reloadRegisteredDevices()
    {
        ui->comp_AudioAdvancedDistributed->reloadRegisteredDevices();
    }

} // namespace swift::gui::components

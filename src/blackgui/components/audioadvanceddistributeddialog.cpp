/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "audioadvanceddistributeddialog.h"
#include "ui_audioadvanceddistributeddialog.h"

namespace BlackGui::Components
{
    CAudioAdvancedDistributedDialog::CAudioAdvancedDistributedDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CAudioAdvancedDistributedDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        this->reloadRegisteredDevices();
    }

    CAudioAdvancedDistributedDialog::~CAudioAdvancedDistributedDialog()
    { }

    void CAudioAdvancedDistributedDialog::reloadRegisteredDevices()
    {
        ui->comp_AudioAdvancedDistributed->reloadRegisteredDevices();
    }

} // ns

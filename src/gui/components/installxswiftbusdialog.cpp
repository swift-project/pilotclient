// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "installxswiftbusdialog.h"

#include "ui_installxswiftbusdialog.h"

namespace swift::gui::components
{
    CInstallXSwiftBusDialog::CInstallXSwiftBusDialog(QWidget *parent) : QDialog(parent),
                                                                        ui(new Ui::CInstallXSwiftBusDialog)
    {
        ui->setupUi(this);
    }

    CInstallXSwiftBusDialog::~CInstallXSwiftBusDialog()
    {}

    void CInstallXSwiftBusDialog::setDefaultDownloadName(const QString &defaultName)
    {
        ui->comp_InstallXSwiftBus->setDefaultDownloadName(defaultName);
    }
} // namespace swift::gui::components

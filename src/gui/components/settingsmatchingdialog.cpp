// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "settingsmatchingdialog.h"
#include "ui_settingsmatchingdialog.h"

using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CSettingsMatchingDialog::CSettingsMatchingDialog(QWidget *parent) : QDialog(parent),
                                                                        ui(new Ui::CSettingsMatchingDialog)
    {
        ui->setupUi(this);
    }

    CSettingsMatchingDialog::~CSettingsMatchingDialog()
    {}

    CAircraftMatcherSetup CSettingsMatchingDialog::getMatchingSetup() const
    {
        return ui->comp_Settings->getMatchingSetup();
    }

    void CSettingsMatchingDialog::setMatchingSetup(const CAircraftMatcherSetup &setup)
    {
        ui->comp_Settings->setMatchingSetup(setup);
    }
} // ns

/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "settingsmatchingdialog.h"
#include "ui_settingsmatchingdialog.h"

using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CSettingsMatchingDialog::CSettingsMatchingDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CSettingsMatchingDialog)
    {
        ui->setupUi(this);
    }

    CSettingsMatchingDialog::~CSettingsMatchingDialog()
    { }

    CAircraftMatcherSetup CSettingsMatchingDialog::getMatchingSetup() const
    {
        return ui->comp_Settings->getMatchingSetup();
    }

    void CSettingsMatchingDialog::setMatchingSetup(const CAircraftMatcherSetup &setup)
    {
        ui->comp_Settings->setMatchingSetup(setup);
    }
} // ns

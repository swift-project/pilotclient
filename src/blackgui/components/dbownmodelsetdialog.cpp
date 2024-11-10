// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbownmodelsetdialog.h"
#include "ui_dbownmodelsetdialog.h"

using namespace swift::misc::simulation;

namespace BlackGui::Components
{
    CDbOwnModelSetDialog::CDbOwnModelSetDialog(QWidget *parent) : QDialog(parent),
                                                                  ui(new Ui::CDbOwnModelSetDialog)
    {
        ui->setupUi(this);
        ui->comp_OwnModelSet->setSimulatorSelectorMode(CSimulatorSelector::ComboBox);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CDbOwnModelSetDialog::~CDbOwnModelSetDialog()
    {}

    void CDbOwnModelSetDialog::setSimulator(const CSimulatorInfo &simulator)
    {
        ui->comp_OwnModelSet->setSimulator(simulator);
    }

    const CDbOwnModelSetComponent *CDbOwnModelSetDialog::modelSetComponent() const
    {
        return ui->comp_OwnModelSet;
    }

    CDbOwnModelSetComponent *CDbOwnModelSetDialog::modelSetComponent()
    {
        return ui->comp_OwnModelSet;
    }

    void CDbOwnModelSetDialog::enableButtons(bool firstSet, bool newSet)
    {
        ui->comp_OwnModelSet->enableButtons(firstSet, newSet);
    }
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "dbownmodelsdialog.h"

#include "ui_dbownmodelsdialog.h"

#include "gui/components/dbownmodelscomponent.h"

using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CDbOwnModelsDialog::CDbOwnModelsDialog(QWidget *parent) : QDialog(parent),
                                                              ui(new Ui::CDbOwnModelsDialog)
    {
        ui->setupUi(this);
        ui->comp_OwnModels->setSimulatorSelectorMode(CSimulatorSelector::ComboBox);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        connect(ui->comp_OwnModels, &CDbOwnModelsComponent::successfullyLoadedModels, this, &CDbOwnModelsDialog::successfullyLoadedModels);
    }

    CDbOwnModelsDialog::~CDbOwnModelsDialog()
    {}

    void CDbOwnModelsDialog::setSimulator(const CSimulatorInfo &simulator)
    {
        ui->comp_OwnModels->setSimulator(simulator);
    }

    bool CDbOwnModelsDialog::requestModelsInBackground(const CSimulatorInfo &simulator, bool onlyIfNotEmpty)
    {
        return ui->comp_OwnModels->requestModelsInBackground(simulator, onlyIfNotEmpty);
    }

    int CDbOwnModelsDialog::getOwnModelsCount() const
    {
        return ui->comp_OwnModels->getOwnModelsCount();
    }

    const CDbOwnModelsComponent *CDbOwnModelsDialog::modelsComponent() const
    {
        return ui->comp_OwnModels;
    }
} // namespace swift::gui::components

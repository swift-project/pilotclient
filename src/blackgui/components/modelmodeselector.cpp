// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/modelmodeselector.h"
#include "ui_modelmodeselector.h"

#include <QRadioButton>

using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CModelModeSelector::CModelModeSelector(QWidget *parent) : QFrame(parent),
                                                              ui(new Ui::CModelModeSelector)
    {
        ui->setupUi(this);
        connect(ui->rb_Include, &QRadioButton::released, this, &CModelModeSelector::changed);
    }

    CModelModeSelector::~CModelModeSelector()
    {
        // void
    }

    BlackMisc::Simulation::CAircraftModel::ModelMode CModelModeSelector::getMode() const
    {
        if (ui->rb_Include->isChecked())
        {
            return CAircraftModel::Include;
        }
        else
        {
            return CAircraftModel::Exclude;
        }
    }

    void CModelModeSelector::setValue(CAircraftModel::ModelMode mode)
    {
        if (mode == CAircraftModel::Include)
        {
            ui->rb_Include->setChecked(true);
        }
        else
        {
            ui->rb_Exclude->setChecked(true);
        }
    }

    void CModelModeSelector::setValue(const CAircraftModel &model)
    {
        this->setValue(model.getModelMode());
    }

    void CModelModeSelector::setReadOnly(bool readOnly)
    {
        this->setEnabled(!readOnly);
    }
} // ns

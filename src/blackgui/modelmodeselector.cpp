/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmodeselector.h"
#include "ui_modelmodeselector.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    CModelModeSelector::CModelModeSelector(QWidget *parent) :
        QFrame(parent),
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
}

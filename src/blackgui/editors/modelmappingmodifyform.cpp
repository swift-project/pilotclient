/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelmappingmodifyform.h"
#include "ui_modelmappingmodifyform.h"
#include "blackmisc/simulation/aircraftmodel.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CModelMappingModifyForm::CModelMappingModifyForm(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CModelMappingModifyForm)
        {
            ui->setupUi(this);
            connect(ui->le_Description, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::ps_returnPressed);
            connect(ui->le_Name, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::ps_returnPressed);
            connect(ui->frp_IncludeSelector, &CModelModeSelector::changed, this, &CModelMappingModifyForm::ps_changed);
            connect(ui->frp_SimulatorSelector, &CSimulatorSelector::changed, this, &CModelMappingModifyForm::ps_changed);
        }

        CModelMappingModifyForm::~CModelMappingModifyForm()
        {
            // void
        }

        CPropertyIndexVariantMap CModelMappingModifyForm::getValues() const
        {
            CPropertyIndexVariantMap vm;
            if (ui->cb_Name->isChecked())
            {
                vm.addValue(CAircraftModel::IndexName, ui->le_Name->text().trimmed());
            }

            if (ui->cb_Description->isChecked())
            {
                vm.addValue(CAircraftModel::IndexDescription, ui->le_Description->text().trimmed());
            }

            if (ui->cb_Simulator->isChecked())
            {
                vm.addValue(CAircraftModel::IndexSimulatorInfo, ui->frp_SimulatorSelector->getValue());
            }

            if (ui->cb_Simulator->isChecked())
            {
                vm.addValue(CAircraftModel::IndexSimulatorInfo, ui->frp_SimulatorSelector->getValue());
            }

            if (ui->cb_Mode->isChecked())
            {
                vm.addValue(CAircraftModel::IndexModelMode, ui->frp_IncludeSelector->getMode());
            }
            return vm;
        }

        void CModelMappingModifyForm::setValue(const CAircraftModel &model)
        {
            this->ui->le_Description->setText(model.getDescription());
            this->ui->le_Name->setText(model.getName());
            this->ui->frp_SimulatorSelector->setValue(model.getSimulator());
            this->ui->frp_IncludeSelector->setValue(model);
        }

        void CModelMappingModifyForm::ps_returnPressed()
        {
            QCheckBox *cb = widgetToCheckbox(sender());
            if (!cb) { return; }
            cb->setChecked(true);
        }

        void CModelMappingModifyForm::ps_changed()
        {
            QCheckBox *cb = widgetToCheckbox(sender());
            if (!cb) { return; }
            cb->setChecked(true);
        }

        QCheckBox *CModelMappingModifyForm::widgetToCheckbox(QObject *widget) const
        {
            if (!widget) { return nullptr; }
            if (widget == ui->le_Description) { return ui->cb_Description; }
            if (widget == ui->le_Name) { return ui->cb_Name; }
            if (widget == ui->frp_IncludeSelector) { return ui->cb_Mode; }
            if (widget == ui->frp_SimulatorSelector) { return ui->cb_Simulator; }
            return nullptr;
        }
    } // ns
} // ns

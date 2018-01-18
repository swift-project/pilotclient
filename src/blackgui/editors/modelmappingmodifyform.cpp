/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/modelmodeselector.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/editors/modelmappingmodifyform.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/network/authenticateduser.h"
#include "ui_modelmappingmodifyform.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QString>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CModelMappingModifyForm::CModelMappingModifyForm(QWidget *parent) :
            CFormDbUser(parent),
            ui(new Ui::CModelMappingModifyForm)
        {
            ui->setupUi(this);
            connect(ui->le_Description, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::returnPressed);
            connect(ui->le_Name, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::returnPressed);
            connect(ui->selector_ModeSelector, &CModelModeSelector::changed, this, &CModelMappingModifyForm::changed);
            connect(ui->selector_SimulatorSelector, &CSimulatorSelector::changed, this, &CModelMappingModifyForm::changed);

            this->userChanged();
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

            if (ui->cb_Mode->isChecked())
            {
                vm.addValue(CAircraftModel::IndexModelMode, ui->selector_ModeSelector->getMode());
            }

            if (ui->cb_Simulator->isChecked())
            {
                vm.addValue(CAircraftModel::IndexSimulatorInfo, ui->selector_SimulatorSelector->getValue());
            }

            if (ui->cb_Mode->isChecked())
            {
                vm.addValue(CAircraftModel::IndexModelMode, ui->selector_ModeSelector->getMode());
            }
            return vm;
        }

        void CModelMappingModifyForm::setValue(const CAircraftModel &model)
        {
            ui->le_Description->setText(model.getDescription());
            ui->le_Name->setText(model.getName());
            ui->selector_SimulatorSelector->setValue(model.getSimulator());
            ui->selector_ModeSelector->setValue(model);
        }

        void CModelMappingModifyForm::setReadOnly(bool readOnly)
        {
            // void
            Q_UNUSED(readOnly);
            // this->forceStyleSheetUpdate();
        }

        void CModelMappingModifyForm::userChanged()
        {
            const CAuthenticatedUser user(this->getSwiftDbUser());
            if (user.hasAdminRole())
            {
                ui->selector_ModeSelector->setValue(CAircraftModel::Include);
                ui->selector_ModeSelector->setReadOnly(false);
            }
            else
            {
                ui->selector_ModeSelector->setReadOnly(true);
            }

            CFormDbUser::userChanged();
        }

        void CModelMappingModifyForm::returnPressed()
        {
            QCheckBox *cb = widgetToCheckbox(sender());
            if (!cb) { return; }
            cb->setChecked(true);
        }

        void CModelMappingModifyForm::changed()
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
            if (widget == ui->selector_ModeSelector) { return ui->cb_Mode; }
            if (widget == ui->selector_SimulatorSelector) { return ui->cb_Simulator; }
            return nullptr;
        }
    } // ns
} // ns

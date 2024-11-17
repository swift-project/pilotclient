// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/editors/modelmappingmodifyform.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QString>

#include "ui_modelmappingmodifyform.h"

#include "gui/components/modelmodeselector.h"
#include "gui/components/simulatorselector.h"
#include "gui/uppercasevalidator.h"
#include "misc/network/authenticateduser.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;
using namespace swift::gui::components;

namespace swift::gui::editors
{
    CModelMappingModifyForm::CModelMappingModifyForm(QWidget *parent)
        : CFormDbUser(parent), ui(new Ui::CModelMappingModifyForm)
    {
        ui->setupUi(this);
        ui->le_Parts->setPlaceholderText("Parts " + CAircraftModel::supportedParts());
        this->userChanged();

        CUpperCaseValidator *ucv =
            new CUpperCaseValidator(true, 0, CAircraftModel::supportedParts().size(), ui->le_Parts);
        ucv->setAllowedCharacters(CAircraftModel::supportedParts());
        ui->le_Parts->setValidator(ucv);

        // "auto checked" disabled
        // connect(ui->le_Description, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::returnPressed);
        // connect(ui->le_Name, &QLineEdit::returnPressed, this, &CModelMappingModifyForm::returnPressed);
        // connect(ui->selector_ModeSelector, &CModelModeSelector::changed, this, &CModelMappingModifyForm::changed);
        // connect(ui->selector_SimulatorSelector, &CSimulatorSelector::changed, this,
        // &CModelMappingModifyForm::changed);

        connect(ui->le_CG, &QLineEdit::editingFinished, this, &CModelMappingModifyForm::onCGEdited);
    }

    CModelMappingModifyForm::~CModelMappingModifyForm()
    {
        // void
    }

    CPropertyIndexVariantMap CModelMappingModifyForm::getValues() const
    {
        CPropertyIndexVariantMap vm;

        if (ui->cb_Name->isChecked()) { vm.addValue(CAircraftModel::IndexName, ui->le_Name->text().trimmed()); }

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

        if (ui->cb_Parts->isChecked())
        {
            vm.addValue(CAircraftModel::IndexSupportedParts, ui->le_Parts->text().toUpper());
        }

        if (ui->cb_CG->isChecked())
        {
            const QString cgv = ui->le_CG->text().trimmed();
            CLength cg = CLength::null();
            if (!cgv.isEmpty()) { cg.parseFromString(cgv, CPqString::SeparatorBestGuess); }
            vm.addValue(CAircraftModel::IndexCG, cg);
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
        else { ui->selector_ModeSelector->setReadOnly(true); }

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
        if (widget == ui->le_CG) { return ui->cb_CG; }
        if (widget == ui->le_Parts) { return ui->cb_Parts; }
        if (widget == ui->selector_SimulatorSelector) { return ui->cb_Simulator; }
        return nullptr;
    }

    void CModelMappingModifyForm::onCGEdited()
    {
        QString cgv = ui->le_CG->text().trimmed();
        if (isDigitsOnlyString(cgv)) { cgv += "ft"; }
        CLength cg = CLength::null();
        cg.parseFromString(cgv, CPqString::SeparatorBestGuess);
        ui->le_CG->setText(cg.isNull() ? "" : cg.toQString(true));
    }
} // namespace swift::gui::editors

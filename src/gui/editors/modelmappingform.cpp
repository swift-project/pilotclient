// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/editors/modelmappingform.h"

#include <QLineEdit>
#include <QPushButton>

#include "ui_modelmappingform.h"

#include "gui/components/modelmodeselector.h"
#include "gui/components/simulatorselector.h"
#include "gui/editors/validationindicator.h"
#include "gui/labelandicon.h"
#include "gui/uppercasevalidator.h"
#include "misc/icons.h"
#include "misc/network/authenticateduser.h"
#include "misc/stringutils.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;
using namespace swift::gui::components;

namespace swift::gui::editors
{
    CModelMappingForm::CModelMappingForm(QWidget *parent) : CFormDbUser(parent), ui(new Ui::CModelMappingForm)
    {
        ui->setupUi(this);
        ui->le_LastUpdated->setReadOnly(true);
        ui->le_Id->setReadOnly(true);
        ui->le_Parts->setPlaceholderText("Allowed: " + CAircraftModel::supportedParts());
        ui->lai_Id->set(CIcons::appMappings16(), "Id:");
        auto *uc = new CUpperCaseValidator(0, 5, ui->le_Parts);
        uc->setAllowedCharacters(CAircraftModel::supportedParts());
        ui->le_Parts->setValidator(uc);

        connect(ui->le_CG, &QLineEdit::editingFinished, this, &CModelMappingForm::onCgEditFinished);
        connect(ui->pb_Stash, &QPushButton::clicked, this, &CModelMappingForm::requestStash);

        // for setting mode (include/exclude)
        this->userChanged();
    }

    CModelMappingForm::~CModelMappingForm() = default;

    swift::misc::simulation::CAircraftModel CModelMappingForm::getValue() const
    {
        CAircraftModel model(m_originalModel);
        model.setSimulator(ui->selector_Simulator->getValue());
        model.setDescription(ui->le_Description->text());
        model.setModelString(ui->le_ModelKey->text());
        model.setName(ui->le_Name->text());
        model.setModelMode(ui->selector_ModelMode->getMode());
        model.setCG(this->getCGFromUI());
        model.setSupportedParts(ui->le_Parts->text().trimmed().toUpper());
        return model;
    }

    CStatusMessageList CModelMappingForm::validate(bool withNestedObjects) const
    {
        const CAircraftModel model(getValue());
        const CStatusMessageList msgs(model.validate(withNestedObjects));
        ui->val_Indicator->setState(msgs);
        return msgs;
    }

    void CModelMappingForm::setReadOnly(bool readOnly)
    {
        ui->le_Description->setReadOnly(readOnly);
        ui->le_ModelKey->setReadOnly(readOnly);
        ui->le_Name->setReadOnly(readOnly);
        this->forceStyleSheetUpdate();
    }

    void CModelMappingForm::setSelectOnly() { this->setReadOnly(true); }

    void CModelMappingForm::setValue(swift::misc::simulation::CAircraftModel &model)
    {
        ui->le_ModelKey->setText(model.getModelString());
        ui->le_LastUpdated->setText(model.getFormattedUtcTimestampYmdhms());
        ui->le_Id->setText(model.getDbKeyAsString());
        ui->le_Description->setText(model.getDescription());
        ui->le_Name->setText(model.getName());
        ui->le_Parts->setText(model.getSupportedParts());
        ui->le_FileName->setText(model.getFileName());
        ui->selector_ModelMode->setValue(model.getModelMode());
        ui->selector_Simulator->setValue(model.getSimulator());
        this->setCGtoUI(model.getCG());
        m_originalModel = model;
    }

    void CModelMappingForm::userChanged()
    {
        const CAuthenticatedUser user(this->getSwiftDbUser());
        if (user.hasAdminRole())
        {
            ui->selector_ModelMode->setValue(CAircraftModel::Include);
            ui->selector_ModelMode->setReadOnly(false);
        }
        else { ui->selector_ModelMode->setReadOnly(true); }

        CFormDbUser::userChanged();
    }

    CLength CModelMappingForm::getCGFromUI() const
    {
        if (ui->le_CG->text().isEmpty()) { return CLength::null(); }
        const QString v = ui->le_CG->text();

        // without unit we assume ft
        if (isDigitsOnlyString(v))
        {
            bool ok;
            const double cgv = v.toDouble(&ok);
            if (!ok) { return CLength::null(); }
            return { cgv, CLengthUnit::ft() };
        }

        CLength cg;
        cg.parseFromString(v, CPqString::SeparatorBestGuess);
        return cg;
    }

    void CModelMappingForm::setCGtoUI(const CLength &cg)
    {
        if (cg.isNull()) { ui->le_CG->clear(); }
        else { ui->le_CG->setText(cg.valueRoundedWithUnit(CLengthUnit::ft(), 1)); }
    }

    void CModelMappingForm::onCgEditFinished()
    {
        const CLength cg = this->getCGFromUI();
        this->setCGtoUI(cg);
    }
} // namespace swift::gui::editors

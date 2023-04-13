/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/modelmodeselector.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/editors/modelmappingform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/labelandicon.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/icons.h"
#include "blackmisc/network/authenticateduser.h"
#include "blackmisc/stringutils.h"
#include "ui_modelmappingform.h"

#include <QLineEdit>
#include <QPushButton>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Components;

namespace BlackGui::Editors
{
    CModelMappingForm::CModelMappingForm(QWidget *parent) : CFormDbUser(parent),
                                                            ui(new Ui::CModelMappingForm)
    {
        ui->setupUi(this);
        ui->le_LastUpdated->setReadOnly(true);
        ui->le_Id->setReadOnly(true);
        ui->le_Parts->setPlaceholderText("Allowed: " + CAircraftModel::supportedParts());
        ui->lai_Id->set(CIcons::appMappings16(), "Id:");
        CUpperCaseValidator *uc = new CUpperCaseValidator(0, 5, ui->le_Parts);
        uc->setAllowedCharacters(CAircraftModel::supportedParts());
        ui->le_Parts->setValidator(uc);

        connect(ui->le_CG, &QLineEdit::editingFinished, this, &CModelMappingForm::onCgEditFinished);
        connect(ui->pb_Stash, &QPushButton::clicked, this, &CModelMappingForm::requestStash);

        // for setting mode (include/exclude)
        this->userChanged();
    }

    CModelMappingForm::~CModelMappingForm()
    {}

    BlackMisc::Simulation::CAircraftModel CModelMappingForm::getValue() const
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

    void CModelMappingForm::setSelectOnly()
    {
        this->setReadOnly(true);
    }

    void CModelMappingForm::setValue(BlackMisc::Simulation::CAircraftModel &model)
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
        else
        {
            ui->selector_ModelMode->setReadOnly(true);
        }

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
            return CLength(cgv, CLengthUnit::ft());
        }

        CLength cg;
        cg.parseFromString(v, CPqString::SeparatorBestGuess);
        return cg;
    }

    void CModelMappingForm::setCGtoUI(const CLength &cg)
    {
        if (cg.isNull())
        {
            ui->le_CG->clear();
        }
        else
        {
            ui->le_CG->setText(cg.valueRoundedWithUnit(CLengthUnit::ft(), 1));
        }
    }

    void CModelMappingForm::onCgEditFinished()
    {
        const CLength cg = this->getCGFromUI();
        this->setCGtoUI(cg);
    }
} // ns

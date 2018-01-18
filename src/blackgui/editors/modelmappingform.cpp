/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/modelmodeselector.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/editors/modelmappingform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/labelandicon.h"
#include "blackmisc/icons.h"
#include "blackmisc/network/authenticateduser.h"
#include "ui_modelmappingform.h"

#include <QLineEdit>
#include <QPushButton>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Editors
    {
        CModelMappingForm::CModelMappingForm(QWidget *parent) :
            CFormDbUser(parent),
            ui(new Ui::CModelMappingForm)
        {
            ui->setupUi(this);
            ui->le_LastUpdated->setReadOnly(true);
            ui->le_Id->setReadOnly(true);
            ui->lai_Id->set(CIcons::appMappings16(), "Id:");
            connect(ui->pb_Stash, &QPushButton::clicked, this, &CModelMappingForm::requestStash);

            // for setting mode (include/exclude)
            this->userChanged();
        }

        CModelMappingForm::~CModelMappingForm()
        { }

        BlackMisc::Simulation::CAircraftModel CModelMappingForm::getValue() const
        {
            CAircraftModel model(m_originalModel);
            model.setSimulator(ui->selector_Simulator->getValue());
            model.setDescription(ui->le_Description->text());
            model.setModelString(ui->le_ModelKey->text());
            model.setName(ui->le_Name->text());
            model.setModelMode(ui->selector_ModelMode->getMode());
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
            ui->le_FileName->setText(model.getFileName());
            ui->selector_ModelMode->setValue(model.getModelMode());
            ui->selector_Simulator->setValue(model.getSimulator());
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
    } // ns
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbmodelmappingmodifydialog.h"
#include "blackgui/editors/modelmappingmodifyform.h"
#include "ui_dbmodelmappingmodifydialog.h"

#include <QWidget>

using namespace swift::misc;
using namespace swift::misc::simulation;

namespace BlackGui::Components
{
    CDbModelMappingModifyDialog::CDbModelMappingModifyDialog(QWidget *parent) : QDialog(parent),
                                                                                CDbMappingComponentAware(parent),
                                                                                ui(new Ui::CDbModelMappingModifyDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    }

    CDbModelMappingModifyDialog::~CDbModelMappingModifyDialog()
    {
        // void
    }

    CPropertyIndexVariantMap CDbModelMappingModifyDialog::getValues() const
    {
        return (ui->editor_ModelMappingModify->getValues());
    }

    void CDbModelMappingModifyDialog::setValue(const CAircraftModel &model)
    {
        ui->editor_ModelMappingModify->setValue(model);
    }
} // ns

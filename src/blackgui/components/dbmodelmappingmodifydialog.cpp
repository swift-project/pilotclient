/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbmodelmappingmodifydialog.h"
#include "blackgui/editors/modelmappingmodifyform.h"
#include "ui_dbmodelmappingmodifydialog.h"

#include <QWidget>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CDbModelMappingModifyDialog::CDbModelMappingModifyDialog(QWidget *parent) :
        QDialog(parent),
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

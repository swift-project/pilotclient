// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/editors/distributorform.h"

#include <QLineEdit>
#include <QMetaType>

#include "distributorform.h"
#include "ui_distributorform.h"

#include "gui/components/dbdistributorselectorcomponent.h"
#include "gui/dropsite.h"
#include "gui/editors/validationindicator.h"
#include "gui/labelandicon.h"
#include "misc/icons.h"
#include "misc/mixin/mixincompare.h"
#include "misc/simulation/distributorlist.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::gui::components;

namespace swift::gui::editors
{
    CDistributorForm::CDistributorForm(QWidget *parent) : CForm(parent), ui(new Ui::CDistributorForm)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->comp_DistributorSelector);
        ui->le_Updated->setReadOnly(true);
        ui->comp_DistributorSelector->withDistributorDescription(false);
        ui->lai_Id->set(CIcons::appDistributors16(), "Id:");

        // drag and drop
        connect(ui->drop_DropData, &CDropSite::droppedValueObject, this, &CDistributorForm::onDroppedCode);
        connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::changedDistributor, this,
                &CDistributorForm::setValue);
        // connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::returnPressed, this,
        // &CDistributorForm::onReturnPressed);
        ui->drop_DropData->setInfoText("<drop distributor>");
        ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CDistributor>(), qMetaTypeId<CDistributorList>() });
    }

    CDistributorForm::~CDistributorForm() {}

    bool CDistributorForm::setValue(const swift::misc::simulation::CDistributor &distributor)
    {
        if (m_currentDistributor == distributor) { return false; }
        m_currentDistributor = distributor;

        ui->comp_DistributorSelector->setDistributor(distributor);
        ui->le_Description->setText(distributor.getDescription());
        ui->le_Alias1->setText(distributor.getAlias1());
        ui->le_Alias2->setText(distributor.getAlias2());
        ui->le_Updated->setText(distributor.getFormattedUtcTimestampYmdhms());
        return true;
    }

    void CDistributorForm::jsonPasted(const QString &json) { Q_UNUSED(json); }

    CDistributor CDistributorForm::getValue() const
    {
        CDistributor distributor(ui->comp_DistributorSelector->getDistributor());
        distributor.setAlias1(ui->le_Alias1->text());
        distributor.setAlias2(ui->le_Alias2->text());
        distributor.setDescription(ui->le_Description->text());
        return distributor;
    }

    CStatusMessageList CDistributorForm::validate(bool nested) const
    {
        Q_UNUSED(nested);
        CDistributor distributor(getValue());
        CStatusMessageList msgs(distributor.validate());
        if (this->isReadOnly())
        {
            // in readonly I cannot change the data anyway, so skip warnings
            msgs.removeWarningsAndBelow();
        }
        ui->val_Indicator->setState(msgs);
        return msgs;
    }

    void CDistributorForm::allowDrop(bool allowDrop) { ui->drop_DropData->allowDrop(allowDrop); }

    bool CDistributorForm::isDropAllowed() const { return ui->drop_DropData->isDropAllowed(); }

    void CDistributorForm::setReadOnly(bool readOnly)
    {
        ui->le_Alias1->setReadOnly(readOnly);
        ui->le_Alias2->setReadOnly(readOnly);
        ui->le_Description->setReadOnly(readOnly);
        ui->comp_DistributorSelector->setReadOnly(readOnly);
        ui->drop_DropData->setVisible(!readOnly);
        this->forceStyleSheetUpdate();
    }

    void CDistributorForm::setSelectOnly()
    {
        this->setReadOnly(true);
        ui->comp_DistributorSelector->setReadOnly(false);
        ui->drop_DropData->setVisible(true);
    }

    void CDistributorForm::clear()
    {
        this->setValue(CDistributor());
        ui->comp_DistributorSelector->clear();
        ui->comp_DistributorSelector->setReadOnly(false);
    }

    void CDistributorForm::onDroppedCode(const swift::misc::CVariant &variantDropped)
    {
        CDistributor distributor;
        if (variantDropped.canConvert<CDistributor>()) { distributor = variantDropped.value<CDistributor>(); }
        else if (variantDropped.canConvert<CDistributorList>())
        {
            const CDistributorList icaoList(variantDropped.value<CDistributorList>());
            if (icaoList.isEmpty()) { return; }
            distributor = icaoList.front();
        }
        else { return; }
        this->setValue(distributor);
    }

    bool CDistributorForm::hasAnyUiDetailsValues() const
    {
        return !(ui->le_Alias1->text().isEmpty() && ui->le_Alias2->text().isEmpty() &&
                 ui->le_Description->text().isEmpty() && ui->le_Updated->text().isEmpty());
    }
} // namespace swift::gui::editors

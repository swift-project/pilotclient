/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/dbdistributorselectorcomponent.h"
#include "blackgui/editors/distributorform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/labelandicon.h"
#include "blackgui/dropsite.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"
#include "blackmisc/simulation/distributorlist.h"
#include "distributorform.h"
#include "ui_distributorform.h"

#include <QLineEdit>
#include <QMetaType>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CDistributorForm::CDistributorForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CDistributorForm)
        {
            ui->setupUi(this);
            this->setFocusProxy(ui->distributor_Selector);
            ui->le_Updated->setReadOnly(true);
            ui->distributor_Selector->withDistributorDescription(false);
            ui->lai_Id->set(CIcons::appDistributors16(), "Id:");

            // drag and drop
            connect(ui->drop_DropData, &CDropSite::droppedValueObject, this, &CDistributorForm::ps_droppedCode);
            connect(ui->distributor_Selector, &CDbDistributorSelectorComponent::changedDistributor, this, &CDistributorForm::setValue);
            ui->drop_DropData->setInfoText("<drop distributor>");
            ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CDistributor>(), qMetaTypeId<CDistributorList>()});
        }

        CDistributorForm::~CDistributorForm()
        { }

        bool CDistributorForm::setValue(const BlackMisc::Simulation::CDistributor &distributor)
        {
            const CDistributor currentDistributor(this->getValue());
            if (currentDistributor == distributor) { return false; }

            ui->distributor_Selector->setDistributor(distributor);
            ui->le_Description->setText(distributor.getDescription());
            ui->le_Alias1->setText(distributor.getAlias1());
            ui->le_Alias2->setText(distributor.getAlias2());
            ui->le_Updated->setText(distributor.getFormattedUtcTimestampYmdhms());
            return true;
        }

        void CDistributorForm::jsonPasted(const QString &json)
        {
            Q_UNUSED(json);
        }

        CDistributor CDistributorForm::getValue() const
        {
            CDistributor distributor(ui->distributor_Selector->getDistributor());
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

        void CDistributorForm::allowDrop(bool allowDrop)
        {
            ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CDistributorForm::isDropAllowed() const
        {
            return ui->drop_DropData->isDropAllowed();
        }

        void CDistributorForm::setReadOnly(bool readOnly)
        {
            ui->le_Alias1->setReadOnly(readOnly);
            ui->le_Alias2->setReadOnly(readOnly);
            ui->le_Description->setReadOnly(readOnly);
            ui->distributor_Selector->setReadOnly(readOnly);
            ui->drop_DropData->setVisible(!readOnly);
            this->forceStyleSheetUpdate();
        }

        void CDistributorForm::setSelectOnly()
        {
            this->setReadOnly(true);
            ui->distributor_Selector->setReadOnly(false);
            ui->drop_DropData->setVisible(true);
        }

        void CDistributorForm::clear()
        {
            setValue(CDistributor());
            ui->distributor_Selector->setReadOnly(false);
        }

        void CDistributorForm::ps_droppedCode(const BlackMisc::CVariant &variantDropped)
        {
            CDistributor distributor;
            if (variantDropped.canConvert<CDistributor>())
            {
                distributor = variantDropped.value<CDistributor>();
            }
            else if (variantDropped.canConvert<CDistributorList>())
            {
                const CDistributorList icaoList(variantDropped.value<CDistributorList>());
                if (icaoList.isEmpty()) { return; }
                distributor = icaoList.front();
            }
            else
            {
                return;
            }
            this->setValue(distributor);
        }
    } // ns
} // ns

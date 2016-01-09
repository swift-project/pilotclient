/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/country.h"
#include "distributorform.h"
#include "ui_distributorform.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CDistributorForm::CDistributorForm(QWidget *parent) :
            CForm(parent),
            BlackMisc::Network::CWebDataServicesAware(nullptr),
            ui(new Ui::CDistributorForm)
        {
            ui->setupUi(this);
            this->ui->le_Updated->setReadOnly(true);
            this->ui->distributor_Selector->withDistributorDescription(false);
            ui->lai_Id->set(CIcons::appDistributors16(), "Id:");

            // drag and drop
            connect(this->ui->drop_DropData, &CDropSite::droppedValueObject, this, &CDistributorForm::ps_droppedCode);
            connect(this->ui->distributor_Selector, &CDbDistributorSelectorComponent::changedDistributor, this, &CDistributorForm::setValue);
            this->ui->drop_DropData->setInfoText("<drop distributor>");
            this->ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CDistributor>(), qMetaTypeId<CDistributorList>()});
        }

        CDistributorForm::~CDistributorForm()
        { }

        void CDistributorForm::setValue(const BlackMisc::Simulation::CDistributor &distributor)
        {
            this->ui->distributor_Selector->setDistributor(distributor);
            this->ui->le_Description->setText(distributor.getDescription());
            this->ui->le_Alias1->setText(distributor.getAlias1());
            this->ui->le_Alias2->setText(distributor.getAlias2());
            this->ui->le_Updated->setText(distributor.getFormattedUtcTimestampYmdhms());
        }

        CDistributor CDistributorForm::getValue() const
        {
            CDistributor distributor(ui->distributor_Selector->getDistributor());
            distributor.setAlias1(this->ui->le_Alias1->text());
            distributor.setAlias2(this->ui->le_Alias2->text());
            distributor.setDescription(this->ui->le_Description->text());
            return distributor;
        }

        CStatusMessageList CDistributorForm::validate() const
        {
            CDistributor distributor(getValue());
            CStatusMessageList msgs;
            if (!distributor.getDbKey().isEmpty())
            {
                // optional distributor
                msgs = distributor.validate();
            }
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }
            this->ui->val_Indicator->setState(msgs);
            return msgs;
        }

        void CDistributorForm::allowDrop(bool allowDrop)
        {
            this->ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CDistributorForm::isDropAllowed() const
        {
            return this->ui->drop_DropData->isDropAllowed();
        }

        void CDistributorForm::setReadOnly(bool readOnly)
        {
            this->ui->le_Alias1->setReadOnly(readOnly);
            this->ui->le_Alias2->setReadOnly(readOnly);
            this->ui->le_Description->setReadOnly(readOnly);
            this->ui->distributor_Selector->setReadOnly(readOnly);
        }

        void CDistributorForm::clear()
        {
            setValue(CDistributor());
            this->ui->distributor_Selector->setReadOnly(false);
        }

        void CDistributorForm::setMappingMode(bool mappingMode)
        {
            this->setReadOnly(mappingMode);
        }

        void CDistributorForm::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            this->ui->distributor_Selector->setProvider(webDataReaderProvider);
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
                CDistributorList icaoList(variantDropped.value<CDistributorList>());
                if (icaoList.isEmpty()) { return;  }
                distributor =  icaoList.front();
            }
            else
            {
                return;
            }
            this->setValue(distributor);
        }

    } // ns
} // ns

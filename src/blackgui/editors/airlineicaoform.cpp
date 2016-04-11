/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/guiutility.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/country.h"
#include "airlineicaoform.h"
#include "ui_airlineicaoform.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CAirlineIcaoForm::CAirlineIcaoForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CAirlineIcaoForm)
        {
            ui->setupUi(this);
            this->ui->le_Updated->setReadOnly(true);
            this->ui->le_Id->setReadOnly(true);
            this->ui->lai_Id->set(CIcons::appAirlineIcao16(), "Id:");

            this->ui->selector_AirlineDesignator->withIcaoDescription(false);
            connect(this->ui->selector_AirlineName, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CAirlineIcaoForm::setValue);
            connect(this->ui->selector_AirlineDesignator, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CAirlineIcaoForm::setValue);

            // drag and drop
            connect(this->ui->drop_DropData, &CDropSite::droppedValueObject, this, &CAirlineIcaoForm::ps_droppedCode);
            this->ui->drop_DropData->setInfoText("<drop airline ICAO code>");
            this->ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CAirlineIcaoCode>(), qMetaTypeId<CAirlineIcaoCodeList>()});
        }

        CAirlineIcaoForm::~CAirlineIcaoForm()
        { }

        void CAirlineIcaoForm::setValue(const BlackMisc::Aviation::CAirlineIcaoCode &icao)
        {
            if (this->m_originalCode == icao) { return; }
            this->m_originalCode = icao;

            this->ui->selector_AirlineDesignator->setAirlineIcao(icao);
            this->ui->selector_AirlineName->setAirlineIcao(icao);
            this->ui->le_Id->setText(icao.getDbKeyAsString());
            this->ui->le_TelephonyDesignator->setText(icao.getTelephonyDesignator());
            this->ui->le_Updated->setText(icao.getFormattedUtcTimestampYmdhms());
            this->ui->cb_Va->setChecked(icao.isVirtualAirline());
            this->ui->cb_Military->setChecked(icao.isMilitary());
            this->ui->country_Selector->setCountry(icao.getCountry());
            this->ui->lbl_AirlineIcon->setPixmap(icao.toPixmap());

            if (this->m_originalCode.hasCompleteData())
            {
                emit airlineChanged(this->m_originalCode);
            }
        }

        CAirlineIcaoCode CAirlineIcaoForm::getValue() const
        {
            CAirlineIcaoCode code(m_originalCode);
            code.setVirtualAirline(this->ui->cb_Va->isChecked());
            code.setMilitary(this->ui->cb_Military->isChecked());
            code.setCountry(this->ui->country_Selector->getCountry());
            code.setName(this->ui->selector_AirlineName->getAirlineIcao().getName());
            code.setTelephonyDesignator(this->ui->le_TelephonyDesignator->text());
            return code;
        }

        CStatusMessageList CAirlineIcaoForm::validate() const
        {
            CAirlineIcaoCode code(getValue());
            CStatusMessageList msgs(code.validate());
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }

            this->ui->val_Indicator->setState(msgs);
            return msgs;
        }

        void CAirlineIcaoForm::allowDrop(bool allowDrop)
        {
            this->ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CAirlineIcaoForm::isDropAllowed() const
        {
            return this->ui->drop_DropData->isDropAllowed();
        }

        void CAirlineIcaoForm::setReadOnly(bool readOnly)
        {
            this->ui->selector_AirlineDesignator->setReadOnly(readOnly);
            this->ui->selector_AirlineName->setReadOnly(readOnly);
            this->ui->le_TelephonyDesignator->setReadOnly(readOnly);
            this->ui->country_Selector->setReadOnly(readOnly);

            CGuiUtility::checkBoxReadOnly(this->ui->cb_Va, readOnly);
            CGuiUtility::checkBoxReadOnly(this->ui->cb_Military, readOnly);
        }

        void CAirlineIcaoForm::setSelectOnly()
        {
            this->setReadOnly(true);
            this->ui->selector_AirlineDesignator->setReadOnly(false);
            this->ui->selector_AirlineName->setReadOnly(false);
        }

        void CAirlineIcaoForm::clear()
        {
            setValue(CAirlineIcaoCode());
        }

        void CAirlineIcaoForm::ps_droppedCode(const BlackMisc::CVariant &variantDropped)
        {
            CAirlineIcaoCode icao;
            if (variantDropped.canConvert<CAirlineIcaoCode>())
            {
                icao = variantDropped.value<CAirlineIcaoCode>();
            }
            else if (variantDropped.canConvert<CAirlineIcaoCodeList>())
            {
                CAirlineIcaoCodeList icaoList(variantDropped.value<CAirlineIcaoCodeList>());
                if (icaoList.isEmpty()) { return;  }
                icao =  icaoList.front();
            }
            else
            {
                return;
            }
            this->setValue(icao);
        }
    } // ns
} // ns

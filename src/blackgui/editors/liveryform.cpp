/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/colorselector.h"
#include "blackgui/components/dbliveryselectorcomponent.h"
#include "blackgui/dropsite.h"
#include "blackgui/editors/airlineicaoform.h"
#include "blackgui/editors/liveryform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/labelandicon.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"
#include "liveryform.h"
#include "ui_liveryform.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMetaType>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CLiveryForm::CLiveryForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CLiveryForm)
        {
            ui->setupUi(this);
            this->ui->le_Id->setReadOnly(true);
            this->ui->le_Updated->setReadOnly(true);
            this->ui->lai_Id->set(CIcons::appLiveries16(), "Id:");
            this->ui->comp_LiverySelector->withLiveryDescription(false);

            // selector
            connect(this->ui->comp_LiverySelector, &CDbLiverySelectorComponent::changedLivery, this, &CLiveryForm::setValue);

            // drag and drop
            connect(this->ui->drop_DropData, &CDropSite::droppedValueObject, this, &CLiveryForm::ps_droppedLivery);
            this->ui->drop_DropData->setInfoText("<drop livery>");
            this->ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CLivery>(), qMetaTypeId<CLiveryList>()});

            // embedded form
            connect(this->ui->editor_AirlineIcao, &CAirlineIcaoForm::airlineChanged, this, &CLiveryForm::ps_airlineChanged);
        }

        CLiveryForm::~CLiveryForm() { }

        CLivery CLiveryForm::getValue() const
        {
            CLivery livery(this->ui->comp_LiverySelector->getLivery());
            if (livery.hasCompleteData() && livery.hasValidDbKey())
            {
                // already complete data from selector
                return livery;
            }

            CAirlineIcaoCode airline(this->ui->editor_AirlineIcao->getValue());
            livery.setAirlineIcaoCode(airline);
            livery.setDescription(this->ui->le_Description->text());
            livery.setMilitary(this->ui->cb_Military->isChecked());
            return livery;
        }

        CAirlineIcaoCode CLiveryForm::getValueAirlineIcao() const
        {
            return this->ui->editor_AirlineIcao->getValue();
        }

        bool CLiveryForm::setValue(const CLivery &livery)
        {
            if (this->m_originalLivery == livery) { return false; }

            this->m_originalLivery = livery;
            this->ui->comp_LiverySelector->setLivery(livery);
            this->ui->le_Id->setText(livery.getDbKeyAsString());
            this->ui->le_Description->setText(livery.getDescription());
            this->ui->le_Updated->setText(livery.getFormattedUtcTimestampYmdhms());
            this->ui->color_Fuselage->setColor(livery.getColorFuselage());
            this->ui->color_Tail->setColor(livery.getColorTail());
            this->ui->cb_Military->setChecked(livery.isMilitary());

            if (livery.isColorLivery())
            {
                this->ui->editor_AirlineIcao->clear();
            }
            else
            {
                this->ui->editor_AirlineIcao->setValue(livery.getAirlineIcaoCode());
            }
            return true;
        }

        CStatusMessageList CLiveryForm::validate(bool withNestedForms) const
        {
            CLivery livery(getValue());
            CStatusMessageList msgs(livery.validate());
            if (withNestedForms)
            {
                if (!livery.isColorLivery())
                {
                    msgs.push_back(this->ui->editor_AirlineIcao->validate());
                }
            }
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }
            this->ui->val_Indicator->setState(msgs);
            return msgs;
        }

        CStatusMessageList CLiveryForm::validateAirlineIcao() const
        {
            return this->ui->editor_AirlineIcao->validate();
        }

        void CLiveryForm::allowDrop(bool allowDrop)
        {
            this->ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CLiveryForm::isDropAllowed() const
        {
            return this->ui->drop_DropData->isDropAllowed();
        }

        void CLiveryForm::setReadOnly(bool readOnly)
        {
            this->m_readOnly = readOnly;
            this->ui->comp_LiverySelector->setReadOnly(readOnly);
            this->ui->le_Description->setReadOnly(readOnly);
            this->ui->color_Fuselage->setReadOnly(readOnly);
            this->ui->color_Tail->setReadOnly(readOnly);
            this->ui->editor_AirlineIcao->setReadOnly(readOnly);
            CGuiUtility::checkBoxReadOnly(this->ui->cb_Military, readOnly);
        }

        void CLiveryForm::setSelectOnly()
        {
            this->setReadOnly(true);
            this->ui->comp_LiverySelector->setReadOnly(false);
            this->ui->editor_AirlineIcao->setSelectOnly();
        }

        void CLiveryForm::clear()
        {
            this->setValue(CLivery());
        }

        void CLiveryForm::ps_droppedLivery(const BlackMisc::CVariant &variantDropped)
        {
            CLivery livery;
            if (variantDropped.canConvert<CLivery>())
            {
                livery = variantDropped.value<CLivery>();
            }
            else if (variantDropped.canConvert<CLiveryList>())
            {
                CLiveryList liveryList(variantDropped.value<CLiveryList>());
                if (liveryList.isEmpty()) { return;  }
                livery =  liveryList.front();
            }
            else { return; }
            this->setValue(livery);
        }

        void CLiveryForm::ps_airlineChanged(const CAirlineIcaoCode &code)
        {
            if (!code.hasCompleteData()) { return; }
            if (!code.hasValidDbKey()) { return; }

            const CLivery stdLivery(sGui->getWebDataServices()->getLiveries().findStdLiveryByAirlineIcaoDesignator(code));
            if (stdLivery.hasValidDbKey())
            {
                this->setValue(stdLivery);
            }
        }
    } // ns
} // ns

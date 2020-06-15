/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "liveryform.h"
#include "ui_liveryform.h"
#include "blackgui/components/colorselector.h"
#include "blackgui/components/dbliveryselectorcomponent.h"
#include "blackgui/dropsite.h"
#include "blackgui/editors/airlineicaoform.h"
#include "blackgui/editors/liveryform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/labelandicon.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"

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
            ui->le_Updated->setReadOnly(true);
            ui->le_Id->setValidator(new QIntValidator(0, 999999, ui->le_Id));
            ui->lai_Id->set(CIcons::appLiveries16(), "Id:");
            ui->comp_LiverySelector->withLiveryDescription(false);

            // Id
            connect(ui->le_Id, &QLineEdit::returnPressed, this, &CLiveryForm::onIdEntered);

            // selector
            connect(ui->comp_LiverySelector, &CDbLiverySelectorComponent::changedLivery, this, &CLiveryForm::setValue, Qt::QueuedConnection);

            // drag and drop, paste
            connect(ui->tb_Paste, &QToolButton::clicked, this, &CLiveryForm::pasted);
            connect(ui->drop_DropData, &CDropSite::droppedValueObject, this, &CLiveryForm::onDroppedLivery);
            ui->drop_DropData->setInfoText("<drop livery>");
            ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CLivery>(), qMetaTypeId<CLiveryList>()});

            // embedded form
            connect(ui->editor_AirlineIcao, &CAirlineIcaoForm::airlineChangedDigest, this, &CLiveryForm::onAirlineChanged, Qt::QueuedConnection);

            // Set as temp.livery or search color
            connect(ui->pb_TempLivery, &QPushButton::pressed, this, &CLiveryForm::setTemporaryLivery);
            connect(ui->pb_SearchColor, &QPushButton::pressed, this, &CLiveryForm::searchForColor);
        }

        CLiveryForm::~CLiveryForm() { }

        CLivery CLiveryForm::getValue() const
        {
            CLivery livery;
            const QString id = ui->le_Id->text();
            if (!id.isEmpty() && sGui && !sGui->isShuttingDown() && sGui->hasWebDataServices())
            {
                bool ok;
                const int dbKey = id.toInt(&ok);
                if (ok)
                {
                    livery = sGui->getWebDataServices()->getLiveryForDbKey(dbKey);
                }
            }

            // fallback
            if (!livery.hasValidDbKey())
            {
                livery = ui->comp_LiverySelector->getLivery();
            }

            if (livery.hasCompleteData() && livery.hasValidDbKey())
            {
                // already complete data from selector
                return livery;
            }

            CAirlineIcaoCode airline(ui->editor_AirlineIcao->getValue());
            livery.setAirlineIcaoCode(airline);
            livery.setDescription(ui->le_Description->text());
            livery.setMilitary(ui->cb_Military->isChecked());
            return livery;
        }

        CAirlineIcaoCode CLiveryForm::getValueAirlineIcao() const
        {
            return ui->editor_AirlineIcao->getValue();
        }

        bool CLiveryForm::setValue(const CLivery &livery)
        {
            if (m_originalLivery == livery) { return false; }

            m_originalLivery = livery;
            ui->comp_LiverySelector->setLivery(livery);
            ui->le_Id->setText(livery.getDbKeyAsString());
            ui->le_Description->setText(livery.getDescription());
            ui->le_Updated->setText(livery.getFormattedUtcTimestampYmdhms());
            ui->color_Fuselage->setColor(livery.getColorFuselage());
            ui->color_Tail->setColor(livery.getColorTail());
            ui->cb_Military->setChecked(livery.isMilitary());

            if (livery.isColorLivery())
            {
                ui->editor_AirlineIcao->clear();
            }
            else
            {
                ui->editor_AirlineIcao->setValue(livery.getAirlineIcaoCode());
            }
            return true;
        }

        void CLiveryForm::jsonPasted(const QString &json)
        {
            if (json.isEmpty()) { return; } // avoid unnecessary conversions
            try
            {
                CVariant jsonVariant;
                jsonVariant.convertFromJson(Json::jsonObjectFromString(json));
                if (!jsonVariant.canConvert<CLiveryList>()) { return; }
                const CLiveryList liveries = jsonVariant.value<CLiveryList>();
                if (!liveries.isEmpty())
                {
                    this->setValue(liveries.front());
                }
            }
            catch (const CJsonException &ex)
            {
                Q_UNUSED(ex);
            }
        }

        CStatusMessageList CLiveryForm::validate(bool withNestedForms) const
        {
            CLivery livery(getValue());
            CStatusMessageList msgs(livery.validate());
            if (withNestedForms)
            {
                if (!livery.isColorLivery())
                {
                    msgs.push_back(ui->editor_AirlineIcao->validate());
                }
            }
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }
            ui->val_Indicator->setState(msgs);
            return msgs;
        }

        CStatusMessageList CLiveryForm::validateAirlineIcao() const
        {
            return ui->editor_AirlineIcao->validate();
        }

        void CLiveryForm::allowDrop(bool allowDrop)
        {
            ui->drop_DropData->allowDrop(allowDrop);
            ui->comp_LiverySelector->allowDrop(allowDrop);
            ui->editor_AirlineIcao->allowDrop(allowDrop);
        }

        bool CLiveryForm::isDropAllowed() const
        {
            return ui->drop_DropData->isDropAllowed();
        }

        void CLiveryForm::setReadOnly(bool readOnly)
        {
            m_readOnly = readOnly;
            ui->le_Id->setReadOnly(readOnly);
            ui->comp_LiverySelector->setReadOnly(readOnly);
            ui->le_Description->setReadOnly(readOnly);
            ui->color_Fuselage->setReadOnly(readOnly);
            ui->color_Tail->setReadOnly(readOnly);
            ui->editor_AirlineIcao->setReadOnly(readOnly);
            ui->pb_SearchColor->setVisible(!readOnly);
            ui->pb_TempLivery->setVisible(!readOnly);
            ui->drop_DropData->setVisible(!readOnly);
            ui->tb_Paste->setVisible(!readOnly);
            CGuiUtility::checkBoxReadOnly(ui->cb_Military, readOnly);
            this->forceStyleSheetUpdate();
        }

        void CLiveryForm::setSelectOnly()
        {
            this->setReadOnly(true);
            ui->comp_LiverySelector->setReadOnly(false);
            ui->le_Id->setReadOnly(false);
            ui->editor_AirlineIcao->setSelectOnly();
            ui->drop_DropData->setVisible(true);
            ui->pb_SearchColor->setVisible(true);
            ui->pb_TempLivery->setVisible(true);
            ui->tb_Paste->setVisible(true);
        }

        void CLiveryForm::clear()
        {
            this->setValue(CLivery());
        }

        void CLiveryForm::resetValue()
        {
            this->setValue(m_originalLivery);
        }

        void CLiveryForm::onDroppedLivery(const BlackMisc::CVariant &variantDropped)
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

        void CLiveryForm::onAirlineChanged(const CAirlineIcaoCode &code)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
            if (!code.hasCompleteData()) { return; }
            if (!code.hasValidDbKey())   { return; }

            // only replace with STD livery if airline does not match
            const CLivery currentLivery = this->getValue();
            if (currentLivery.hasValidDbKey() && currentLivery.getAirlineIcaoCode() == code) { return; }

            const CLivery stdLivery(sGui->getWebDataServices()->getLiveries().findStdLiveryByAirlineIcaoVDesignator(code));
            if (stdLivery.hasValidDbKey())
            {
                this->setValue(stdLivery);
            }
        }

        void CLiveryForm::setTemporaryLivery()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
            const CLivery l = sGui->getWebDataServices()->getTempLiveryOrDefault();
            if (l.isLoadedFromDb())
            {
                this->setValue(l);
            }
        }

        void CLiveryForm::searchForColor()
        {
            if (!m_colorSearch)
            {
                m_colorSearch = new CDbLiveryColorSearchDialog(this);
                m_colorSearch->setModal(true);
            }
            const QDialog::DialogCode c = static_cast<QDialog::DialogCode>(m_colorSearch->exec());
            if (c == QDialog::Rejected) { return; }
            const CLivery found = m_colorSearch->getLivery();
            if (found.isLoadedFromDb())
            {
                this->setValue(found);
            }
        }

        void CLiveryForm::onIdEntered()
        {
            if (!sGui || !sGui->hasWebDataServices())
            {
                ui->le_Id->undo();
                return;
            }

            const int id = ui->le_Id->text().toInt();
            const CLivery livery = sGui->getWebDataServices()->getLiveryForDbKey(id);
            if (!livery.isLoadedFromDb())
            {
                ui->le_Id->undo();
                return;
            }
            this->setValue(livery);
        }
    } // ns
} // ns

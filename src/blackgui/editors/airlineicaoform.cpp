/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/dbairlineicaoselectorcomponent.h"
#include "blackgui/components/dbairlinenameselectorcomponent.h"
#include "blackgui/components/dbcountryselectorcomponent.h"
#include "blackgui/dropsite.h"
#include "blackgui/editors/airlineicaoform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/guiutility.h"
#include "blackgui/labelandicon.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"
#include "ui_airlineicaoform.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>

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
            ui->le_Updated->setReadOnly(true);
            ui->le_Id->setValidator(new QIntValidator(0, 999999, ui->le_Id));
            ui->lai_Id->set(CIcons::appAirlineIcao16(), "Id:");

            ui->selector_AirlineDesignator->displayWithIcaoDescription(false);
            connect(ui->selector_AirlineName, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CAirlineIcaoForm::setValue, Qt::QueuedConnection);
            connect(ui->selector_AirlineDesignator, &CDbAirlineIcaoSelectorComponent::changedAirlineIcao, this, &CAirlineIcaoForm::setValue, Qt::QueuedConnection);

            // Id
            connect(ui->le_Id, &QLineEdit::returnPressed, this, &CAirlineIcaoForm::onIdEntered);

            // drag and drop, paste
            connect(ui->tb_Paste, &QToolButton::clicked, this, &CAirlineIcaoForm::pasted);
            connect(ui->drop_DropData, &CDropSite::droppedValueObject, this, &CAirlineIcaoForm::onDroppedCode);
            ui->drop_DropData->setInfoText("<drop airline ICAO code>");
            ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CAirlineIcaoCode>(), qMetaTypeId<CAirlineIcaoCodeList>()});
        }

        CAirlineIcaoForm::~CAirlineIcaoForm()
        { }

        void CAirlineIcaoForm::setValue(const CAirlineIcaoCode &icao)
        {
            if (m_currentCode == icao) { return; }
            m_currentCode = icao;

            ui->selector_AirlineDesignator->setAirlineIcao(icao);
            ui->selector_AirlineName->setAirlineIcao(icao);
            ui->le_Id->setText(icao.getDbKeyAsString());
            ui->le_TelephonyDesignator->setText(icao.getTelephonyDesignator());
            ui->le_Updated->setText(icao.getFormattedUtcTimestampYmdhms());
            ui->cb_Va->setChecked(icao.isVirtualAirline());
            ui->cb_Military->setChecked(icao.isMilitary());
            ui->country_Selector->setCountry(icao.getCountry());

            const QPixmap pm = icao.toPixmap();
            if (pm.width() < 125)
            {
                ui->lbl_AirlineIcon->setPixmap(pm);
            }
            else
            {
                ui->lbl_AirlineIcon->setTextFormat(Qt::RichText);
                ui->lbl_AirlineIcon->setText(CGuiUtility::asSimpleHtmlImageHeight(icao.toIcon(), 25));
            }

            // sometimes artefacts when icon is displayed
            this->repaint();

            if (m_currentCode.hasCompleteData())
            {
                emit this->airlineChanged(m_currentCode);
            }
        }

        CAirlineIcaoCode CAirlineIcaoForm::getValue() const
        {
            CAirlineIcaoCode code;
            const QString id = ui->le_Id->text();
            if (sGui && !sGui->isShuttingDown() && sGui->hasWebDataServices())
            {
                bool ok;
                const int dbKey = id.toInt(&ok);
                if (ok)
                {
                    code = sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(dbKey);
                }
            }

            if (code.hasValidDbKey()) { return code; }
            code = m_currentCode;
            code.setVirtualAirline(ui->cb_Va->isChecked());
            code.setMilitary(ui->cb_Military->isChecked());
            code.setCountry(ui->country_Selector->getCountry());
            code.setName(ui->selector_AirlineName->getAirlineIcao().getName());
            code.setTelephonyDesignator(ui->le_TelephonyDesignator->text());
            return code;
        }

        CStatusMessageList CAirlineIcaoForm::validate(bool nested) const
        {
            Q_UNUSED(nested);
            CAirlineIcaoCode code(getValue());
            CStatusMessageList msgs(code.validate());
            if (this->isReadOnly())
            {
                // in readonly I cannot change the data anyway, so skip warnings
                msgs.removeWarningsAndBelow();
            }

            ui->val_Indicator->setState(msgs);
            return msgs;
        }

        void CAirlineIcaoForm::allowDrop(bool allowDrop)
        {
            ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CAirlineIcaoForm::isDropAllowed() const
        {
            return ui->drop_DropData->isDropAllowed();
        }

        void CAirlineIcaoForm::setReadOnly(bool readOnly)
        {
            ui->le_Id->setReadOnly(readOnly);
            ui->selector_AirlineDesignator->setReadOnly(readOnly);
            ui->selector_AirlineName->setReadOnly(readOnly);
            ui->le_TelephonyDesignator->setReadOnly(readOnly);
            ui->country_Selector->setReadOnly(readOnly);
            ui->drop_DropData->setVisible(!readOnly);
            ui->tb_Paste->setVisible(!readOnly);

            CGuiUtility::checkBoxReadOnly(ui->cb_Va, readOnly);
            CGuiUtility::checkBoxReadOnly(ui->cb_Military, readOnly);

            this->forceStyleSheetUpdate();
        }

        void CAirlineIcaoForm::setSelectOnly()
        {
            this->setReadOnly(true);
            ui->le_Id->setReadOnly(false);
            ui->selector_AirlineDesignator->setReadOnly(false);
            ui->selector_AirlineName->setReadOnly(false);
            ui->drop_DropData->setVisible(true);
            ui->tb_Paste->setVisible(true);
        }

        void CAirlineIcaoForm::clear()
        {
            this->setValue(CAirlineIcaoCode());
        }

        void CAirlineIcaoForm::resetValue()
        {
            this->setValue(m_currentCode);
        }

        void CAirlineIcaoForm::jsonPasted(const QString &json)
        {
            if (json.isEmpty()) { return; } // avoid unnecessary conversions
            try
            {
                CVariant jsonVariant;
                jsonVariant.convertFromJson(Json::jsonObjectFromString(json));
                if (!jsonVariant.canConvert<CAirlineIcaoCodeList>()) { return; }
                const CAirlineIcaoCodeList icaos = jsonVariant.value<CAirlineIcaoCodeList>();
                if (!icaos.isEmpty())
                {
                    this->setValue(icaos.front());
                }
            }
            catch (const CJsonException &ex)
            {
                Q_UNUSED(ex);
            }
        }

        void CAirlineIcaoForm::onDroppedCode(const BlackMisc::CVariant &variantDropped)
        {
            CAirlineIcaoCode icao;
            if (variantDropped.canConvert<CAirlineIcaoCode>())
            {
                icao = variantDropped.value<CAirlineIcaoCode>();
            }
            else if (variantDropped.canConvert<CAirlineIcaoCodeList>())
            {
                const CAirlineIcaoCodeList icaoList(variantDropped.value<CAirlineIcaoCodeList>());
                if (icaoList.isEmpty()) { return;  }
                icao =  icaoList.front();
            }
            else
            {
                return;
            }
            this->setValue(icao);
        }

        void CAirlineIcaoForm::onIdEntered()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }

            bool ok;
            const int id = ui->le_Id->text().toInt(&ok);
            const CAirlineIcaoCode icao = sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(id);
            if (ok && !icao.isLoadedFromDb())
            {
                ui->le_Id->undo();
                return;
            }
            this->setValue(icao);
        }

        void CAirlineIcaoForm::emitAirlineChangedDigest()
        {
            emit this->airlineChangedDigest(m_currentCode);
        }
    } // ns
} // ns

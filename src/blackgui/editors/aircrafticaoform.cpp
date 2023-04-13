/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "ui_aircrafticaoform.h"
#include "aircrafticaoform.h"

#include "blackgui/components/aircraftcombinedtypeselector.h"
#include "blackgui/components/dbaircrafticaoselectorcomponent.h"
#include "blackgui/dropsite.h"
#include "blackgui/editors/aircrafticaoform.h"
#include "blackgui/editors/validationindicator.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/labelandicon.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/icons.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMetaType>
#include <QString>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;
using namespace BlackGui::Components;

namespace BlackGui::Editors
{
    CAircraftIcaoForm::CAircraftIcaoForm(QWidget *parent) : CForm(parent),
                                                            ui(new Ui::CAircraftIcaoForm)
    {
        ui->setupUi(this);
        this->setFocusProxy(ui->le_Id);
        ui->lai_Id->set(CIcons::appAircraftIcao16(), "Id:");
        ui->le_Updated->setReadOnly(true);
        ui->le_Id->setValidator(new QIntValidator(0, 999999, ui->le_Id));
        ui->aircraft_Selector->displayWithIcaoDescription(false);
        connect(ui->aircraft_Selector, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &CAircraftIcaoForm::setValue);

        // Id
        connect(ui->le_Id, &QLineEdit::returnPressed, this, &CAircraftIcaoForm::idEntered);

        // drag and drop, pasted
        connect(ui->drop_DropData, &CDropSite::droppedValueObject, this, &CAircraftIcaoForm::droppedCode);
        connect(ui->tb_Paste, &QToolButton::clicked, this, &CAircraftIcaoForm::pasted);
        ui->drop_DropData->setInfoText("<drop aircraft ICAO code>");
        ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>() });
    }

    CAircraftIcaoForm::~CAircraftIcaoForm()
    {}

    bool CAircraftIcaoForm::setValue(const BlackMisc::Aviation::CAircraftIcaoCode &icao)
    {
        if (icao == m_originalCode) { return false; }
        m_originalCode = icao;

        ui->le_Id->setText(icao.getDbKeyAsString());
        ui->aircraft_Selector->setAircraftIcao(icao);
        ui->le_Manufacturer->setText(icao.getManufacturer());
        ui->le_ModelDescription->setText(icao.getModelDescription());
        ui->le_Family->setText(icao.getFamily());
        ui->le_Iata->setText(icao.getIataCode());

        ui->cb_Legacy->setChecked(icao.isLegacyAircraft());
        ui->cb_Military->setChecked(icao.isMilitary());
        ui->cb_RealWorld->setChecked(icao.isRealWorld());
        ui->combined_TypeSelector->setCombinedType(icao.getCombinedType());

        QString rank(icao.getRankString());
        QString wtc(icao.getWtc());
        CGuiUtility::setComboBoxValueByStartingString(ui->cb_Rank, rank, "unspecified");
        CGuiUtility::setComboBoxValueByStartingString(ui->cb_Wtc, wtc, "unspecified");

        ui->le_Updated->setText(icao.getFormattedUtcTimestampYmdhms());
        return true;
    }

    void CAircraftIcaoForm::jsonPasted(const QString &json)
    {
        if (json.isEmpty()) { return; } // avoid unnecessary conversions
        try
        {
            CVariant jsonVariant;
            jsonVariant.convertFromJson(Json::jsonObjectFromString(json));
            if (!jsonVariant.canConvert<CAircraftIcaoCodeList>()) { return; }
            const CAircraftIcaoCodeList icaos = jsonVariant.value<CAircraftIcaoCodeList>();
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

    CAircraftIcaoCode CAircraftIcaoForm::getValue() const
    {
        CAircraftIcaoCode icao(ui->aircraft_Selector->getAircraftIcao());
        if (!icao.hasValidDbKey() && sGui && sGui->getWebDataServices())
        {
            // not based on DB yet, do we have a DB key
            int k = this->getDbKeyFromGui();
            if (k >= 0)
            {
                // we got an id, we get the DB object for it
                CAircraftIcaoCode fromDb(sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(k));
                if (fromDb.getDesignator() == icao.getDesignator())
                {
                    // we replace by DB object
                    icao = fromDb;
                }
            }
        }

        const QString manufacturer(ui->le_Manufacturer->text().trimmed().toUpper());
        const QString modelDescription(ui->le_ModelDescription->text().trimmed());
        const QString iata(ui->le_Iata->text().trimmed().toUpper());
        const QString family(ui->le_Family->text().trimmed().toUpper());
        const QString wtc(ui->cb_Wtc->currentText().left(1));
        const QString combined(ui->combined_TypeSelector->getCombinedType());
        bool ok;
        int rank = ui->cb_Rank->currentText().toInt(&ok);
        if (!ok) { rank = 10; }
        bool legacy = ui->cb_Legacy->isChecked();
        bool military = ui->cb_Military->isChecked();
        bool realWorld = ui->cb_RealWorld->isChecked();
        icao.setManufacturer(manufacturer);
        icao.setModelDescription(modelDescription);
        icao.setWtc(wtc);
        icao.setCodeFlags(military, legacy, realWorld);
        icao.setRank(rank);
        icao.setCombinedType(combined);
        icao.setFamily(family);
        icao.setIataCode(iata);
        return icao;
    }

    CStatusMessageList CAircraftIcaoForm::validate(bool nested) const
    {
        Q_UNUSED(nested);
        const CAircraftIcaoCode code(this->getValue());
        const CStatusMessageList msgs(code.validate());
        ui->val_Indicator->setState(msgs);
        return msgs;
    }

    void CAircraftIcaoForm::allowDrop(bool allowDrop)
    {
        ui->drop_DropData->allowDrop(allowDrop);
    }

    bool CAircraftIcaoForm::isDropAllowed() const
    {
        return ui->drop_DropData->isDropAllowed();
    }

    void CAircraftIcaoForm::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
        ui->aircraft_Selector->setReadOnly(readOnly);
        ui->le_Manufacturer->setReadOnly(readOnly);
        ui->le_ModelDescription->setReadOnly(readOnly);
        ui->le_Family->setReadOnly(readOnly);
        ui->le_Iata->setReadOnly(readOnly);
        ui->le_Id->setReadOnly(readOnly);
        ui->tb_Paste->setVisible(!readOnly);

        CGuiUtility::checkBoxReadOnly(ui->cb_Legacy, readOnly);
        CGuiUtility::checkBoxReadOnly(ui->cb_Military, readOnly);
        CGuiUtility::checkBoxReadOnly(ui->cb_RealWorld, readOnly);

        ui->cb_Wtc->setEnabled(!readOnly);
        ui->cb_Rank->setEnabled(!readOnly);
        ui->drop_DropData->setVisible(!readOnly);
        ui->combined_TypeSelector->setReadOnly(readOnly);

        this->forceStyleSheetUpdate();
    }

    void CAircraftIcaoForm::setSelectOnly()
    {
        this->setReadOnly(true);
        ui->tb_Paste->setVisible(true);
        ui->aircraft_Selector->setReadOnly(false);
        ui->le_Id->setReadOnly(false);
        ui->drop_DropData->setVisible(true);
    }

    void CAircraftIcaoForm::clear()
    {
        this->setValue(CAircraftIcaoCode());
    }

    void CAircraftIcaoForm::resetValue()
    {
        this->setValue(m_originalCode);
    }

    void CAircraftIcaoForm::droppedCode(const BlackMisc::CVariant &variantDropped)
    {
        CAircraftIcaoCode icao;
        if (variantDropped.canConvert<CAircraftIcaoCode>())
        {
            icao = variantDropped.value<CAircraftIcaoCode>();
        }
        else if (variantDropped.canConvert<CAircraftIcaoCodeList>())
        {
            const CAircraftIcaoCodeList icaoList(variantDropped.value<CAircraftIcaoCodeList>());
            if (icaoList.isEmpty()) { return; }
            icao = icaoList.front();
        }
        else
        {
            return;
        }
        this->setValue(icao);
    }

    void CAircraftIcaoForm::idEntered()
    {
        if (!sGui || !sGui->hasWebDataServices())
        {
            ui->le_Id->undo();
            return;
        }

        const int id = ui->le_Id->text().toInt();
        const CAircraftIcaoCode icao = sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(id);
        if (!icao.isLoadedFromDb())
        {
            ui->le_Id->undo();
            return;
        }
        this->setValue(icao);
    }

    int CAircraftIcaoForm::getDbKeyFromGui() const
    {
        QString key(ui->le_Id->text().trimmed());
        return IDatastoreObjectWithIntegerKey::stringToDbKey(key);
    }
} // ns

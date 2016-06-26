/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
#include "blackmisc/compare.h"
#include "blackmisc/icons.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QMetaType>
#include <QString>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Editors
    {
        CAircraftIcaoForm::CAircraftIcaoForm(QWidget *parent) :
            CForm(parent),
            ui(new Ui::CAircraftIcaoForm)
        {
            ui->setupUi(this);
            this->ui->lai_id->set(CIcons::appAircraftIcao16(), "Id:");
            this->ui->le_Updated->setReadOnly(true);
            this->ui->le_Id->setReadOnly(true);
            this->ui->aircraft_Selector->withIcaoDescription(false);
            connect(this->ui->aircraft_Selector, &CDbAircraftIcaoSelectorComponent::changedAircraftIcao, this, &CAircraftIcaoForm::setValue);

            // drag and drop
            connect(this->ui->drop_DropData, &CDropSite::droppedValueObject, this, &CAircraftIcaoForm::ps_droppedCode);
            this->ui->drop_DropData->setInfoText("<drop aircraft ICAO code>");
            this->ui->drop_DropData->setAcceptedMetaTypeIds({ qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>()});
        }

        CAircraftIcaoForm::~CAircraftIcaoForm()
        { }

        bool CAircraftIcaoForm::setValue(const BlackMisc::Aviation::CAircraftIcaoCode &icao)
        {
            if (icao == this->m_originalCode) { return false; }
            this->m_originalCode = icao;

            this->ui->le_Id->setText(icao.getDbKeyAsString());
            this->ui->aircraft_Selector->setAircraftIcao(icao);
            this->ui->le_Manufacturer->setText(icao.getManufacturer());
            this->ui->le_ModelDescription->setText(icao.getModelDescription());
            this->ui->le_Family->setText(icao.getFamily());
            this->ui->le_Iata->setText(icao.getIataCode());

            this->ui->cb_Legacy->setChecked(icao.isLegacyAircraft());
            this->ui->cb_Military->setChecked(icao.isMilitary());
            this->ui->cb_RealWorld->setChecked(icao.isRealWorld());
            this->ui->combined_TypeSelector->setCombinedType(icao.getCombinedType());

            QString rank(icao.getRankString());
            QString wtc(icao.getWtc());
            CGuiUtility::setComboBoxValueByStartingString(this->ui->cb_Rank, rank, "unspecified");
            CGuiUtility::setComboBoxValueByStartingString(this->ui->cb_Wtc, wtc, "unspecified");

            this->ui->le_Updated->setText(icao.getFormattedUtcTimestampYmdhms());
            return true;
        }

        CAircraftIcaoCode CAircraftIcaoForm::getValue() const
        {
            CAircraftIcaoCode icao(this->ui->aircraft_Selector->getAircraftIcao());
            if (!icao.hasValidDbKey())
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

            const QString manufacturer(this->ui->le_Manufacturer->text().trimmed().toUpper());
            const QString modelDescription(this->ui->le_ModelDescription->text().trimmed());
            const QString iata(this->ui->le_Family->text().trimmed().toUpper());
            const QString family(this->ui->le_Iata->text().trimmed().toUpper());
            const QString wtc(ui->cb_Wtc->currentText().left(1));
            const QString combined(ui->combined_TypeSelector->getCombinedType());
            bool ok;
            int rank = this->ui->cb_Rank->currentText().toInt(&ok);
            if (!ok) { rank = 10; }
            bool legacy = this->ui->cb_Legacy->isChecked();
            bool military = this->ui->cb_Military->isChecked();
            bool realWorld = this->ui->cb_RealWorld->isChecked();
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
            CAircraftIcaoCode code(getValue());
            CStatusMessageList msgs(code.validate());
            this->ui->val_Indicator->setState(msgs);
            return msgs;
        }

        void CAircraftIcaoForm::allowDrop(bool allowDrop)
        {
            this->ui->drop_DropData->allowDrop(allowDrop);
        }

        bool CAircraftIcaoForm::isDropAllowed() const
        {
            return ui->drop_DropData->isDropAllowed();
        }

        void CAircraftIcaoForm::setReadOnly(bool readOnly)
        {
            this->m_readOnly = readOnly;
            this->ui->aircraft_Selector->setReadOnly(readOnly);
            this->ui->le_Manufacturer->setReadOnly(readOnly);
            this->ui->le_ModelDescription->setReadOnly(readOnly);
            this->ui->le_Family->setReadOnly(readOnly);
            this->ui->le_Iata->setReadOnly(readOnly);

            CGuiUtility::checkBoxReadOnly(this->ui->cb_Legacy, readOnly);
            CGuiUtility::checkBoxReadOnly(this->ui->cb_Military, readOnly);
            CGuiUtility::checkBoxReadOnly(this->ui->cb_RealWorld, readOnly);

            this->ui->cb_Wtc->setEnabled(!readOnly);
            this->ui->cb_Rank->setEnabled(!readOnly);
            this->ui->combined_TypeSelector->setReadOnly(readOnly);
        }

        void CAircraftIcaoForm::setSelectOnly()
        {
            this->setReadOnly(true);
            this->ui->aircraft_Selector->setReadOnly(false);
        }

        void CAircraftIcaoForm::clear()
        {
            setValue(CAircraftIcaoCode());
        }

        void CAircraftIcaoForm::ps_droppedCode(const BlackMisc::CVariant &variantDropped)
        {
            CAircraftIcaoCode icao;
            if (variantDropped.canConvert<CAircraftIcaoCode>())
            {
                icao = variantDropped.value<CAircraftIcaoCode>();
            }
            else if (variantDropped.canConvert<CAircraftIcaoCodeList>())
            {
                CAircraftIcaoCodeList icaoList(variantDropped.value<CAircraftIcaoCodeList>());
                if (icaoList.isEmpty()) { return;  }
                icao =  icaoList.front();
            }
            else
            {
                return;
            }
            this->setValue(icao);
        }

        int CAircraftIcaoForm::getDbKeyFromGui() const
        {
            QString key(this->ui->le_Id->text().trimmed());
            return IDatastoreObjectWithIntegerKey::stringToDbKey(key);
        }
    } // ns
} // ns

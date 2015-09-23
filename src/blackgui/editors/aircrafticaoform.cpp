/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/guiutility.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "aircrafticaoform.h"
#include "ui_aircrafticaoform.h"

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

        void CAircraftIcaoForm::setValue(const BlackMisc::Aviation::CAircraftIcaoCode &icao)
        {
            this->m_aircraft = icao;
            this->ui->le_Id->setText(icao.getDbKeyAsString());
            this->ui->aircraft_Selector->setAircraftIcao(icao);
            this->ui->le_Manufacturer->setText(icao.getManufacturer());
            this->ui->le_ModelDescription->setText(icao.getModelDescription());

            this->ui->cb_Legacy->setChecked(icao.isLegacyAircraft());
            this->ui->cb_Military->setChecked(icao.isMilitary());
            this->ui->cb_RealWorld->setChecked(icao.isRealWorld());
            this->ui->combined_TypeSelector->setCombinedType(icao.getCombinedType());

            QString rank(icao.getRankString());
            QString wtc(icao.getWtc());
            CGuiUtility::setComboBoxValueByStartingString(this->ui->cb_Rank, rank, "unspecified");
            CGuiUtility::setComboBoxValueByStartingString(this->ui->cb_Wtc, wtc, "unspecified");

            this->ui->le_Updated->setText(icao.getFormattedUtcTimestampYmdhms());
        }

        const CAircraftIcaoCode &CAircraftIcaoForm::getValue() const
        {
            CAircraftIcaoCode icao(this->ui->aircraft_Selector->getAircraftIcao());
            if (icao.hasCompleteData())
            {
                m_aircraft = icao;
            }
            QString manufacturer(this->ui->le_Manufacturer->text().trimmed().toUpper());
            QString modelDescription(this->ui->le_ModelDescription->text());
            QString wtc(ui->cb_Wtc->currentText().left(1));
            QString combined(ui->combined_TypeSelector->getCombinedType());
            bool ok;
            int rank = this->ui->cb_Rank->currentText().toInt(&ok);
            if (!ok) { rank = 10; }
            bool legacy = this->ui->cb_Legacy->isChecked();
            bool military = this->ui->cb_Military->isChecked();
            bool realWorld = this->ui->cb_RealWorld->isChecked();
            m_aircraft.setManufacturer(manufacturer);
            m_aircraft.setModelDescription(modelDescription);
            m_aircraft.setWtc(wtc);
            m_aircraft.setCodeFlags(military, legacy, realWorld);
            m_aircraft.setRank(rank);
            m_aircraft.setCombinedType(combined);
            return m_aircraft;
        }

        CStatusMessageList CAircraftIcaoForm::validate() const
        {
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

            this->ui->cb_Legacy->setCheckable(!readOnly);
            this->ui->cb_Military->setCheckable(!readOnly);
            this->ui->cb_RealWorld->setCheckable(!readOnly);
        }

        void CAircraftIcaoForm::clear()
        {
            setValue(CAircraftIcaoCode());
        }

        void CAircraftIcaoForm::setMappingMode()
        {
            this->setReadOnly(true);
            this->ui->aircraft_Selector->setReadOnly(false);
        }

        void CAircraftIcaoForm::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            CWebDataServicesAware::setProvider(webDataReaderProvider);
            this->ui->aircraft_Selector->setProvider(webDataReaderProvider);
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

    } // ns
} // ns

/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "airlineicaofilterbar.h"
#include "ui_airlineicaofilterbar.h"
#include "blackgui/uppercasevalidator.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui
{
    namespace Filters
    {
        CAirlineIcaoFilterBar::CAirlineIcaoFilterBar(QWidget *parent) :
            CFilterWidget(parent),
            ui(new Ui::CAirlineIcaoFilterBar)
        {
            ui->setupUi(this);
            this->setButtonsAndCount(this->ui->filter_Buttons);

            connect(ui->le_Designator, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Name, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->country_Selector, &CDbCountrySelectorComponent::countryChanged, this, &CAirlineIcaoFilterBar::ps_CountryChanged);

            ui->le_Designator->setValidator(new CUpperCaseValidator(this));
        }

        CAirlineIcaoFilterBar::~CAirlineIcaoFilterBar() { }

        std::unique_ptr<BlackGui::Models::IModelFilter<CAirlineIcaoCodeList> > CAirlineIcaoFilterBar::createModelFilter() const
        {
            return std::unique_ptr<CAirlineIcaoFilter>(
                       new CAirlineIcaoFilter(
                           this->ui->le_Designator->text(),
                           this->ui->le_Name->text(),
                           this->ui->country_Selector->isSet() ? this->ui->country_Selector->getCountry().getIsoCode() : "",
                           this->ui->cb_RealAirline->isChecked(),
                           this->ui->cb_VirtualAirline->isChecked()
                       ));
        }

        void CAirlineIcaoFilterBar::setProvider(Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            this->ui->country_Selector->setProvider(webDataReaderProvider);
        }

        void CAirlineIcaoFilterBar::filter(const CAirlineIcaoCode &icao)
        {
            bool filter = false;
            if (icao.hasValidDesignator())
            {
                this->ui->le_Designator->setText(icao.getVDesignator());
                filter = true;
            }
            if (icao.hasName())
            {
                this->ui->le_Name->setText(icao.getName());
                filter = true;
            }

            if (filter)
            {
                this->ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
            }
        }

        void CAirlineIcaoFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            this->ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CAirlineIcaoFilterBar::ps_CountryChanged(const CCountry &country)
        {
            if (country.hasIsoCode())
            {
                this->ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
            }
        }

        void CAirlineIcaoFilterBar::clearForm()
        {
            this->ui->le_Designator->clear();
            this->ui->le_Name->clear();
            this->ui->country_Selector->clear();
            this->ui->cb_RealAirline->setChecked(true);
            this->ui->cb_VirtualAirline->setChecked(true);
        }

    } // ns
} // ns

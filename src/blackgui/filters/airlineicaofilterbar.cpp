// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/dbcountryselectorcomponent.h"
#include "blackgui/filters/airlineicaofilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/airlineicaofilter.h"
#include "blackgui/uppercasevalidator.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/country.h"
#include "ui_airlineicaofilterbar.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QIntValidator>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui::Filters
{
    CAirlineIcaoFilterBar::CAirlineIcaoFilterBar(QWidget *parent) : CFilterWidget(parent),
                                                                    ui(new Ui::CAirlineIcaoFilterBar)
    {
        ui->setupUi(this);
        this->setButtonsAndCount(ui->filter_Buttons);

        connect(ui->le_Designator, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Name, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_AirlineId, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->country_Selector, &CDbCountrySelectorComponent::countryChanged, this, &CAirlineIcaoFilterBar::ps_CountryChanged);

        ui->le_Designator->setValidator(new CUpperCaseValidator(ui->le_Designator));
        ui->le_AirlineId->setValidator(new QIntValidator(ui->le_AirlineId));

        // reset form
        this->clearForm();
    }

    CAirlineIcaoFilterBar::~CAirlineIcaoFilterBar() {}

    std::unique_ptr<IModelFilter<CAirlineIcaoCodeList>> CAirlineIcaoFilterBar::createModelFilter() const
    {
        return std::make_unique<CAirlineIcaoFilter>(
            convertDbId(ui->le_AirlineId->text()),
            ui->le_Designator->text().trimmed(),
            ui->le_Name->text().trimmed(),
            ui->country_Selector->isSet() ? ui->country_Selector->getCountry().getIsoCode() : "",
            ui->cb_RealAirline->isChecked(),
            ui->cb_VirtualAirline->isChecked());
    }

    void CAirlineIcaoFilterBar::filter(const CAirlineIcaoCode &icao)
    {
        bool filter = false;
        if (icao.hasValidDesignator())
        {
            ui->le_Designator->setText(icao.getVDesignator());
            filter = true;
        }
        if (icao.hasName())
        {
            ui->le_Name->setText(icao.getName());
            filter = true;
        }

        if (filter)
        {
            ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
        }
    }

    void CAirlineIcaoFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CAirlineIcaoFilterBar::ps_CountryChanged(const CCountry &country)
    {
        if (country.hasIsoCode())
        {
            ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
        }
    }

    void CAirlineIcaoFilterBar::clearForm()
    {
        ui->le_AirlineId->clear();
        ui->le_Designator->clear();
        ui->le_Name->clear();
        ui->country_Selector->clear();
        ui->cb_RealAirline->setChecked(true);
        ui->cb_VirtualAirline->setChecked(true);
    }
} // ns

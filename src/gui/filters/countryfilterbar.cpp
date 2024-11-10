// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/countryfilterbar.h"
#include "gui/filters/filterbarbuttons.h"
#include "gui/models/countryfilter.h"
#include "gui/uppercasevalidator.h"
#include "misc/countrylist.h"
#include "ui_countryfilterbar.h"

#include <QLineEdit>

using namespace swift::gui;
using namespace swift::gui::models;
using namespace swift::misc;
using namespace swift::misc::network;

namespace swift::gui::filters
{
    CCountryFilterBar::CCountryFilterBar(QWidget *parent) : CFilterWidget(parent),
                                                            ui(new Ui::CCountryFilterBar)
    {
        ui->setupUi(this);
        this->setButtonsAndCount(ui->filter_Buttons);
        connect(ui->le_IsoCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Name, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

        CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
        ui->le_IsoCode->setValidator(ucv);

        // reset form
        this->clearForm();
    }

    CCountryFilterBar::~CCountryFilterBar()
    {}

    std::unique_ptr<swift::gui::models::IModelFilter<CCountryList>> CCountryFilterBar::createModelFilter() const
    {
        return std::make_unique<CCountryFilter>(
            ui->le_IsoCode->text(),
            ui->le_Name->text());
    }

    void CCountryFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CCountryFilterBar::clearForm()
    {
        ui->le_IsoCode->clear();
        ui->le_Name->clear();
    }
} // ns

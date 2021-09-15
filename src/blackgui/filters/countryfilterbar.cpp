/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/filters/countryfilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/countryfilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/countrylist.h"
#include "ui_countryfilterbar.h"

#include <QLineEdit>

using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackGui::Filters
{
    CCountryFilterBar::CCountryFilterBar(QWidget *parent) :
        CFilterWidget(parent),
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
    { }

    std::unique_ptr<BlackGui::Models::IModelFilter<CCountryList> > CCountryFilterBar::createModelFilter() const
    {
        return std::make_unique<CCountryFilter>(
                    ui->le_IsoCode->text(),
                    ui->le_Name->text()
                );
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

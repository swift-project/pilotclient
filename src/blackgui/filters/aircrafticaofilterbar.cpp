/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircrafticaofilterbar.h"
#include "blackgui/uppercasevalidator.h"
#include "ui_aircrafticaofilterbar.h"

using namespace BlackMisc::Aviation;
using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Filters
    {
        CAircraftIcaoFilterBar::CAircraftIcaoFilterBar(QWidget *parent) :
            CFilterWidget(parent),
            ui(new Ui::CAircraftIcaoFilterBar)
        {
            ui->setupUi(this);
            this->setButtonsAndCount(this->ui->filter_Buttons);
            connect(ui->le_Designator, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Manufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Description, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            this->ui->le_Designator->setValidator(ucv);
            this->ui->le_Manufacturer->setValidator(ucv);
        }

        CAircraftIcaoFilterBar::~CAircraftIcaoFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<CAircraftIcaoCodeList> > CAircraftIcaoFilterBar::createModelFilter() const
        {
            return std::unique_ptr<CAircraftIcaoFilter>(
                       new CAircraftIcaoFilter(
                           this->ui->le_Designator->text(),
                           this->ui->le_Manufacturer->text(),
                           this->ui->le_Description->text(),
                           this->ui->combinedtype_Selector->getCombinedType()
                       ));
        }

        void CAircraftIcaoFilterBar::filter(const CAircraftIcaoCode &icao)
        {
            bool filter = false;
            if (icao.hasDesignator())
            {
                this->ui->le_Designator->setText(icao.getDesignator());
                filter = true;
            }
            if (icao.hasManufacturer())
            {
                this->ui->le_Manufacturer->setText(icao.getManufacturer());
                filter = true;
            }
            if (icao.hasModelDescription())
            {
                this->ui->le_Manufacturer->setText("*" + icao.getModelDescription() + "*");
                filter = true;
            }
            if (filter)
            {
                this->ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
            }
        }

        void CAircraftIcaoFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            this->ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CAircraftIcaoFilterBar::clearForm()
        {
            ui->le_Designator->clear();
            ui->le_Manufacturer->clear();
            ui->le_Description->clear();
            ui->combinedtype_Selector->clear();
        }

        void CAircraftIcaoFilterBar::setProvider(BlackMisc::Network::IWebDataServicesProvider *webDataReaderProvider)
        {
            Q_UNUSED(webDataReaderProvider);
        }

    } // ns
} // nss

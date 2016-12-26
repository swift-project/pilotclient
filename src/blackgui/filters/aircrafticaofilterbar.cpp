/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/aircraftcombinedtypeselector.h"
#include "blackgui/filters/aircrafticaofilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/aircrafticaofilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "ui_aircrafticaofilterbar.h"

#include <QLineEdit>
#include <QString>

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
            this->setButtonsAndCount(ui->filter_Buttons);
            connect(ui->le_Designator, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Manufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_Description, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            ui->le_Designator->setValidator(ucv);
            ui->le_Manufacturer->setValidator(ucv);

            // reset form
            this->clearForm();
        }

        CAircraftIcaoFilterBar::~CAircraftIcaoFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<CAircraftIcaoCodeList> > CAircraftIcaoFilterBar::createModelFilter() const
        {
            return std::make_unique<CAircraftIcaoFilter>(
                       ui->le_Designator->text(),
                       ui->le_Manufacturer->text(),
                       ui->le_Description->text(),
                       ui->combinedType_Selector->getCombinedType()
                   );
        }

        void CAircraftIcaoFilterBar::filter(const CAircraftIcaoCode &icao)
        {
            this->clearForm();
            bool filter = false;
            if (icao.hasDesignator())
            {
                ui->le_Designator->setText(icao.getDesignator());
                filter = true;
            }
            if (icao.hasManufacturer())
            {
                ui->le_Manufacturer->setText(icao.getManufacturer());
                filter = true;
            }
            if (icao.hasModelDescription())
            {
                ui->le_Description->setText("*" + icao.getModelDescription() + "*");
                filter = true;
            }
            if (filter)
            {
                ui->filter_Buttons->clickButton(CFilterBarButtons::Filter);
            }
            else
            {
                ui->filter_Buttons->clickButton(CFilterBarButtons::RemoveFilter);
            }
        }

        void CAircraftIcaoFilterBar::hideDescriptionField(bool hide)
        {
            ui->le_Description->setVisible(!hide);
        }

        void CAircraftIcaoFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CAircraftIcaoFilterBar::clearForm()
        {
            ui->le_Designator->clear();
            ui->le_Manufacturer->clear();
            ui->le_Description->clear();
            ui->combinedType_Selector->clear();
        }
    } // ns
} // nss

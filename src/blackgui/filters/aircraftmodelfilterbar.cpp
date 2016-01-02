/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelfilterbar.h"
#include "ui_aircraftmodelfilterbar.h"
#include "blackgui/uppercasevalidator.h"

using namespace BlackGui;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Filters
    {
        CAircraftModelFilterBar::CAircraftModelFilterBar(QWidget *parent) :
            CFilterWidget(parent),
            ui(new Ui::CAircraftModelFilterBar)
        {
            ui->setupUi(this);
            this->ui->frp_SimulatorSelector->setAll();
            this->setButtonsAndCount(this->ui->filter_Buttons);
            connect(ui->le_AircraftIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AircraftManufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AirlineIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AirlineName, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_LiveryCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_ModelDescription, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_ModelKey, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            this->ui->le_AircraftIcao->setValidator(ucv);
            this->ui->le_AirlineIcao->setValidator(ucv);
            this->ui->le_ModelKey->setValidator(ucv);
            this->ui->le_AircraftManufacturer->setValidator(ucv);
        }

        CAircraftModelFilterBar::~CAircraftModelFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList> > CAircraftModelFilterBar::createModelFilter() const
        {
            return std::unique_ptr<CAircraftModelFilter>(
                       new CAircraftModelFilter(
                           this->ui->le_ModelKey->text(),
                           this->ui->le_ModelDescription->text(),
                           this->ui->le_AircraftIcao->text(),
                           this->ui->le_AircraftManufacturer->text(),
                           this->ui->le_AirlineIcao->text(),
                           this->ui->le_AirlineName->text(),
                           this->ui->le_LiveryCode->text(),
                           this->ui->frp_SimulatorSelector->getValue()
                       ));
        }

        void CAircraftModelFilterBar::onRowCountChanged(int count, bool withFilter)
        {
            this->ui->filter_Buttons->onRowCountChanged(count, withFilter);
        }

        void CAircraftModelFilterBar::clearForm()
        {
            this->ui->le_ModelKey->clear();
            this->ui->le_ModelDescription->clear();
            this->ui->le_AircraftIcao->clear();
            this->ui->le_AircraftManufacturer->clear();
            this->ui->le_AirlineIcao->clear();
            this->ui->le_AirlineName->clear();
            this->ui->le_LiveryCode->clear();
            this->ui->frp_SimulatorSelector->setAll();
        }
    } // ns
} // ns

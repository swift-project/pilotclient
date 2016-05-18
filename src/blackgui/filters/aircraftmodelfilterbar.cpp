/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/dbdistributorselectorcomponent.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/filters/aircraftmodelfilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/aircraftmodelfilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "ui_aircraftmodelfilterbar.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

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
            this->ui->comp_DistributorSelector->withDistributorDescription(false);
            this->setButtonsAndCount(this->ui->filter_Buttons);
            connect(ui->le_AircraftIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AircraftManufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AirlineIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_AirlineName, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_LiveryCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_ModelDescription, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_ModelKey, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

            connect(ui->cb_Include, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);
            connect(ui->cb_Exclude, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);

            connect(ui->frp_SimulatorSelector, &CSimulatorSelector::changed, this, &CAircraftModelFilterBar::ps_simulatorSelectionChanged);
            connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::changedDistributor, this, &CAircraftModelFilterBar::ps_distributorChanged);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            this->ui->le_AircraftIcao->setValidator(ucv);
            this->ui->le_AirlineIcao->setValidator(ucv);
            this->ui->le_ModelKey->setValidator(ucv);
            this->ui->le_AircraftManufacturer->setValidator(ucv);
            this->ui->le_LiveryCode->setValidator(ucv);

            // reset form
            this->clearForm();
        }

        CAircraftModelFilterBar::~CAircraftModelFilterBar()
        { }

        std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList> > CAircraftModelFilterBar::createModelFilter() const
        {
            CAircraftModel::ModelModeFilter mf = this->ui->cb_Include->isChecked() ? CAircraftModel::Include : CAircraftModel::Undefined;
            if (this->ui->cb_Exclude->isChecked()) { mf |= CAircraftModel::Exclude; }
            return std::unique_ptr<CAircraftModelFilter>(
                       new CAircraftModelFilter(
                           this->ui->le_ModelKey->text(),
                           this->ui->le_ModelDescription->text(),
                           mf,
                           this->ui->le_AircraftIcao->text(),
                           this->ui->le_AircraftManufacturer->text(),
                           this->ui->le_AirlineIcao->text(),
                           this->ui->le_AirlineName->text(),
                           this->ui->le_LiveryCode->text(),
                           this->ui->frp_SimulatorSelector->getValue(),
                           this->ui->comp_DistributorSelector->getDistributor()
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
            this->ui->comp_DistributorSelector->clear();
            this->ui->cb_Exclude->setChecked(true);
            this->ui->cb_Include->setChecked(true);
        }

        void CAircraftModelFilterBar::ps_simulatorSelectionChanged(const BlackMisc::Simulation::CSimulatorInfo &info)
        {
            Q_UNUSED(info);
            this->triggerFilter();
        }

        void CAircraftModelFilterBar::ps_distributorChanged(const BlackMisc::Simulation::CDistributor &distributor)
        {
            Q_UNUSED(distributor);
            this->triggerFilter();
        }

        void CAircraftModelFilterBar::ps_checkBoxChanged(bool state)
        {
            Q_UNUSED(state);
            triggerFilter();
        }
    } // ns
} // ns

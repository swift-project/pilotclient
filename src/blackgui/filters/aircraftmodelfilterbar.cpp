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
#include "blackmisc/db/datastore.h"
#include "ui_aircraftmodelfilterbar.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Db;
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
            connect(ui->le_ModelString, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
            connect(ui->le_FileName, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

            connect(ui->cbt_Db, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);
            connect(ui->cbt_IncludeExclude, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);
            connect(ui->cbt_Military, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);
            connect(ui->cbt_ColorLiveries, &QCheckBox::toggled, this, &CAircraftModelFilterBar::ps_checkBoxChanged);

            connect(ui->frp_SimulatorSelector, &CSimulatorSelector::changed, this, &CAircraftModelFilterBar::ps_simulatorSelectionChanged);
            connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::changedDistributor, this, &CAircraftModelFilterBar::ps_distributorChanged);

            CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
            this->ui->le_AircraftIcao->setValidator(ucv);
            this->ui->le_AirlineIcao->setValidator(ucv);
            this->ui->le_ModelString->setValidator(ucv);
            this->ui->le_AircraftManufacturer->setValidator(ucv);
            this->ui->le_LiveryCode->setValidator(ucv);

            // reset form
            this->clearForm();
        }

        CAircraftModelFilterBar::~CAircraftModelFilterBar()
        { }

        void CAircraftModelFilterBar::displayCount(bool show)
        {
            ui->filter_Buttons->displayCount(show);
        }

        std::unique_ptr<BlackGui::Models::IModelFilter<BlackMisc::Simulation::CAircraftModelList> > CAircraftModelFilterBar::createModelFilter() const
        {
            CAircraftModel::ModelModeFilter mf = CAircraftModel::All;
            if (ui->cbt_IncludeExclude->checkState() == Qt::Checked)
            {
                mf = CAircraftModel::Include;
            }
            else if (ui->cbt_IncludeExclude->checkState() == Qt::Unchecked)
            {
                mf = CAircraftModel::Exclude;
            }

            BlackMisc::Db::DbKeyStateFilter dbf = BlackMisc::Db::All;
            if (ui->cbt_Db->checkState() == Qt::Checked)
            {
                dbf = BlackMisc::Db::Valid;
            }
            else if (ui->cbt_Db->checkState() == Qt::Unchecked)
            {
                dbf = BlackMisc::Db::Invalid;
            }

            return std::unique_ptr<CAircraftModelFilter>(
                       new CAircraftModelFilter(
                           this->ui->le_ModelString->text(),
                           this->ui->le_ModelDescription->text(),
                           mf,
                           dbf,
                           ui->cbt_Military->checkState(),
                           ui->cbt_ColorLiveries->checkState(),
                           this->ui->le_AircraftIcao->text(),
                           this->ui->le_AircraftManufacturer->text(),
                           this->ui->le_AirlineIcao->text(),
                           this->ui->le_AirlineName->text(),
                           this->ui->le_LiveryCode->text(),
                           this->ui->le_FileName->text(),
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
            this->ui->le_ModelString->clear();
            this->ui->le_ModelDescription->clear();
            this->ui->le_AircraftIcao->clear();
            this->ui->le_AircraftManufacturer->clear();
            this->ui->le_AirlineIcao->clear();
            this->ui->le_AirlineName->clear();
            this->ui->le_LiveryCode->clear();
            this->ui->le_FileName->clear();
            this->ui->frp_SimulatorSelector->setAll();
            this->ui->comp_DistributorSelector->clear();
            this->ui->cbt_IncludeExclude->setCheckState(Qt::PartiallyChecked);
            this->ui->cbt_Db->setCheckState(Qt::PartiallyChecked);
            this->ui->cbt_Military->setCheckState(Qt::PartiallyChecked);
            this->ui->cbt_ColorLiveries->setCheckState(Qt::PartiallyChecked);
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

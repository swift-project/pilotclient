/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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
#include <QPointer>
#include <QTimer>

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Db;
using namespace BlackGui;
using namespace BlackGui::Models;
using namespace BlackGui::Components;

namespace BlackGui::Filters
{
    CAircraftModelFilterBar::CAircraftModelFilterBar(QWidget *parent) : CFilterWidget(parent),
                                                                        ui(new Ui::CAircraftModelFilterBar)
    {
        ui->setupUi(this);

        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::CheckBoxes);
        ui->comp_SimulatorSelector->setNoSelectionMeansAll(true);
        ui->comp_SimulatorSelector->setRememberSelection(false);
        ui->comp_SimulatorSelector->checkAll();

        ui->comp_DistributorSelector->withDistributorDescription(false);
        this->setButtonsAndCount(ui->filter_Buttons);

        CUpperCaseValidator *ucv = new CUpperCaseValidator(this);
        ui->le_AircraftIcao->setValidator(ucv);
        ui->le_AirlineIcao->setValidator(ucv);
        ui->le_ModelString->setValidator(ucv);
        ui->le_AircraftManufacturer->setValidator(ucv);
        ui->le_LiveryCode->setValidator(ucv);
        ui->le_Id->setValidator(new QIntValidator(ui->le_Id));

        // reset form
        this->clearForm();

        // connect deferred, avoid to filter during the UI "swing in period"
        QPointer<CAircraftModelFilterBar> myself(this);
        QTimer::singleShot(2500, this, [=] {
            if (!myself) { return; }
            this->connectTriggerFilterSignals();
        });
    }

    CAircraftModelFilterBar::~CAircraftModelFilterBar()
    {}

    void CAircraftModelFilterBar::displayCount(bool show)
    {
        ui->filter_Buttons->displayCount(show);
    }

    std::unique_ptr<IModelFilter<CAircraftModelList>> CAircraftModelFilterBar::createModelFilter() const
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

        DbKeyStateFilter dbf = BlackMisc::Db::All;
        if (ui->cbt_Db->checkState() == Qt::Checked)
        {
            dbf = BlackMisc::Db::Valid;
        }
        else if (ui->cbt_Db->checkState() == Qt::Unchecked)
        {
            dbf = BlackMisc::Db::Invalid;
        }

        return std::make_unique<CAircraftModelFilter>(
            convertDbId(ui->le_Id->text()),
            ui->le_ModelString->text(),
            ui->le_ModelDescription->text(),
            mf,
            dbf,
            ui->cbt_Military->checkState(),
            ui->cbt_ColorLiveries->checkState(),
            ui->le_AircraftIcao->text(),
            ui->le_AircraftManufacturer->text(),
            ui->le_AirlineIcao->text(),
            ui->le_AirlineName->text(),
            ui->le_LiveryCode->text(),
            ui->le_FileName->text(),
            ui->comp_CombinedType->getCombinedType(),
            ui->comp_SimulatorSelector->getValue(),
            ui->comp_DistributorSelector->getDistributor());
    }

    void CAircraftModelFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CAircraftModelFilterBar::clearForm()
    {
        ui->le_ModelString->clear();
        ui->le_ModelDescription->clear();
        ui->le_AircraftIcao->clear();
        ui->le_AircraftManufacturer->clear();
        ui->le_AirlineIcao->clear();
        ui->le_AirlineName->clear();
        ui->le_LiveryCode->clear();
        ui->le_FileName->clear();
        ui->comp_SimulatorSelector->checkAll();
        ui->comp_DistributorSelector->clear();
        ui->comp_CombinedType->clear();
        ui->cbt_IncludeExclude->setCheckState(Qt::PartiallyChecked);
        ui->cbt_Db->setCheckState(Qt::PartiallyChecked);
        ui->cbt_Military->setCheckState(Qt::PartiallyChecked);
        ui->cbt_ColorLiveries->setCheckState(Qt::PartiallyChecked);
    }

    void CAircraftModelFilterBar::onSimulatorSelectionChanged(const CSimulatorInfo &info)
    {
        Q_UNUSED(info);
        this->triggerFilter();
    }

    void CAircraftModelFilterBar::onDistributorChanged(const CDistributor &distributor)
    {
        Q_UNUSED(distributor);
        this->triggerFilter();
    }

    void CAircraftModelFilterBar::onCombinedTypeChanged(const QString &combinedType)
    {
        Q_UNUSED(combinedType);
        this->triggerFilter();
    }

    void CAircraftModelFilterBar::onCheckBoxChanged(bool state)
    {
        Q_UNUSED(state);
        triggerFilter();
    }

    void CAircraftModelFilterBar::connectTriggerFilterSignals()
    {
        connect(ui->le_AircraftIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_AircraftManufacturer, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_AirlineIcao, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_AirlineName, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_LiveryCode, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_Id, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_ModelDescription, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_ModelString, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);
        connect(ui->le_FileName, &QLineEdit::returnPressed, this, &CFilterWidget::triggerFilter);

        connect(ui->cbt_Db, &QCheckBox::clicked, this, &CAircraftModelFilterBar::onCheckBoxChanged);
        connect(ui->cbt_IncludeExclude, &QCheckBox::clicked, this, &CAircraftModelFilterBar::onCheckBoxChanged);
        connect(ui->cbt_Military, &QCheckBox::clicked, this, &CAircraftModelFilterBar::onCheckBoxChanged);
        connect(ui->cbt_ColorLiveries, &QCheckBox::clicked, this, &CAircraftModelFilterBar::onCheckBoxChanged);

        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CAircraftModelFilterBar::onSimulatorSelectionChanged);
        connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::changedDistributor, this, &CAircraftModelFilterBar::onDistributorChanged);
        connect(ui->comp_CombinedType, &CAircraftCombinedTypeSelector::changedCombinedType, this, &CAircraftModelFilterBar::onCombinedTypeChanged);
    }
} // ns

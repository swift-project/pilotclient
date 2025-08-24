// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/aircraftmodelfilterbar.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPointer>
#include <QTimer>
#include <QtGlobal>

#include "ui_aircraftmodelfilterbar.h"

#include "gui/components/dbdistributorselectorcomponent.h"
#include "gui/components/simulatorselector.h"
#include "gui/filters/filterbarbuttons.h"
#include "gui/models/aircraftmodelfilter.h"
#include "gui/uppercasevalidator.h"
#include "misc/db/datastore.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"

using namespace swift::misc::simulation;
using namespace swift::misc::db;
using namespace swift::gui;
using namespace swift::gui::models;
using namespace swift::gui::components;

namespace swift::gui::filters
{
    CAircraftModelFilterBar::CAircraftModelFilterBar(QWidget *parent)
        : CFilterWidget(parent), ui(new Ui::CAircraftModelFilterBar)
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
        QTimer::singleShot(2500, this, [=, this] {
            if (!myself) { return; }
            this->connectTriggerFilterSignals();
        });
    }

    CAircraftModelFilterBar::~CAircraftModelFilterBar() {}

    void CAircraftModelFilterBar::displayCount(bool show) { ui->filter_Buttons->displayCount(show); }

    std::unique_ptr<IModelFilter<CAircraftModelList>> CAircraftModelFilterBar::createModelFilter() const
    {
        CAircraftModel::ModelModeFilter mf = CAircraftModel::All;
        if (ui->cbt_IncludeExclude->checkState() == Qt::Checked) { mf = CAircraftModel::Include; }
        else if (ui->cbt_IncludeExclude->checkState() == Qt::Unchecked) { mf = CAircraftModel::Exclude; }

        DbKeyStateFilter dbf = swift::misc::db::All;
        if (ui->cbt_Db->checkState() == Qt::Checked) { dbf = swift::misc::db::Valid; }
        else if (ui->cbt_Db->checkState() == Qt::Unchecked) { dbf = swift::misc::db::Invalid; }

        return std::make_unique<CAircraftModelFilter>(
            convertDbId(ui->le_Id->text()), ui->le_ModelString->text(), ui->le_ModelDescription->text(), mf, dbf,
            ui->cbt_Military->checkState(), ui->cbt_ColorLiveries->checkState(), ui->le_AircraftIcao->text(),
            ui->le_AircraftManufacturer->text(), ui->le_AirlineIcao->text(), ui->le_AirlineName->text(),
            ui->le_LiveryCode->text(), ui->le_FileName->text(), ui->comp_CombinedType->getCombinedType(),
            ui->comp_SimulatorSelector->getValue(), ui->comp_DistributorSelector->getDistributor());
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

        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                &CAircraftModelFilterBar::onSimulatorSelectionChanged);
        connect(ui->comp_DistributorSelector, &CDbDistributorSelectorComponent::changedDistributor, this,
                &CAircraftModelFilterBar::onDistributorChanged);
        connect(ui->comp_CombinedType, &CAircraftCombinedTypeSelector::changedCombinedType, this,
                &CAircraftModelFilterBar::onCombinedTypeChanged);
    }
} // namespace swift::gui::filters

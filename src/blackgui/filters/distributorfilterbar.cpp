/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/filters/distributorfilterbar.h"
#include "blackgui/filters/filterbarbuttons.h"
#include "blackgui/models/distributorfilter.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/simulation/distributorlist.h"
#include "ui_distributorfilterbar.h"

#include <QLineEdit>

using namespace BlackGui::Models;
using namespace BlackGui::Components;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui::Filters
{
    CDistributorFilterBar::CDistributorFilterBar(QWidget *parent) :
        CFilterWidget(parent),
        ui(new Ui::CDistributorFilterBar)
    {
        ui->setupUi(this);
        ui->comp_Simulator->setMode(CSimulatorSelector::CheckBoxes);
        ui->comp_Simulator->setNoSelectionMeansAll(true);
        ui->comp_Simulator->setRememberSelection(false);
        ui->comp_Simulator->checkAll();

        this->setButtonsAndCount(ui->filter_Buttons);

        // reset form
        this->clearForm();

        // connect deferred, avoid to filter during the UI "swing in period"
        QPointer<CDistributorFilterBar> myself(this);
        QTimer::singleShot(2500, this, [ = ]
        {
            if (!myself) { return; }
            this->connectTriggerFilterSignals();
        });
    }

    CDistributorFilterBar::~CDistributorFilterBar()
    { }

    std::unique_ptr<BlackGui::Models::IModelFilter<CDistributorList> > CDistributorFilterBar::createModelFilter() const
    {
        return std::make_unique<CDistributorFilter>(ui->comp_Simulator->getValue());
    }

    void CDistributorFilterBar::setSimulator(const CSimulatorInfo &simulator)
    {
        ui->comp_Simulator->setValue(simulator);
    }

    void CDistributorFilterBar::onRowCountChanged(int count, bool withFilter)
    {
        ui->filter_Buttons->onRowCountChanged(count, withFilter);
    }

    void CDistributorFilterBar::clearForm()
    {
        ui->comp_Simulator->clear();
    }

    void CDistributorFilterBar::connectTriggerFilterSignals()
    {
        connect(ui->comp_Simulator, &CSimulatorSelector::changed, this, &CFilterWidget::triggerFilter, Qt::QueuedConnection);
    }
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/filters/aircraftmodelfilterbar.h"
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "ui_aircraftmodelfilterdialog.h"

using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackGui::Filters
{
    CAircraftModelFilterDialog::CAircraftModelFilterDialog(QWidget *parent) : CFilterDialog(parent),
                                                                              ui(new Ui::CAircraftModelFilterDialog)
    {
        ui->setupUi(this);
        this->setWindowTitle("Filter models");
        this->resize(640, 100);
        ui->filter_Models->displayCount(false);
        connect(ui->filter_Models, &CAircraftModelFilterBar::rejectDialog, this, &CAircraftModelFilterDialog::reject);
    }

    CAircraftModelFilterDialog::~CAircraftModelFilterDialog()
    {}

    std::unique_ptr<IModelFilter<CAircraftModelList>> CAircraftModelFilterDialog::createModelFilter() const
    {
        return ui->filter_Models->createModelFilter();
    }
} // namespace

// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/filters/aircraftmodelfilterdialog.h"

#include "ui_aircraftmodelfilterdialog.h"

#include "gui/filters/aircraftmodelfilterbar.h"
#include "misc/simulation/aircraftmodellist.h"

using namespace swift::gui::models;
using namespace swift::gui::filters;
using namespace swift::misc::simulation;
using namespace swift::misc::network;

namespace swift::gui::filters
{
    CAircraftModelFilterDialog::CAircraftModelFilterDialog(QWidget *parent)
        : CFilterDialog(parent), ui(new Ui::CAircraftModelFilterDialog)
    {
        ui->setupUi(this);
        this->setWindowTitle("Filter models");
        this->resize(640, 100);
        ui->filter_Models->displayCount(false);
        connect(ui->filter_Models, &CAircraftModelFilterBar::rejectDialog, this, &CAircraftModelFilterDialog::reject);
    }

    CAircraftModelFilterDialog::~CAircraftModelFilterDialog() {}

    std::unique_ptr<IModelFilter<CAircraftModelList>> CAircraftModelFilterDialog::createModelFilter() const
    {
        return ui->filter_Models->createModelFilter();
    }
} // namespace swift::gui::filters

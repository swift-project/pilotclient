/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

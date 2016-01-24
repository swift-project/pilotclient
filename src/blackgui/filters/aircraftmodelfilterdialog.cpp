/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelfilterdialog.h"
#include "ui_aircraftmodelfilterdialog.h"

using namespace BlackGui::Models;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Filters
    {
        CAircraftModelFilterDialog::CAircraftModelFilterDialog(QWidget *parent) :
            CFilterDialog(parent),
            ui(new Ui::CAircraftModelFilterDialog)
        {
            ui->setupUi(this);
            this->setWindowTitle("Filter models");
            this->resize(640, 100);
        }

        CAircraftModelFilterDialog::~CAircraftModelFilterDialog()
        { }

        std::unique_ptr<IModelFilter<CAircraftModelList>> CAircraftModelFilterDialog::createModelFilter() const
        {
            return ui->filter_Models->createModelFilter();
        }

        void CAircraftModelFilterDialog::setProvider(IWebDataServicesProvider *webDataReaderProvider)
        {
            this->ui->filter_Models->setProvider(webDataReaderProvider);
        }
    } // namespace
} // namespace

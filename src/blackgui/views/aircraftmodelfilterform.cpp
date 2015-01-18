/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelfilterform.h"
#include "ui_aircraftmodelfilterform.h"

using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {

        CAircraftModelFilterForm::CAircraftModelFilterForm(QWidget *parent) :
            CFilterDialog(parent),
            ui(new Ui::CAircraftModelFilterForm)
        {
            ui->setupUi(this);
            this->setWindowTitle("Filter models");
        }

        CAircraftModelFilterForm::~CAircraftModelFilterForm()
        { }

        std::unique_ptr<CAircraftModelFilter> CAircraftModelFilterForm::getFilter() const
        {
            QString model(this->ui->le_ModelString->text());
            QString desc(this->ui->le_ModelDescription->text());
            return std::unique_ptr<CAircraftModelFilter>(new CAircraftModelFilter(model, desc));
        }

    } // namespace
} // namespace

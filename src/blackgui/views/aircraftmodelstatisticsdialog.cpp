/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelstatisticsdialog.h"
#include "ui_aircraftmodelstatisticsdialog.h"

using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelStatisticsDialog::CAircraftModelStatisticsDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAircraftModelStatisticsDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
        }

        CAircraftModelStatisticsDialog::~CAircraftModelStatisticsDialog()
        { }

        void CAircraftModelStatisticsDialog::analyzeModels(const CAircraftModelList &models)
        {
            ui->te_Statistics->setHtml(models.htmlStatistics(true, true));
        }
    } // ns
} // ns

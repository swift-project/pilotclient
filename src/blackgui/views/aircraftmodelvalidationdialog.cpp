/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelvalidationdialog.h"
#include "ui_aircraftmodelvalidationdialog.h"
#include "blackmisc/simulation/aircraftmodelutils.h"

#include <QPushButton>
#include <QDialogButtonBox>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Views
    {
        CAircraftModelValidationDialog::CAircraftModelValidationDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAircraftModelValidationDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

            QPushButton *validateButton = new QPushButton("Validate", ui->bb_ValidationDialog);
            ui->bb_ValidationDialog->addButton(validateButton, QDialogButtonBox::ActionRole);
            connect(validateButton, &QPushButton::released, this, &CAircraftModelValidationDialog::validate);
        }

        CAircraftModelValidationDialog::~CAircraftModelValidationDialog()
        { }

        void CAircraftModelValidationDialog::validate()
        {
            ui->comp_StatusMessage->clear();
            if (m_models.isEmpty()) { return; }

            CAircraftModelList valid;
            CAircraftModelList invalid;
            const CCountPerSimulator counts = m_models.countPerSimulator();
            const double fsFamilyCount = counts.getCountForFsFamilySimulators();
            const double fsRatio = fsFamilyCount / m_models.size();
            const bool ignoreEmpty = false;
            const int maxFailedFiles = 25;
            const CStatusMessageList msgs = fsRatio > 0.9 ?
                                            CFsCommonUtil::validateConfigFiles(m_models, valid, invalid, ignoreEmpty, maxFailedFiles) :
                                            m_models.validateFiles(valid, invalid, ignoreEmpty, maxFailedFiles);
            ui->comp_StatusMessage->clear();
            ui->comp_StatusMessage->appendStatusMessagesToList(msgs);
        }

    } // ns
} // ns

/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelvalidationdialog.h"
#include "ui_aircraftmodelvalidationdialog.h"
#include "blackmisc/simulation/aircraftmodelutils.h"

#include <QPushButton>
#include <QDialogButtonBox>
#include <QPointer>
#include <atomic>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui::Views
{
    CAircraftModelValidationDialog::CAircraftModelValidationDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CAircraftModelValidationDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

        QPushButton *validateButton = new QPushButton("Validate", ui->bb_ValidationDialog);
        ui->bb_ValidationDialog->addButton(validateButton, QDialogButtonBox::ActionRole);
        connect(validateButton, &QPushButton::released, this, &CAircraftModelValidationDialog::validate, Qt::QueuedConnection);
    }

    CAircraftModelValidationDialog::~CAircraftModelValidationDialog()
    { }

    void CAircraftModelValidationDialog::setModels(const CAircraftModelList &models, const CSimulatorInfo &simulator, const QString &simulatorDir)
    {
        m_models       = models;
        m_simulator    = simulator;
        m_simulatorDir = simulatorDir;
    }

    void CAircraftModelValidationDialog::triggerValidation(int delayMs)
    {
        QPointer<CAircraftModelValidationDialog> myself(this);
        QTimer::singleShot(delayMs, this, [ = ]
        {
            if (!myself) { return; }
            this->validate();
        });
    }

    void CAircraftModelValidationDialog::validate()
    {
        // in view validation of models
        ui->comp_StatusMessage->clear();
        if (m_models.isEmpty()) { return; }

        CAircraftModelList valid;
        CAircraftModelList invalid;
        const bool ignoreEmpty = false;
        const int  maxFailedFiles = 25;
        std::atomic_bool wasStopped { false };
        const CStatusMessageList msgs = CAircraftModelUtilities::validateModelFiles(m_simulator, m_models, valid, invalid, ignoreEmpty, maxFailedFiles, wasStopped, m_simulatorDir);
        ui->comp_StatusMessage->clear();
        ui->comp_StatusMessage->setNoSorting(); // we use the pre-sorted list
        ui->comp_StatusMessage->appendStatusMessagesToList(msgs);
    }

} // ns

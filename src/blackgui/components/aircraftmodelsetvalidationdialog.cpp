/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "aircraftmodelsetvalidationdialog.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "ui_aircraftmodelsetvalidationdialog.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CAircraftModelSetValidationDialog::CAircraftModelSetValidationDialog(QWidget *parent) :
            QDialog(parent),
            ui(new Ui::CAircraftModelSetValidationDialog)
        {
            ui->setupUi(this);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        }

        CAircraftModelSetValidationDialog::~CAircraftModelSetValidationDialog()
        { }

        void CAircraftModelSetValidationDialog::validatedModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &valid, const CAircraftModelList &invalid, bool stopped, const CStatusMessageList &msgs)
        {
            ui->comp_AircraftModelValidation->validatedModelSet(simulator, valid, invalid, stopped, msgs);
            this->setWindowTitle(QStringLiteral("Model validation for '%1'").arg(simulator.toQString(true)));
        }
    } // ns
} // ns

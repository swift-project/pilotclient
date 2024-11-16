// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftmodelsetvalidationdialog.h"

#include "ui_aircraftmodelsetvalidationdialog.h"

#include "gui/guiapplication.h"
#include "misc/simulation/simulatorinfo.h"

using namespace swift::misc;
using namespace swift::misc::simulation;

namespace swift::gui::components
{
    CAircraftModelSetValidationDialog::CAircraftModelSetValidationDialog(QWidget *parent) : QDialog(parent),
                                                                                            ui(new Ui::CAircraftModelSetValidationDialog)
    {
        ui->setupUi(this);
        this->setWindowFlags(this->windowFlags() | Qt::WindowContextHelpButtonHint);
    }

    CAircraftModelSetValidationDialog::~CAircraftModelSetValidationDialog()
    {}

    void CAircraftModelSetValidationDialog::validatedModelSet(const CSimulatorInfo &simulator, const CAircraftModelList &valid, const CAircraftModelList &invalid, bool stopped, const CStatusMessageList &msgs)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        ui->comp_AircraftModelValidation->validatedModelSet(simulator, valid, invalid, stopped, msgs);
        this->setWindowTitle(QStringLiteral("Model validation for '%1'").arg(simulator.toQString(true)));
        CGuiApplication::modalWindowToFront();
    }

    bool CAircraftModelSetValidationDialog::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }
} // namespace swift::gui::components

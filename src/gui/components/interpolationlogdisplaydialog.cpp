// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "interpolationlogdisplaydialog.h"

#include "ui_interpolationlogdisplaydialog.h"

#include "core/simulator.h"
#include "gui/guiapplication.h"

using namespace swift::core;

namespace swift::gui::components
{
    CInterpolationLogDisplayDialog::CInterpolationLogDisplayDialog(ISimulator *simulator,
                                                                   CAirspaceMonitor *airspaceMonitor, QWidget *parent)
        : QDialog(parent), ui(new Ui::CInterpolationLogDisplayDialog)
    {
        ui->setupUi(this);
        this->setSimulator(simulator);

        if (airspaceMonitor) { this->setAirspaceMonitor(airspaceMonitor); }
        else { ui->comp_InterpolationLogDisplay->linkWithAirspaceMonitor(); }
    }

    CInterpolationLogDisplayDialog::~CInterpolationLogDisplayDialog()
    {
        //! \todo KB 2018-05 this is a hack, I have no idea why I need to invalidate the parent. But without doing it,
        //! the dtor of comp_InterpolationLogDisplay is called tywice
        ui->comp_InterpolationLogDisplay->setParent(nullptr);
    }

    void CInterpolationLogDisplayDialog::setSimulator(ISimulator *simulator)
    {
        ui->comp_InterpolationLogDisplay->setSimulator(simulator);
    }

    void CInterpolationLogDisplayDialog::setAirspaceMonitor(CAirspaceMonitor *airspaceMonitor)
    {
        ui->comp_InterpolationLogDisplay->setAirspaceMonitor(airspaceMonitor);
    }

    bool CInterpolationLogDisplayDialog::event(QEvent *event)
    {
        if (CGuiApplication::triggerShowHelp(this, event)) { return true; }
        return QDialog::event(event);
    }
} // namespace swift::gui::components

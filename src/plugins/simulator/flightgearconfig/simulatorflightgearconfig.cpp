// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorflightgearconfig.h"

#include "simulatorflightgearconfigwindow.h"

class QWidget;

namespace swift::simplugin::flightgear
{
    CSimulatorFlightgearConfig::CSimulatorFlightgearConfig(QObject *parent) : QObject(parent) {}

    swift::gui::CPluginConfigWindow *CSimulatorFlightgearConfig::createConfigWindow(QWidget *parent)
    {
        auto w = new CSimulatorFlightgearConfigWindow(parent);
        return w;
    }
} // namespace swift::simplugin::flightgear

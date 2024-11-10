// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

class QWidget;

namespace swift::simplugin::xplane
{
    CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    swift::gui::CPluginConfigWindow *CSimulatorXPlaneConfig::createConfigWindow(QWidget *parent)
    {
        CSimulatorXPlaneConfigWindow *w = new CSimulatorXPlaneConfigWindow(parent);
        return w;
    }
} // ns

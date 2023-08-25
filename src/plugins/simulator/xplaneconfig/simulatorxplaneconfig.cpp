// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorxplaneconfig.h"
#include "simulatorxplaneconfigwindow.h"

class QWidget;

namespace BlackSimPlugin::XPlane
{
    CSimulatorXPlaneConfig::CSimulatorXPlaneConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    BlackGui::CPluginConfigWindow *CSimulatorXPlaneConfig::createConfigWindow(QWidget *parent)
    {
        CSimulatorXPlaneConfigWindow *w = new CSimulatorXPlaneConfigWindow(parent);
        return w;
    }
} // ns

// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorp3dconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

namespace swift::simplugin::p3d
{
    CSimulatorP3DConfig::CSimulatorP3DConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    swift::gui::CPluginConfigWindow *CSimulatorP3DConfig::createConfigWindow(QWidget *parent)
    {
        return new fsxcommon::CSimulatorFsxConfigWindow("P3D", parent);
    }
}

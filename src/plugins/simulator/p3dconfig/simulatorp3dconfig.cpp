// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorp3dconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

namespace BlackSimPlugin::P3D
{
    CSimulatorP3DConfig::CSimulatorP3DConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    BlackGui::CPluginConfigWindow *CSimulatorP3DConfig::createConfigWindow(QWidget *parent)
    {
        return new FsxCommon::CSimulatorFsxConfigWindow("P3D", parent);
    }
}

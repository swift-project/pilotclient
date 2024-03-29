// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfsxconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

using namespace BlackGui;
using namespace BlackSimPlugin::FsxCommon;

namespace BlackSimPlugin::Fsx
{
    CSimulatorFsxConfig::CSimulatorFsxConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    CPluginConfigWindow *CSimulatorFsxConfig::createConfigWindow(QWidget *parent)
    {
        return new CSimulatorFsxConfigWindow("FSX", parent);
    }
}

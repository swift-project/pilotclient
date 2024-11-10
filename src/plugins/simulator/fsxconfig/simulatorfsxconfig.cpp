// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatorfsxconfig.h"
#include "../fsxcommon/simulatorfsxconfigwindow.h"

using namespace swift::gui;
using namespace swift::simplugin::fsxcommon;

namespace swift::simplugin::fsx
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

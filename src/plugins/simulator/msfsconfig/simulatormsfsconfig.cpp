// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfsconfig.h"

#include "../fsxcommon/simulatorfsxconfigwindow.h"

namespace swift::simplugin::msfs
{
    CSimulatorMsfsConfig::CSimulatorMsfsConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    swift::gui::CPluginConfigWindow *CSimulatorMsfsConfig::createConfigWindow(QWidget *parent)
    {
        return new fsxcommon::CSimulatorFsxConfigWindow("MSFS", parent);
    }
} // namespace swift::simplugin::msfs

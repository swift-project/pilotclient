// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatormsfs2024config.h"

#include "../fsxcommon/simulatorfsxconfigwindow.h"

namespace swift::simplugin::msfs2024
{
    CSimulatorMsfs2024Config::CSimulatorMsfs2024Config(QObject *parent) : QObject(parent)
    {
        // void
    }

    swift::gui::CPluginConfigWindow *CSimulatorMsfs2024Config::createConfigWindow(QWidget *parent)
    {
        return new fsxcommon::CSimulatorFsxConfigWindow("MSFS2024", parent);
    }
} // namespace swift::simplugin::msfs2024

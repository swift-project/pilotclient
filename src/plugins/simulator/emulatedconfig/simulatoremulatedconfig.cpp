// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatoremulatedconfig.h"
#include "simulatoremulatedconfigwidget.h"

namespace BlackSimPlugin::Emulated
{
    CSimulatorEmulatedConfig::CSimulatorEmulatedConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    BlackGui::CPluginConfigWindow *CSimulatorEmulatedConfig::createConfigWindow(QWidget *parent)
    {
        return new CSimulatorEmulatedConfigWidget(parent);
    }
} // ns

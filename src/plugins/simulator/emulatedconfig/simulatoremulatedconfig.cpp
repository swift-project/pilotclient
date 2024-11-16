// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "simulatoremulatedconfig.h"

#include "simulatoremulatedconfigwidget.h"

namespace swift::simplugin::emulated
{
    CSimulatorEmulatedConfig::CSimulatorEmulatedConfig(QObject *parent) : QObject(parent)
    {
        // void
    }

    swift::gui::CPluginConfigWindow *CSimulatorEmulatedConfig::createConfigWindow(QWidget *parent)
    {
        return new CSimulatorEmulatedConfigWidget(parent);
    }
} // namespace swift::simplugin::emulated

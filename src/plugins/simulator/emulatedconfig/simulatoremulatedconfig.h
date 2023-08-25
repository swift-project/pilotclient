// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIG_H
#define BLACKSIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIG_H

#include "blackgui/pluginconfig.h"

namespace BlackSimPlugin::Emulated
{
    /*!
     * Window for setting up the FSX plugin.
     */
    class CSimulatorEmulatedConfig : public QObject, public BlackGui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulatoremulatedconfig.json")
        Q_INTERFACES(BlackGui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorEmulatedConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorEmulatedConfig() override {}

        //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
        BlackGui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // ns

#endif // guard

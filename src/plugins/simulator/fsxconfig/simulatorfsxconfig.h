// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FSX_CONFIG_H
#define BLACKSIMPLUGIN_SIMULATOR_FSX_CONFIG_H

#include "blackgui/pluginconfig.h"
#include "misc/settingscache.h"

namespace BlackSimPlugin::Fsx
{
    /*!
     * Window for setting up the FSX plugin.
     */
    class CSimulatorFsxConfig : public QObject, public BlackGui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulatorfsxconfig.json")
        Q_INTERFACES(BlackGui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorFsxConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorFsxConfig() override {}

        //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
        BlackGui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // ns

#endif // guard

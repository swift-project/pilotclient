// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_P3D_SIMULATORP3DCONFIG_H
#define BLACKSIMPLUGIN_P3D_SIMULATORP3DCONFIG_H

#include "blackgui/pluginconfig.h"

namespace BlackSimPlugin::P3D
{
    /*!
     * Window for setting up the P3D plugin.
     */
    class CSimulatorP3DConfig : public QObject, public BlackGui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulatorp3dconfig.json")
        Q_INTERFACES(BlackGui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorP3DConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorP3DConfig() {}

        //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
        BlackGui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
}

#endif // guard

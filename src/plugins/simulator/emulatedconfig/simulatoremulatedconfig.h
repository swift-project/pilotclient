/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

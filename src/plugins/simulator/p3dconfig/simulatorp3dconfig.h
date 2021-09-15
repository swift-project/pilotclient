/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

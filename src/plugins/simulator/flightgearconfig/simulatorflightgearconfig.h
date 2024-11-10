// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H
#define BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H

#include "blackgui/pluginconfig.h"
#include "misc/settingscache.h"

#include <QObject>
#include <QString>

class QWidget;

namespace BlackGui
{
    class CPluginConfigWindow;
}
namespace BlackSimPlugin::Flightgear
{
    /*!
     * Config plugin for the Flightgear plugin.
     */
    class CSimulatorFlightgearConfig : public QObject, public BlackGui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulatorflightgearconfig.json")
        Q_INTERFACES(BlackGui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorFlightgearConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorFlightgearConfig() {}

        //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
        BlackGui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // ns

#endif // guard

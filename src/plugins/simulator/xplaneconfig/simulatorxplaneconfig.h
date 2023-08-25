// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_H
#define BLACKSIMPLUGIN_SIMULATOR_XPLANE_CONFIG_H

#include "blackgui/pluginconfig.h"
#include "blackmisc/settingscache.h"

#include <QObject>
#include <QString>

class QWidget;

namespace BlackGui
{
    class CPluginConfigWindow;
}
namespace BlackSimPlugin::XPlane
{
    /*!
     * Config plugin for the X-Plane plugin.
     */
    class CSimulatorXPlaneConfig : public QObject, public BlackGui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.blackgui.pluginconfiginterface" FILE "simulatorxplaneconfig.json")
        Q_INTERFACES(BlackGui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorXPlaneConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorXPlaneConfig() override {}

        //! \copydoc BlackGui::IPluginConfig::createConfigWindow()
        BlackGui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // ns

#endif // guard

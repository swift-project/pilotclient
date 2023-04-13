/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H
#define BLACKSIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H

#include "blackgui/pluginconfig.h"
#include "blackmisc/settingscache.h"

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

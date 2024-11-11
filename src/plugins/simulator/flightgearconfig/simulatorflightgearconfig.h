// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H
#define SWIFT_SIMPLUGIN_SIMULATOR_FLIGHTGEAR_CONFIG_H

#include "gui/pluginconfig.h"
#include "misc/settingscache.h"

#include <QObject>
#include <QString>

class QWidget;

namespace swift::gui
{
    class CPluginConfigWindow;
}
namespace swift::simplugin::flightgear
{
    /*!
     * Config plugin for the Flightgear plugin.
     */
    class CSimulatorFlightgearConfig : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatorflightgearconfig.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorFlightgearConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorFlightgearConfig() {}

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // ns

#endif // guard

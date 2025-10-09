// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIG_H
#define SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIG_H

#include "gui/pluginconfig.h"

namespace swift::simplugin::emulated
{
    /*!
     * Window for setting up the FSX plugin.
     */
    class CSimulatorEmulatedConfig : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatoremulatedconfig.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorEmulatedConfig(QObject *parent = nullptr);

        //! Dtor
        ~CSimulatorEmulatedConfig() override = default;

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // namespace swift::simplugin::emulated

#endif // SWIFT_SIMPLUGIN_EMULATED_SIMULATOREMULATEDCONFIG_H

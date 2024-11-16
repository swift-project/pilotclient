// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_SIMULATOR_FSX_CONFIG_H
#define SWIFT_SIMPLUGIN_SIMULATOR_FSX_CONFIG_H

#include "gui/pluginconfig.h"
#include "misc/settingscache.h"

namespace swift::simplugin::fsx
{
    /*!
     * Window for setting up the FSX plugin.
     */
    class CSimulatorFsxConfig : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatorfsxconfig.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorFsxConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorFsxConfig() override {}

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // namespace swift::simplugin::fsx

#endif // guard

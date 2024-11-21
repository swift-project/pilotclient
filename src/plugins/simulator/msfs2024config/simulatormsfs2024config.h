// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_MSFS2024_SIMULATORMSFS2024CONFIG_H
#define SWIFT_SIMPLUGIN_MSFS2024_SIMULATORMSFS2024CONFIG_H

#include "gui/pluginconfig.h"

namespace swift::simplugin::msfs2024
{
    /*!
     * Window for setting up the MSFS2024 plugin.
     */
    class CSimulatorMsfs2024Config : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatormsfs2024config.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorMsfs2024Config(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorMsfs2024Config() {}

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // namespace swift::simplugin::msfs2024

#endif // guard

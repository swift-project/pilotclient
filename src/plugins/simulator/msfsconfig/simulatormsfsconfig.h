// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_MSFS_SIMULATORMSFSCONFIG_H
#define SWIFT_SIMPLUGIN_MSFS_SIMULATORMSFSCONFIG_H

#include "gui/pluginconfig.h"

namespace swift::simplugin::msfs
{
    /*!
     * Window for setting up the MSFS plugin.
     */
    class CSimulatorMsfsConfig : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatormsfsconfig.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorMsfsConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorMsfsConfig() {}

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // namespace swift::simplugin::msfs

#endif // guard

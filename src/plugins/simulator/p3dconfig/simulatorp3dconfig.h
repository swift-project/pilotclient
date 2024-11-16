// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_SIMPLUGIN_P3D_SIMULATORP3DCONFIG_H
#define SWIFT_SIMPLUGIN_P3D_SIMULATORP3DCONFIG_H

#include "gui/pluginconfig.h"

namespace swift::simplugin::p3d
{
    /*!
     * Window for setting up the P3D plugin.
     */
    class CSimulatorP3DConfig : public QObject, public swift::gui::IPluginConfig
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.swift-project.swiftgui.pluginconfiginterface" FILE "simulatorp3dconfig.json")
        Q_INTERFACES(swift::gui::IPluginConfig)

    public:
        //! Ctor
        CSimulatorP3DConfig(QObject *parent = nullptr);

        //! Dtor
        virtual ~CSimulatorP3DConfig() {}

        //! \copydoc swift::gui::IPluginConfig::createConfigWindow()
        swift::gui::CPluginConfigWindow *createConfigWindow(QWidget *parent) override;
    };
} // namespace swift::simplugin::p3d

#endif // guard

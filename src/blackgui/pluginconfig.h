// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_PLUGINCONFIG_H
#define BLACKGUI_PLUGINCONFIG_H

#include "blackgui/blackguiexport.h"

#include <QtPlugin>

class QWidget;

namespace BlackGui
{
    class CPluginConfigWindow;

    /*!
     * The interface for the plugin config.
     * The plugin config plugin is always called from the GUI process in order
     * to make it possible to create a config window.
     *
     * \sa BlackCore::ISimulator.
     */
    class BLACKGUI_EXPORT IPluginConfig
    {
    public:
        //! Dtor.
        virtual ~IPluginConfig() {}

        //! Creates a new config window and returns its pointer.
        virtual CPluginConfigWindow *createConfigWindow(QWidget *parent) = 0;
    };
}

Q_DECLARE_INTERFACE(BlackGui::IPluginConfig, "org.swift-project.blackgui.pluginconfiginterface")

#endif // guard

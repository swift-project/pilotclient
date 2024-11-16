// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_PLUGINCONFIG_H
#define SWIFT_GUI_PLUGINCONFIG_H

#include <QtPlugin>

#include "gui/swiftguiexport.h"

class QWidget;

namespace swift::gui
{
    class CPluginConfigWindow;

    /*!
     * The interface for the plugin config.
     * The plugin config plugin is always called from the GUI process in order
     * to make it possible to create a config window.
     *
     * \sa swift::core::ISimulator.
     */
    class SWIFT_GUI_EXPORT IPluginConfig
    {
    public:
        //! Dtor.
        virtual ~IPluginConfig() {}

        //! Creates a new config window and returns its pointer.
        virtual CPluginConfigWindow *createConfigWindow(QWidget *parent) = 0;
    };
} // namespace swift::gui

Q_DECLARE_INTERFACE(swift::gui::IPluginConfig, "org.swift-project.swiftgui.pluginconfiginterface")

#endif // guard

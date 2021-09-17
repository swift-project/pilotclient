/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

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

/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 *
 * Class based on qLed: Copyright (C) 2010 by P. Sereno, http://www.sereno-online.com
 */

//! \file

#ifndef BLACKGUI_PLUGINCONFIG_H
#define BCLAKGUI_PLUGINCONFIG_H

#include "blackgui/blackguiexport.h"
#include <QtWidgets/QWidget>
#include <QtPlugin>

namespace BlackGui
{
    class BLACKGUI_EXPORT IPluginConfig
    {

    public:
        virtual ~IPluginConfig() = default;

        virtual QWidget *createConfigWindow() = 0;

    };
}

Q_DECLARE_INTERFACE(BlackGui::IPluginConfig, "org.swift-project.blackgui.pluginconfiginterface")

#endif // guard

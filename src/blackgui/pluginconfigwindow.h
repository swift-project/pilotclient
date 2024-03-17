// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_PLUGINCONFIGWINDOW_H
#define BLACKGUI_PLUGINCONFIGWINDOW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/overlaymessagesframe.h"

class QShowEvent;

namespace BlackGui
{
    /*!
     * Base class for plugin config window.
     */
    class BLACKGUI_EXPORT CPluginConfigWindow : public COverlayMessagesFrame
    {
        Q_OBJECT
    public:
        //! No parent
        explicit CPluginConfigWindow(QWidget *parent);
    };
}

#endif // guard

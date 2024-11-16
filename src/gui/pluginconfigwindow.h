// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_PLUGINCONFIGWINDOW_H
#define SWIFT_GUI_PLUGINCONFIGWINDOW_H

#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"

class QShowEvent;

namespace swift::gui
{
    /*!
     * Base class for plugin config window.
     */
    class SWIFT_GUI_EXPORT CPluginConfigWindow : public COverlayMessagesFrame
    {
        Q_OBJECT
    public:
        //! No parent
        explicit CPluginConfigWindow(QWidget *parent);
    };
} // namespace swift::gui

#endif // guard

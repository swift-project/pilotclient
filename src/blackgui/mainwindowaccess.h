/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKGUI_MAINWINDOWACCESS_H
#define BLACKGUI_MAINWINDOWACCESS_H

#include "blackmisc/statusmessage.h"

namespace BlackGui
{
    /*!
     * Direct acccess to main window`s status bar, info bar and such
     */
    class IMainWindowAccess
    {
    public:
        //! Display in status bar
        virtual bool displayInStatusBar(const BlackMisc::CStatusMessage &message) = 0;

        //! Display in overlay window
        virtual bool displayInOverlayWindow(const BlackMisc::CStatusMessage &message) = 0;
    };
} // ns

#endif // guard
